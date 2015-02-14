//---------------------------------------------------------------------------------------
//
// Project: DirectionalityIndicator
//
// Copyright 2014-2015 Sebastian Eichelbaum (http://www.sebastian-eichelbaum.de)
//           2014-2015 Max Planck Research Group "Neuroanatomy and Connectivity"
//
// This file is part of DirectionalityIndicator.
//
// DirectionalityIndicator is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DirectionalityIndicator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DirectionalityIndicator. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------------------

#include <utility>
#include <map>
#include <string>

#define LogTag "core/ProcessingNetwork"
#include "Logger.h"
#include "Reader.h"

#include "commands/ReadFile.h"

#include "io/PlyReader.h"

#include "ProcessingNetwork.h"

namespace di
{
    namespace core
    {
        ProcessingNetwork::ProcessingNetwork():
            CommandQueue()
        {
        }

        ProcessingNetwork::~ProcessingNetwork()
        {
        }

        void ProcessingNetwork::start()
        {
            // Fill the list of readers. IMPORTANT: in the future, readers will be added dynamically (loaded from DLLs/SOs/DyLibs)
            m_reader.push_back( SPtr< di::io::PlyReader >( new di::io::PlyReader() ) );

            CommandQueue::start();
        }

        void ProcessingNetwork::stop( bool graceful )
        {
            CommandQueue::stop( graceful );
        }

        void ProcessingNetwork::addNetworkNode( SPtr< Algorithm > algorithm )
        {
            // Avoid concurrent access:
            std::lock_guard< std::mutex > lock( m_algorithmsMutex );
            // if already inside ... nothing happens.
            if( std::find( m_algorithms.begin(), m_algorithms.end(), algorithm ) != m_algorithms.end() )
            {
                return;
            }

            m_algorithms.push_back( algorithm );
        }

        void ProcessingNetwork::addNetworkNodeEdge( SPtr< Connection > connection )
        {
            // Avoid concurrent access:
            std::lock_guard< std::mutex > lock( m_connectionsMutex );
            std::lock_guard< std::mutex > lockAlgo( m_algorithmsMutex );

            // Algos involved in this connection:
            auto partners = findNodesOfEdge( connection );

            if( partners.first.size() != 1 )
            {
                throw std::logic_error( "Connection with multiple or no source algorithms are not allowed." );
            }
            if( partners.second.size() != 1 )
            {
                throw std::logic_error( "Connection with multiple or no target algorithms are not allowed." );
            }

            // also ensure that the given target connector is not yet connected somehow:
            for( auto c : m_connections )
            {
                auto theConnection = c.first;
                if( theConnection->getTarget() == connection->getTarget() )
                {
                    throw std::logic_error( "Connecting an input multiple times is not allowed." );
                }
            }

            // store
            m_connections[ connection ] = std::make_pair( partners.first.front(), partners.second.front() );
        }

        SPtr< di::commands::ReadFile > ProcessingNetwork::loadFile( const std::string& fileName, SPtr< CommandObserver > observer )
        {
            return commit(
                SPtr< di::commands::ReadFile >(
                    new di::commands::ReadFile( fileName, observer )
                )
            );
        }

        SPtr< di::commands::AddAlgorithm > ProcessingNetwork::addAlgorithm( SPtr< Algorithm > algorithm, SPtr< CommandObserver > observer )
        {
            return commit(
                SPtr< di::commands::AddAlgorithm >(
                    new di::commands::AddAlgorithm( algorithm, observer )
                )
            );
        }

        std::pair< SPtrVec< Algorithm >, SPtrVec< Algorithm > > ProcessingNetwork::findNodesOfEdge( SPtr< Connection > connection ) const
        {
            SPtrVec< Algorithm > targets;
            SPtrVec< Algorithm > sources;

            // check all algorithms
            visitAlgorithmsNoLock(
                [ & ]( SPtr< Algorithm > algo )
                {
                    if( algo->hasConnector( connection->getTarget() ) )
                    {
                        targets.push_back( algo );
                    }
                    if( algo->hasConnector( connection->getSource() ) )
                    {
                        sources.push_back( algo );
                    }
                }
            );

            return std::make_pair( sources, targets );
        }

        size_t ProcessingNetwork::countInputConnections( ConstSPtr< Algorithm > algorithm ) const
        {
            size_t result = 0;
            for( auto c : m_connections )
            {
                result += ( c.second.second == algorithm ) ? 1 : 0; // never directly add a bool.
            }
            return result;
        }

        SPtr< di::commands::Connect > ProcessingNetwork::connectAlgorithms( ConstSPtr< di::core::Algorithm > from,
                                                                            const std::string& fromConnector,
                                                                            ConstSPtr< di::core::Algorithm > to,
                                                                            const std::string& toConnector,
                                                                            SPtr< CommandObserver > observer
                                                                          )
        {
            return commit(
                SPtr< di::commands::Connect >(
                    new di::commands::Connect( from, fromConnector, to, toConnector, observer )
                )
            );
        }

        SPtr< di::commands::Connect > ProcessingNetwork::connectAlgorithms( ConstSPtr< di::core::ConnectorBase > from,
                                                                            SPtr< di::core::ConnectorBase > to,
                                                                            SPtr< CommandObserver > observer
                                                                          )
        {
            return commit(
              SPtr< di::commands::Connect >(
                    new di::commands::Connect( from, to, observer )
                )
            );
        }

        SPtr< di::commands::RunNetwork > ProcessingNetwork::runNetwork( SPtr< CommandObserver > observer )
        {
            return commit(
              SPtr< di::commands::RunNetwork >(
                    new di::commands::RunNetwork( observer )
                )
            );
        }

        void ProcessingNetwork::process( SPtr< Command > command )
        {
            // IMPORTANT: in the future, it is planned to make this dynamic... for now, it is sufficient to do it the hard-coded way as we need to get
            // started with the visualization itself.

            // NOTE: also keep in mind that a class CommandInterpreter would be useful to move those implementations to somewhere else as this grow
            // big fast.

            // Is a ReadFile command?
            SPtr< di::commands::ReadFile > readFileCmd = std::dynamic_pointer_cast< di::commands::ReadFile >( command );
            if( readFileCmd )
            {
                bool foundReader = false;    // did we find a reader?
                std::string fn = readFileCmd->getFilename();
                LogD << "Try loading: \"" << fn << "\"" << LogEnd;
                // iterate all known readers to find the best:
                for( auto aReader : m_reader )
                {
                    if( aReader->canLoad( fn ) )
                    {
                        // NOTE: exceptions get handled in CommandQueue
                        readFileCmd->setResult( aReader->load( fn ) );
                        foundReader = true;
                        break;
                    }
                }

                // Inform about failed command, as no reader was present
                if( !foundReader )
                {
                    readFileCmd->fail( "No suitable reader found for \"" + fn  + "\"."  );
                }
            }

            // Add a new algorithm?
            SPtr< di::commands::AddAlgorithm > addAlgorithmCmd = std::dynamic_pointer_cast< di::commands::AddAlgorithm >( command );
            if( addAlgorithmCmd )
            {
                // is a null?
                if( !addAlgorithmCmd->getAlgorithm() )
                {
                    addAlgorithmCmd->fail( "Null algorithms are not allowed." );
                }

                // add and done
                addNetworkNode( addAlgorithmCmd->getAlgorithm() );
            }

            // Connect a algorithm?
            SPtr< di::commands::Connect > connectCmd = std::dynamic_pointer_cast< di::commands::Connect >( command );
            if( connectCmd )
            {
                // is a null?
                if( !connectCmd->getFromConnector() )
                {
                    connectCmd->fail( "Need a source connector. Null pointer given." );
                }
                if( !connectCmd->getToConnector() )
                {
                    connectCmd->fail( "Need a target connector. Null pointer given." );
                }

                // add and done
                addNetworkNodeEdge( std::make_shared< Connection >( connectCmd->getFromConnector(), connectCmd->getToConnector() ) );
            }

            // Run the network again?
            SPtr< di::commands::RunNetwork > runCmd = std::dynamic_pointer_cast< di::commands::RunNetwork >( command );
            if( runCmd )
            {
                // Call the proper function. Keep in mind that this is a temporary solution. The will be done by a scheduler in the future.
                runNetworkImpl();
            }
        }

        void ProcessingNetwork::runNetworkImpl()
        {
            // Avoid concurrent access:
            std::lock_guard< std::mutex > lockAlgo( m_algorithmsMutex );
            std::lock_guard< std::mutex > lockCon( m_connectionsMutex );

            LogD << "Running processing network. Propagating changes." << LogEnd;

            // Find sources ( algorithms without input connectors ) and non-connected algorithms. They are the start of the recursive propagation.
            for( auto algo : m_algorithms )
            {
                // source?
                if( algo->isSource() || ( countInputConnections( algo ) == 0 ) )
                {
                    // recurse along connection
                    std::map< SPtr< Algorithm >, size_t > visits; // count visits to handle cycles.
                    visitNetworkNoLock( algo, visits,
                        [ & ]( SPtr< Algorithm > algo )
                        {
                            if( algo->isActive() )
                            {
                                LogI << "Running algorithm \"" << algo->getName() << "\"." << LogEnd;
                                algo->process();
                            }
                            else
                            {
                                LogI << "Ignoring inactive algorithm \"" << algo->getName() << "\"." << LogEnd;
                            }
                        },
                        [ & ]( SPtr< Connection > con )
                        {
                            // Propagate only if target is active:
                            SPtr< Algorithm > target = m_connections[ con ].second;
                            if( target->isActive() )
                            {
                                bool result = con->propagate();
                                LogD << "Propagation: " << *m_connections[ con ].first << ":" << con->getSource()->getName() << " -> " <<
                                                          *m_connections[ con ].second << ":" << con->getTarget()->getName() <<
                                                          " - Result: " << result << LogEnd;
                                return result;
                            }

                            LogD << "Propagation skipped (inactive target): "
                                 << *m_connections[ con ].first << ":" << con->getSource()->getName() << " -> " <<
                                    *m_connections[ con ].second << ":" << con->getTarget()->getName() << LogEnd;
                            return false;
                        }
                    );
                }
            }
        }
    }
}

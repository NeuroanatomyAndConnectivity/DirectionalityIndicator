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

#include <di/core/Reader.h>
#include <di/core/ObserverCallback.h>

#include <di/commands/ReadFile.h>

#include <di/io/PlyReader.h>

#include "ProcessingNetwork.h"

#include <di/core/Logger.h>
#define LogTag "core/ProcessingNetwork"

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

        void ProcessingNetwork::onDirtyNetwork()
        {
            std::unique_lock< std::mutex > lock( m_onDirtyObserversMutex );
            for( auto observer : m_onDirtyObservers )
            {
                observer->notify();
            }
        }

        void ProcessingNetwork::observeOnDirty( SPtr< Observer > observer )
        {
            std::unique_lock< std::mutex > lock( m_onDirtyObserversMutex );

            // if already inside ... nothing happens.
            if( std::find( m_onDirtyObservers.begin(), m_onDirtyObservers.end(), observer ) != m_onDirtyObservers.end() )
            {
                return;
            }

            m_onDirtyObservers.push_back( observer );
        }

        void ProcessingNetwork::removeObserverOnDirty( SPtr< Observer > observer )
        {
            std::unique_lock< std::mutex > lock( m_onDirtyObserversMutex );
            m_onDirtyObservers.erase( std::remove( m_onDirtyObservers.begin(),
                                                   m_onDirtyObservers.end(),
                                                   observer ), m_onDirtyObservers.end() );
        }

        void ProcessingNetwork::start()
        {
            m_onDirtyObserver = std::make_shared< ObserverCallback >( [ this ](){ onDirtyNetwork(); } );

            // Fill the list of readers. IMPORTANT: in the future, readers will be added dynamically (loaded from DLLs/SOs/DyLibs)
            m_reader.push_back( SPtr< di::io::PlyReader >( new di::io::PlyReader() ) );

            CommandQueue::start();
        }

        void ProcessingNetwork::stop( bool graceful )
        {
            CommandQueue::stop( graceful );
        }

        void ProcessingNetwork::addVisualization( SPtr< Visualization > visualization )
        {
            if( !visualization )
            {
                return;
            }

            // Avoid concurrent access:
            std::lock_guard< std::mutex > lock( m_visualizationsMutex );
            // if already inside ... nothing happens.
            if( std::find( m_visualizations.begin(), m_visualizations.end(), visualization ) != m_visualizations.end() )
            {
                return;
            }

            m_visualizations.push_back( visualization );
        }

        void ProcessingNetwork::addNetworkNode( SPtr< Algorithm > algorithm )
        {
            // Avoid concurrent access:
            std::unique_lock< std::mutex > lock( m_algorithmsMutex );
            // if already inside ... nothing happens.
            if( std::find( m_algorithms.begin(), m_algorithms.end(), algorithm ) != m_algorithms.end() )
            {
                return;
            }

            // Register own observer
            algorithm->observe( m_onDirtyObserver );

            m_algorithms.push_back( algorithm );

            lock.unlock();

            // Is a visualization?
            addVisualization( std::dynamic_pointer_cast< Visualization >( algorithm ) );
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
            return loadFile( nullptr, fileName, observer );
        }

        SPtr< di::commands::ReadFile > ProcessingNetwork::loadFile( SPtr< Reader > reader, const std::string& fileName,
                                                                    SPtr< CommandObserver > observer )
        {
            return commit(
                SPtr< di::commands::ReadFile >(
                    new di::commands::ReadFile( reader, fileName, observer )
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

        bool ProcessingNetwork::isUpdateRequested() const
        {
            // Avoid concurrent access:
            std::lock_guard< std::mutex > lock( m_algorithmsMutex );
            for( auto algo : m_algorithms )
            {
                if( algo->isUpdateRequested() )
                {
                    return true;
                }
            }
            return false;
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
            // Source are not connected
            if( algorithm->isSource() )
            {
                return 0;
            }

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
                std::string fn = readFileCmd->getFilename();
                LogD << "Try loading: \"" << fn << "\"" << LogEnd;

                // iterate all known readers to find the best or use the given one
                auto reader = readFileCmd->getReader();
                if( !reader )
                {
                    for( auto aReader : m_reader )
                    {
                        if( aReader->canLoad( fn ) )
                        {
                            reader = aReader;
                            break;
                        }
                    }
                }
                else if( !reader->canLoad( fn ) )   // also ensure the provided reader can actually read this file
                {
                    reader = nullptr;
                }

                if( reader )
                {
                    // NOTE: exceptions get handled in CommandQueue
                    readFileCmd->setResult( reader->load( fn ) );
                }
                else
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

        std::vector<
            std::pair<
                std::vector< SPtr< Algorithm > >,
                std::vector< SPtr< Connection > >
            >
        > ProcessingNetwork::buildRunOrder()
        {
            typedef std::vector< SPtr< Algorithm > > AlgoVec;

            // Associate each algorithm with a layer
            std::map< SPtr< Algorithm >, int > algoLayer;
            // Associate each algorithm with its incoming connected algorithms
            std::map< SPtr< Algorithm >, AlgoVec > incomingAlgos;

            // Find sources ( algorithms without input connectors ) and non-connected algorithms.
            for( auto algo : m_algorithms )
            {
                // use this iteration to init the algorithm - layer map:
                algoLayer[ algo ] = -1;

                // get all incoming connections
                for( auto c : m_connections )
                {
                    // is the target algorithm of this connection our current algorithm?
                    if( c.second.second == algo )
                    {
                        incomingAlgos[ algo ].push_back( c.second.first );
                    }
                }

                // source or not connected? Always first layer.
                if( countInputConnections( algo ) == 0 )
                {
                    algoLayer[ algo ] = 0;
                }
            }

            // Do nice stuff to assign each algorithm
            bool stop = false;
            int globalMaxLayer = 0;
            while( !stop )
            {
                bool allAssigned = true;
                for( auto algo : m_algorithms )
                {
                    // Skip algorithms that have been assigned earlier.
                    if( algoLayer[ algo ] >= 0 )
                    {
                        continue;
                    }

                    // get all incoming connections
                    int maxLayer = -1;
                    bool valid = true;
                    for( auto source : incomingAlgos[ algo ] )
                    {
                        // The max layer is valid until we find a source that was not yet assigned.
                        valid = valid && ( algoLayer[ source ] >= 0 );
                        maxLayer = std::max( maxLayer, algoLayer[ source ] );
                    }

                    // All sources assigned?
                    if( valid )
                    {
                        // Yes. We can 	unambiguously assign this algo too:
                        algoLayer[ algo ] = maxLayer + 1;
                        // the largest ever layer:
                        globalMaxLayer = std::max( globalMaxLayer, maxLayer + 1 );
                    }
                    else
                    {
                        allAssigned = false;
                    }
                }
                stop = allAssigned;
            }

            // The caller asked for the inverse list -> layer to algorithm
            std::vector<
                std::pair<
                    std::vector< SPtr< Algorithm > >,
                    std::vector< SPtr< Connection > >
                >
            > result( globalMaxLayer + 1 );

            // Fill
            for( auto elem : algoLayer )
            {
                auto algo = elem.first;

                // Store the algorithms
                result[ elem.second ].first.push_back( algo );
                // And all outgoing connections starting at this layer
                for( auto con : m_connections )
                {
                    // If the connection source is the current algo
                    if( con.second.first == algo )
                    {
                        result[ elem.second ].second.push_back( con.first );
                    }
                }
            }

            return result;
        }

        void ProcessingNetwork::runNetworkImpl()
        {
            // Avoid concurrent access:
            std::lock_guard< std::mutex > lockAlgo( m_algorithmsMutex );
            std::lock_guard< std::mutex > lockCon( m_connectionsMutex );

            // Get the execution order:
            auto executionLayers = buildRunOrder();

            // Some nice output
            size_t l = 0;
            for( auto layer : executionLayers )
            {
                LogD << "Layer " << l << LogEnd;
                for( auto algo : layer.first )
                {
                    LogD << "    - " << algo->getName() << " ( Dirty: " << algo->isUpdateRequested() << ", Active: " << algo->isActive() << " )"
                         << LogEnd;
                }
                l++;
            }

            LogD << "Running processing network. Propagating changes." << LogEnd;

            // Iterate each layer:
            std::map< SPtr< Algorithm >, bool > dataPropagated;
            for( auto layer : executionLayers )
            {
                // NOTE: as all algorithms in one layer are not dependent on each other, it is possible to use std::async and similar facilities here
                // to parallelize execution. Not yet done.
                for( auto algo : layer.first )
                {
                    if( algo->isActive() && ( algo->isUpdateRequested() || dataPropagated[ algo ] ) )
                    {
                        LogI << "Running algorithm \"" << algo->getName() << "\"." << LogEnd;
                        algo->run();
                    }
                    else
                    {
                        LogI << "Ignoring inactive algorithm \"" << algo->getName() << "\"." << LogEnd;
                    }
                }

                for( auto con : layer.second )
                {
                    bool result = con->propagate();
                    LogD << "Propagation: " << *m_connections[ con ].first << ":" << *con << ":" << *m_connections[ con ].second <<
                                               " - Result: " << result << LogEnd;

                    dataPropagated[ m_connections[ con ].second ] = result;
                }
            }
        }
    }
}

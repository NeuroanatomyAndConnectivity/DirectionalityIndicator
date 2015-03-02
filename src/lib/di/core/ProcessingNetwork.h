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

#ifndef DI_PROCESSINGNETWORK_H
#define DI_PROCESSINGNETWORK_H

#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <utility>

#include <di/Types.h>

#include <di/core/Reader.h>
#include <di/core/Command.h>
#include <di/core/CommandObserver.h>
#include <di/core/CommandQueue.h>
#include <di/core/Algorithm.h>
#include <di/core/Visualization.h>
#include <di/core/Connection.h>

// All commands provided as convenience wrapper.
#include <di/commands/ReadFile.h>
#include <di/commands/AddAlgorithm.h>
#include <di/commands/Connect.h>
#include <di/commands/RunNetwork.h>

namespace di
{
    namespace core
    {
        /**
         * Container class to control a data-flow network.
         *
         * The container itself runs in its own thread and controls the pipeline. It propagates updates and newly loaded data through the network
         * without blocking. All operations on the network are done via commands.
        */
        class ProcessingNetwork: public CommandQueue
        {
        public:
            /**
             * Create an empty processing network container.
             */
            ProcessingNetwork();

            /**
             * Clean up. This stops all visualizations if any and cleans up memory.
             */
            virtual ~ProcessingNetwork();

            /**
             * Start the visualization container. Does nothing if the thread is already running.
             */
            virtual void start();

            /**
             * Stop the visualization container. This causes all algorithms to be informed about the shutdown. The function blocks until the thread
             * stopped. The function immediately returns if not thread is running (anymore).
             *
             * \note call from outside the container's thread only.
             *
             * \param graceful if false, the queue will be stopped immediately. No further commands will be processed.
             */
            virtual void stop( bool graceful = true );

            /**
             * This allows to visit each algorithm inside the network. The list of algorithms is locked and copied before visiting. This means, the
             * visitor is called for a snapshot of the algorithms at the time of calling. This prohibits long-running visitors to block the whole
             * command queue.
             *
             * \note the function locks access to the algorithm list. Avoid nested calls.
             *
             * \tparam VisitorType the visitor type. Something that takes a SPtr< di::core::Algorithm > as parameter.
             * \param visitor the visitor instance itself.
             */
            template< typename VisitorType >
            void visitAlgorithms( VisitorType visitor );

            /**
             * This allows to visit each algorithm, which is a visualization inside the network. The list of algorithms is locked and copied before
             * visiting. This means, the visitor is called for a snapshot of the algorithms at the time of calling. This prohibits long-running
             * visitors to block the whole command queue.
             *
             * \note the function locks access to the algorithm list. Avoid nested calls.
             *
             * \tparam VisitorType the visitor type. Something that takes a SPtr< di::core::Visualization > as parameter.
             * \param visitor the visitor instance itself.
             */
            template< typename VisitorType >
            void visitVisualizations( VisitorType visitor );

            /**
             * \copydoc visitAlgorithms
             */
            template< typename VisitorType >
            void visitAlgorithms( VisitorType visitor ) const;

            /**
             * \copydoc visitVisualizations
             */
            template< typename VisitorType >
            void visitVisualizations( VisitorType visitor ) const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Some convenience methods. The wrap around command-creation. The specific command is mentioned as a note.

            /**
             * Load the specified file. This operation is non-blocking and runs in this container's thread.
             *
             * \note equals to committing a di::commands::ReadFile( fileName, observer );
             *
             * \param fileName the file to load
             * \param observer the observer that gets informed about changes. Can be omitted.
             *
             * \return the command instance. Not needed to keep this.
             *
             */
            virtual SPtr< di::commands::ReadFile > loadFile( const std::string& fileName, SPtr< CommandObserver > observer = nullptr );

            /**
             * Add an algorithm to the network. This operation is asynchronous. If you need to get informed about success, specify a observer.
             *
             * \note equals to committing a di::commands::AddAlgorithm( algorithm );
             *
             * \param algorithm the algorithm to add.
             * \param observer the observer that gets informed about the changes. Can be omitted.
             *
             * \return the command instance. Not needed to keep this.
             */
            virtual SPtr< di::commands::AddAlgorithm > addAlgorithm( SPtr< Algorithm > algorithm, SPtr< CommandObserver > observer = nullptr );

            /**
             * Create a connection between two algorithm connectors. This operation is asynchronous. If you need to get informed about success,
             * specify a observer.
             *
             * \note equals to committing a di::commands::Connect( from, fromConnector, to, toConnector ) );
             *
             * \note command fails if any argument is invalid. The function itself does not fail. This is useful if the specified module not yet set
             * its outputs properly (it is still in the queue).
             *
             * \param from the source algorithm
             * \param fromConnector the connector name of the source connector
             * \param to the target algorithm
             * \param toConnector the connector name of the target connector
             * \param observer the observer that gets informed about the changes. Can be omitted.
             *
             * \return the command instance. Not needed to keep this.
             */
            virtual SPtr< di::commands::Connect > connectAlgorithms( ConstSPtr< di::core::Algorithm > from,
                                                                     const std::string& fromConnector,
                                                                     ConstSPtr< di::core::Algorithm > to,
                                                                     const std::string& toConnector,
                                                                     SPtr< CommandObserver > observer = nullptr
                                                                   );

            /**
             * Create a connection between two algorithm connectors. This operation is asynchronous. If you need to get informed about success,
             * specify a observer.
             *
             * \note equals to committing a di::commands::Connect( from, to );
             *
             * \note command fails if any argument is invalid. The function itself does not fail. This is useful if the specified module not yet set
             * its outputs properly (it is still in the queue).
             *
             * \param from the source connector
             * \param to the target connector
             * \param observer the observer that gets informed about the changes. Can be omitted.
             *
             * \return the command instance. Not needed to keep this.
             */
            virtual SPtr< di::commands::Connect > connectAlgorithms( ConstSPtr< di::core::ConnectorBase > from,
                                                                     SPtr< di::core::ConnectorBase > to,
                                                                     SPtr< CommandObserver > observer = nullptr
                                                                   );

            /**
             * Call to re-run the whole network. This is only a temporary solution. Will be replaced by a proper scheduler soon.
             *
             * \param observer the observer that gets informed about the changes. Can be omitted.
             *
             * \return the command instance. Not needed to keep this.
             */
            virtual SPtr< di::commands::RunNetwork > runNetwork( SPtr< CommandObserver > observer = nullptr );
        protected:
            /**
             * Process the specified command. Use Command::handle to mark the command as being handled.
             *
             * \param command the command to handle
             */
            virtual void process( SPtr< Command > command );

            /**
             * Add the given algorithm to the network. If it already is inside, it is ignored. This method is extremely fault tolerant and does not
             * complain about its argument. The only thing to keep in mind is that after committing a node, it belongs to this graph. It is  managed
             * by this graph and you should not add it to another one again.
             *
             * \note this method is not thread-safe. Call only from within the processing thread of this queue (via commands).
             *
             * \param algorithm the algorithm to add.
             */
            virtual void addNetworkNode( SPtr< Algorithm > algorithm );

            /**
             * Add the given connection to the network. If it already is inside, it is ignored. You can even add connections that use connectors
             * associated with algorithms that are not inside the network. This is practical for breaking out data. This method is extremely fault
             * tolerant and does not complain about its argument. The only thing to keep in mind is that after committing a connection, it belongs
             * to this graph. It is  managed by this graph and you should not add it to another one again.
             *
             * \param connection the connection to setup.
             * \throw std::logic_error connections without a source or target and connections with multiple sources are invalid
             */
            virtual void addNetworkNodeEdge( SPtr< Connection > connection );

            /**
             * Find the nodes that are involved in the given edge (connection).
             *
             * \param connection the connection to check
             *
             * \note the method does not lock the m_algorithms member, as it assumes that the caller already has done it.
             *
             * \return pair contains the source algorithms and the target algorithms. In most cases, source will be a single entry. But it is not
             * guaranteed.
             */
            virtual std::pair< SPtrVec< Algorithm >, SPtrVec< Algorithm > > findNodesOfEdge( SPtr< Connection > connection ) const;

            /**
             * Count the currently defined connections to an algorithm's inputs.
             *
             * \param algorithm the algorithm to count the connections for.
             *
             * \note not thread-safe. Only use from within processing thread, or after acquiring m_connectionsMutex.
             *
             * \return the number of connections
             */
            virtual size_t countInputConnections( ConstSPtr< Algorithm > algorithm ) const;

            /**
             * Re-run the whole network.
             */
            virtual void runNetworkImpl();

            /**
             * \note does not lock!
             *
             * \copydoc visitVisualizations
             */
            template< typename VisitorType >
            void visitAlgorithmsNoLock( VisitorType visitor ) const;

            /**
             * \note does not lock!
             *
             * \copydoc visitVisualizations
             */
            template< typename VisitorType >
            void visitAlgorithmsNoLock( VisitorType visitor );

            /**
             * Follow the network along the connections, starting at the given algorithm.
             *
             * \tparam NodeVisitorType The node visitor type
             * \tparam EdgeVisitorType The edge visitor type
             * \param start the current node
             * \param visits count the visits of each node to handle cycles
             * \param nodeCallback callback for each node
             * \param edgeCallback callback for each edge. If the callback returns false, the edge is not followed.
             */
            template< typename NodeVisitorType, typename EdgeVisitorType >
            void visitNetworkNoLock( SPtr< Algorithm > start, std::map< SPtr< Algorithm >, size_t >& visits,    // NOLINT: non-const reference
                                     NodeVisitorType nodeCallback, EdgeVisitorType edgeCallback );
        private:
            /**
             * A list of all known readers.
             */
            SPtrVec< Reader > m_reader;

            /**
             * All the algorithms managed by this network instance.
             */
            SPtrVec< Algorithm > m_algorithms;

            /**
             * Mutex to secure access to m_algorithms
             */
            mutable std::mutex m_algorithmsMutex;

            /**
             * Mutex to secure access to m_connections
             */
            mutable std::mutex m_connectionsMutex;

            /**
             * The list of all connections. In other words, the edges of the multigraph. It associates a connection with source and target algorithm.
             */
            std::map< SPtr< Connection >,
                        std::pair< SPtr< Algorithm >,
                                   SPtr< Algorithm >
                                 >
                    > m_connections;
        };

        template< typename VisitorType >
        void ProcessingNetwork::visitAlgorithmsNoLock( VisitorType visitor )
        {
            // Iterate and call visitor on each
            for( auto algo : m_algorithms )
            {
                visitor( algo );
            }
        }

        template< typename VisitorType >
        void ProcessingNetwork::visitAlgorithmsNoLock( VisitorType visitor ) const
        {
            const_cast< ProcessingNetwork* >( this )->visitAlgorithmsNoLock( visitor );
        }

        template< typename VisitorType >
        void ProcessingNetwork::visitAlgorithms( VisitorType visitor )
        {
            // Avoid concurrent access:
            std::unique_lock< std::mutex > lock( m_algorithmsMutex );

            // Copy
            SPtrVec< Algorithm > algorithmsSnapshot( m_algorithms );

            // Unlock to avoid long-running visitors to block the network
            lock.unlock();

            // Iterate and call visitor on each
            for( auto algo : algorithmsSnapshot )
            {
                visitor( algo );
            }
        }

        template< typename VisitorType >
        void ProcessingNetwork::visitAlgorithms( VisitorType visitor ) const
        {
            const_cast< ProcessingNetwork* >( this )->visitAlgorithms( visitor );
        }

        template< typename VisitorType >
        void ProcessingNetwork::visitVisualizations( VisitorType visitor )
        {
            // Wrap
            visitAlgorithms( [&]( SPtr< Algorithm > algorithm )
                             {
                                 auto vis = std::dynamic_pointer_cast< Visualization >( algorithm );
                                 if( vis )
                                 {
                                     visitor( vis );
                                 }
                             }
            );
        }

        template< typename VisitorType >
        void ProcessingNetwork::visitVisualizations( VisitorType visitor ) const
        {
            const_cast< ProcessingNetwork* >( this )->visitVisualizations( visitor );
        }

        template< typename NodeVisitorType, typename EdgeVisitorType >
        void ProcessingNetwork::visitNetworkNoLock( SPtr< Algorithm > start, std::map< SPtr< Algorithm >, size_t >& visits, // NOLINT: non-const reference
                                                    NodeVisitorType nodeCallback, EdgeVisitorType edgeCallback )
        {
            // avoid loops by checking if we have been here at most n times, where n is the number of connected inputs. ( and at least 1 times )
            size_t numConnections = countInputConnections( start );
            if( ( visits[ start ] >= 1 ) && ( visits[ start ] >= numConnections ) )
            {
                return;
            }

            // apply visitor to current node:
            visits[ start ]++;
            nodeCallback( start );

            // get all outgoing connections
            for( auto c : m_connections )
            {
                // is the source algorithm of this connection our current algorithm?
                if( c.second.first == start )
                {
                    // yes. Follow the connection if the callback allows us
                    if( edgeCallback( c.first ) )
                    {
                        visitNetworkNoLock( c.second.second, visits, nodeCallback, edgeCallback );
                    }
                }
            }
        }
    }
}

#endif  // DI_PROCESSINGNETWORK_H

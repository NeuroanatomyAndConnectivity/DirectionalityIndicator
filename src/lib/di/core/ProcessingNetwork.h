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
#include <di/core/State.h>

// All commands provided as convenience wrapper.
#include <di/commands/ReadFile.h>
#include <di/commands/AddAlgorithm.h>
#include <di/commands/Connect.h>
#include <di/commands/RunNetwork.h>
#include <di/commands/Callback.h>
#include <di/commands/QueryState.h>

namespace di
{
    namespace algorithms
    {
        class DataInject;
    }

    namespace core
    {
        class Reader;
        class ObserverCallback;

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
             * \copydoc loadFile
             *
             * \param reader the reader instance to use. Can be nullptr to use known readers automatically.
             */
            virtual SPtr< di::commands::ReadFile > loadFile( SPtr< Reader > reader, const std::string& fileName,
                                                             SPtr< CommandObserver > observer = nullptr );

            /**
             * \copydoc loadFile
             *
             * \param inject the injector used to insert the data into the network. If null, it is your task to handle this.
             */
            virtual SPtr< di::commands::ReadFile > loadFile( SPtr< Reader > reader, const std::string& fileName,
                                                             SPtr< di::algorithms::DataInject > inject = nullptr,
                                                             SPtr< CommandObserver > observer = nullptr );

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

            /**
             * Commit a callback command. The command only triggers the observer.
             *
             * \param observer the observer
             *
             * \return the command
             */
            virtual SPtr< di::commands::Callback > callback( SPtr< CommandObserver > observer );

            /**
             * Commit a callback command. The command only triggers the observer.
             *
             * \param callback the callback triggered on execution inside the network thread.
             *
             * \return the command
             */
            virtual SPtr< di::commands::Callback > callback( std::function< void() > callback );

            /**
             * Is an update requested? Convenience method to check all algorithms at once.
             *
             * \return true if at least one algorithm returns true for \ref Algorithm::isUpdateRequested.
             */
            virtual bool isUpdateRequested() const;

            /**
             * Register functor to call whenever the processing network is marked dirty. Be aware that the callback needs to point to an existing
             * class instance/function at all times. Use removeOnDirty to unregister before deleting.
             *
             * \param observer the callback
             */
            void observeOnDirty( SPtr< Observer > observer );

            /**
             * De-register the callback from on-dirty events.
             *
             * \param observer the callback to remove.
             */
            void removeObserverOnDirty( SPtr< Observer > observer );

            /**
             * Get the state object representing this object at the moment of the call.
             *
             * \return  the state
             */
            virtual SPtr< di::commands::QueryState > queryState( SPtr< CommandObserver > observer = nullptr );

            /**
             * Apply the state to this instance.
             *
             * \param state the state to set
             *
             * \return  true if everything was fine.
             */
            bool setState( const di::core::State& state );

            /**
             * Get the state object representing this object at the moment of the call.
             *
             * \return  the state
             */
            di::core::State getState() const;

        protected:
            /**
             * Process the specified command. Use Command::handle to mark the command as being handled.
             *
             * \param command the command to handle
             */
            virtual void process( SPtr< Command > command );

            /**
             * Add the given visualization to the vis-queue. If it already is inside, it is ignored.
             *
             * \note this method is not thread-safe. Call only from within the processing thread of this queue (via commands).
             *
             * \param visualization the visualization to add.
             */
            virtual void addVisualization( SPtr< Visualization > visualization );

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
             * Called by the m_onDirtyObserver whenever an algorithm gets dirty.
             */
            virtual void onDirtyNetwork();

            /**
             * Order algorithms to solve dependencies during execution. REQUIRES that the caller already obtained the m_algorithmsMutex and the
             * m_connectionsMutex.
             *
             * \return a map between a layer and the algorithms on it. All algorithms of one layer only depend on algorithms of one of the above
             * layers.
             */
            std::vector<
                std::pair<
                    std::vector< SPtr< Algorithm > >,
                    std::vector< SPtr< Connection > >
                >
            > buildRunOrder();

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
             * All the visualizations managed by this network instance.
             */
            SPtrVec< Visualization > m_visualizations;

            /**
             * Mutex to secure access to m_algorithms
             */
            mutable std::mutex m_algorithmsMutex;

            /**
             * Mutex to secure access to m_visualizations
             */
            mutable std::mutex m_visualizationsMutex;

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

            /**
             * Observe the dirty state of algorithms.
             */
            SPtr< ObserverCallback > m_onDirtyObserver = nullptr;

            /**
             * Securing the onDirty callback list.
             */
            std::mutex m_onDirtyObserversMutex;

            /**
             * The list of callbacks to call on dirty events.
             */
            std::vector< SPtr< Observer > > m_onDirtyObservers;
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
            // Avoid concurrent access:
            std::unique_lock< std::mutex > lock( m_visualizationsMutex );

            // Copy
            SPtrVec< Visualization > visSnapshot( m_visualizations );

            // Unlock to avoid long-running visitors to block the network
            lock.unlock();

            // Iterate and call visitor on each
            for( auto vis : visSnapshot )
            {
                visitor( vis );
            }
        }

        template< typename VisitorType >
        void ProcessingNetwork::visitVisualizations( VisitorType visitor ) const
        {
            const_cast< ProcessingNetwork* >( this )->visitVisualizations( visitor );
        }
    }
}

#endif  // DI_PROCESSINGNETWORK_H


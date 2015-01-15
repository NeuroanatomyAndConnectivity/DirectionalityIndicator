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

#ifndef PROCESSINGNETWORK_H
#define PROCESSINGNETWORK_H

#include <string>
#include <thread>
#include <vector>

#include "Types.h"

#include "Reader.h"
#include "Command.h"
#include "CommandObserver.h"
#include "CommandQueue.h"
#include "Algorithm.h"
#include "Connection.h"

// All commands provided as convenience wrapper.
#include "commands/ReadFile.h"
#include "commands/AddAlgorithm.h"
#include "commands/Connect.h"

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
             *
             * \return the command instance. Not needed to keep this.
             */
            virtual SPtr< di::commands::Connect > connectAlgorithms( ConstSPtr< di::core::Algorithm > from,
                                                                     const std::string& fromConnector,
                                                                     ConstSPtr< di::core::Algorithm > to,
                                                                     const std::string& toConnector
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
             *
             * \return the command instance. Not needed to keep this.
             */
            virtual SPtr< di::commands::Connect > connectAlgorithms( ConstSPtr< di::core::ConnectorBase > from,
                                                                     ConstSPtr< di::core::ConnectorBase > to
                                                                   );

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
             * \note this method is not thread-safe. Call only from within the processing thread of this queue (via commands).
             *
             * \param connection the connection to setup.
             */
            virtual void addNetworkNodeEdge( SPtr< Connection > connection );

        private:
            /**
             * A list of all known readers.
             */
            std::vector< SPtr< Reader > > m_reader;

            /**
             * All the algorithms managed by this network instance.
             */
            SPtrSet< Algorithm > m_algorithms;

            /**
             * The list of all connections. In other words, the edges of the multigraph.
             */
            SPtrSet< Connection > m_connections;
        };
    }
}

#endif  // PROCESSINGNETWORK_H


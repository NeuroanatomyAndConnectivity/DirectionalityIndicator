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

// All commands provided as convenience wrapper.
#include "commands/ReadFile.h"
#include "commands/AddAlgorithm.h"

namespace di
{
    namespace core
    {
        /**
         * Container class to control a data-flow visualization network. For now, there is only one data-flow possible. It is hard-coded.
         * This will change to a dynamic network if the application gets extended.
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
             */
            virtual void stop();

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
             * \return the command
             */
            virtual SPtr< di::commands::ReadFile > loadFile( const std::string& fileName, SPtr< CommandObserver > observer = nullptr );

            /**
             * Add an algorithm to the network. This operation is asynchronous. If you need to get informed about success, specify a observer.
             *
             * \note equals to committing a di::commands::AddAlgorithm( algorithm );
             *
             * \param algorithm the algorithm to add.
             * \param observer the observer that gets informed about the changes. Can be omitted.
             */
            virtual SPtr< di::commands::AddAlgorithm > addAlgorithm( SPtr< Algorithm > algorithm, SPtr< CommandObserver > observer = nullptr );

        protected:
            /**
             * Process the specified command. Use Command::handle to mark the command as being handled.
             *
             * \param command the command to handle
             */
            virtual void process( SPtr< Command > command );

        private:
            /**
             * A list of all known readers.
             */
            std::vector< SPtr< Reader > > m_reader;
        };
    }
}

#endif  // PROCESSINGNETWORK_H


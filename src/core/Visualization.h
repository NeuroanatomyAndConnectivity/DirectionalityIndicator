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

#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <string>
#include <thread>

#include "Types.h"

#include "Command.h"
#include "CommandObserver.h"
#include "CommandQueue.h"

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
        class Visualization: public CommandQueue
        {
        public:
            /**
             * Create an empty visualization container.
             */
            Visualization();

            /**
             * Clean up. This stops all visualizations if any and cleans up memory.
             */
            virtual ~Visualization();

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

            /**
             * Load the specified mesh file. This operation is non-blocking and runs in this container's thread.
             *
             * \param fileName the file to load
             * \param observer the observer that gets informed about changes. Can be omitted.
             */
            virtual void loadMesh( const std::string& fileName, SPtr< CommandObserver > observer = nullptr );

            /**
             * Load the specified label file. This operation is non-blocking and runs in this container's thread.
             *
             * \param fileName the file to load
             * \param observer the observer that gets informed about changes. Can be omitted.
             */
            virtual void loadLabels( const std::string& fileName, SPtr< CommandObserver > observer = nullptr );

        protected:
            /**
             * Process the specified command. Use Command::handle to mark the command as being handled.
             *
             * \param command the command to handle
             */
            virtual void process( SPtr< Command > command );

        private:
        };
    }
}

#endif  // VISUALIZATION_H


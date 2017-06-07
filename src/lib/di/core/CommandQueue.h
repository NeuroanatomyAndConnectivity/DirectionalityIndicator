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

#ifndef DI_COMMANDQUEUE_H
#define DI_COMMANDQUEUE_H

#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <thread>

#include <di/Types.h>

#include <di/core/Command.h>

namespace di
{
    namespace core
    {
        /**
         * Implements a command queue. Commit commands to the queue and they will be processed in a separate thread. Class is abstract.
        */
        class CommandQueue
        {
        public:
            /**
             * Create an empty command queue. This does not start the queue. Use \ref start() to start processing.
             * After construction, the queue already accepts commands.
             */
            CommandQueue();

            /**
             * Clean up. This stops the queue and removes remaining commands.
             */
            virtual ~CommandQueue();

            /**
             * Start queue processing.
             */
            virtual void start();

            /**
             * Gracefully stop the queue by processing the remaining commands and refusing new ones.
             *
             * \param graceful if false, the queue will be stopped immediately. No further commands will be processed.
             */
            virtual void stop( bool graceful = true );

            /**
             * Commit the command to the queue.
             *
             * \param command the command to commit.
             *
             * \return the command itself. Useful for one-line creation and storage.
             */
            template< typename CommandType >
            SPtr< CommandType > commit( SPtr< CommandType > command )
            {
                // grab lock
                std::unique_lock< std::mutex > theLock( m_commandQueueMutex );

                // add and notify processing thread ...
                m_commandQueue.push_back( command );

                // Change command state.
                command->waiting();

                // Done.
                theLock.unlock();

                // Notify thread
                notifyThread();

                return command;
            }

        protected:
            /**
             * Thread method. Runs inside the m_thread - thread. Processes the currently commited threads.
             */
            void run();

            /**
             * Process the specified command. Use Command::handle to mark the command as being handled.
             *
             * \param command the command to handle
             */
            virtual void process( SPtr< Command > command ) = 0;

        private:
            /**
             * The thread of this command queue.
             */
            SPtr< std::thread > m_thread = nullptr;

            /**
             * The actual command queue.
             */
            std::list< SPtr< Command > > m_commandQueue;

            /**
             * Securing the command queue during processing. This mutex protects
             * m_commandQueue, m_running and m_gracefulStop.
             */
            std::mutex m_commandQueueMutex;

            /**
             * Used to notify the processing thread.
             */
            std::condition_variable m_commandQueueCond;

            /**
             * Denotes whether the queue thread is curring.
             */
            bool m_running = false;

            /**
             * Stop the command queue the graceful way. Finish all commands and stop.
             */
            bool m_gracefulStop = true;

            /**
             * Handles thread notification.
             */
            void notifyThread();

            /**
             * Handle a single command.
             *
             * \param command the command to handle
             */
            void processCommand( SPtr< Command > command );
        };
    }
}

#endif  // DI_COMMANDQUEUE_H


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

#include <string>

#include "CommandQueue.h"

#include <di/core/Logger.h>
#define LogTag "core/CommandQueue"

namespace di
{
    namespace core
    {
        CommandQueue::CommandQueue()
        {
        }

        CommandQueue::~CommandQueue()
        {
            stop( false );
        }

        void CommandQueue::run()
        {
            bool running = true;

            // loop and handle ...
            while( running )
            {
                // Mutex lock to pull/copy all guarded variables.
                std::unique_lock< std::mutex > lock( m_commandQueueMutex );
                bool gracefulStop;
                std::list< SPtr< Command > > pullCommands;

                // Only wait if there is nothing to do. It's a loop to
                // handle spurious wakeups as well.
                while( m_commandQueue.empty() && m_running )
                {
                    LogD << "Empty queue. Sleeping." << LogEnd;
                    m_commandQueueCond.wait( lock );
                }
                running = m_running;
                gracefulStop = m_gracefulStop;
                m_commandQueue.swap( pullCommands );


                // From here we are unlocked to process all pulled commands.
                lock.unlock();

                LogD << "Wakeup " << pullCommands.size() << " commands"
                    << (running ? "." : " and exit.") << LogEnd;

                for( auto command : pullCommands )
                {
                    // be fool-proof
                    if( !command )
                    {
                        continue;
                    }

                    // If we stop gracefully, we allow each command to be processed.
                    if( running || gracefulStop )
                    {
                        processCommand( command );
                    }
                    else // for a forced stop, we abort the remaining commands
                    {
                        command->abort();
                    }
                }
            }
        }

        void CommandQueue::processCommand( SPtr< Command > command )
        {
            // If the command was aborted ...
            if( command->isAborted() )
            {
                return;
            }

            // The command is now busy ...
            command->busy();
            try
            {
                process( command );
            }
            catch( const std::exception& e )
            {
                command->fail( e );
            }
            catch( ... )
            {
                command->fail( "Unknown exception occurred." );
            }

            // maybe someone is waiting ... notify
            command->success();
        }

        void CommandQueue::start()
        {
            std::lock_guard< std::mutex > lock( m_commandQueueMutex );

            // ignore the call if the thread is running already
            if( m_thread )
                return;

            // start the std::thread.
            m_running = true;
            m_thread = SPtr< std::thread >( new std::thread( &CommandQueue::run, this ) );
        }

        void CommandQueue::stop( bool graceful )
        {
            std::unique_lock< std::mutex > lock( m_commandQueueMutex );

            // ignore the call if there is no thread running
            if( !m_thread )
                return;

            m_running = false;
            m_gracefulStop = graceful;

            lock.unlock();

            notifyThread();
            m_thread->join();
            m_thread = nullptr;
        }

        void CommandQueue::notifyThread()
        {
            m_commandQueueCond.notify_one();
        }
    }
}

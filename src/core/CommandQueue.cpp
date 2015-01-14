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

#define LogTag "core/CommandQueue"
#include "Logger.h"

#include "CommandQueue.h"

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
            // Used to lock the command queue mutex
            std::unique_lock< std::mutex > lock( m_commandQueueMutex );

            // loop and handle ...
            while( m_running )
            {
                // was the thread notified again?
                if( !m_notified )
                {
                    // No. We wait.
                    m_commandQueueCond.wait( lock,
                                             [ this ]  // keep waiting if not explicitly notified
                                             {
                                                 return m_notified;
                                             }
                                           );
                }
                m_notified = false;

                // stop?
                if( !m_running )
                {
                    // if stopping, process the remaining commands:
                    for( auto command : m_commandQueue )
                    {
                        // be fool-proof
                        if( !command )
                        {
                            continue;
                        }

                        // If we stop gracefully, we allow each command to be processed.
                        if( m_gracefulStop )
                        {
                            processCommand( command );
                        }
                        else // for a forced stop, we abort the remaining commands
                        {
                            command->abort();
                        }
                    }
                    // Cleanup ...
                    m_commandQueue.clear();
                }
                else    // business as usual ... process command
                {
                    // get command
                    SPtr< Command > command;
                    if( !m_commandQueue.empty() )
                    {
                        command = m_commandQueue.front();
                        m_commandQueue.pop_back();
                    }
                    // be fool-proof
                    if( !command )
                    {
                        continue;
                    }

                    // process
                    processCommand( command );
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
            // ignore the call if the thread is running already
            if( m_thread )
            {
                return;
            }

            // start the std::thread.
            m_running = true;
            m_thread = SPtr< std::thread >( new std::thread( &CommandQueue::run, this ) );
        }

        void CommandQueue::stop( bool graceful )
        {
            m_running = false;
            m_gracefulStop = graceful;
            if( m_thread )
            {
                notifyThread();
                m_thread->join();
                m_thread = nullptr;
            }
        }

        void CommandQueue::notifyThread()
        {
            // Always set this variable to ensure that a busy thread continues to work if it is not waiting on m_commandQueueCond right now.
            m_notified = true;
            m_commandQueueCond.notify_one();
        }
    }
}

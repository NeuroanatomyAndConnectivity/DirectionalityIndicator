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
// along with DirectionalityIndicator. If not, see <http:#www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------------------

#include <string>

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
        }

        void CommandQueue::run()
        {
            // loop and handle ...
        }

        void CommandQueue::start()
        {
            // ignore the call if the thread is running already
            if( m_thread )
            {
                return;
            }

            // start the std::thread.
            m_thread = SPtr< std::thread >( new std::thread( &CommandQueue::run, this ) );
        }

        void CommandQueue::stop()
        {
            if( m_thread )
            {
                m_thread->join();
                m_thread = nullptr;
            }
        }
    }
}

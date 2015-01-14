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

#include "Command.h"

#include "Types.h"

namespace di
{
    namespace core
    {
        Command::Command( SPtr< CommandObserver > observer ):
            std::enable_shared_from_this< Command >(),
            m_observer( observer )
        {
        }

        Command::~Command()
        {
        }

        SPtr< CommandObserver > Command::getObserver() const
        {
            return m_observer;
        }

        void Command::busy()
        {
            // Cannot be busy anymore.
            if( isDone() )
            {
                return;
            }

            // Change state and notify
            m_isBusy = true;
            m_isWaiting = false;
            if( m_observer )
            {
                m_observer->busy( shared_from_this() );
            }
        }

        bool Command::isBusy() const
        {
            return m_isBusy;
        }

        void Command::waiting()
        {
            // Cannot be waiting anymore.
            if( m_isBusy || isDone() )
            {
                return;
            }

            // Change state and notify
            m_isWaiting = true;
            if( m_observer )
            {
                m_observer->waiting( shared_from_this() );
            }
        }

        bool Command::isWaiting() const
        {
            return m_isWaiting;
        }

        void Command::success()
        {
            // Ignore the case if it already was marked as somehow finished or if it is still waiting.
            if( isDone() || m_isWaiting )
            {
                return;
            }

            // Change state and notify
            m_isSuccessful = true;
            m_isWaiting = false;
            m_isBusy = false;
            if( m_observer )
            {
                m_observer->success( shared_from_this() );
            }
        }

        bool Command::isSuccessful() const
        {
            return m_isSuccessful;
        }

        void Command::abort()
        {
            // Ignore the case if it already was marked as somehow finished.
            if( isDone() )
            {
                return;
            }

            // Change state and notify
            m_isAborted = true;
            m_isWaiting = false;
            m_isBusy = false;
            m_isSuccessful = false;
            if( m_observer )
            {
                m_observer->abort( shared_from_this() );
            }
        }

        bool Command::isAborted() const
        {
            return m_isAborted;
        }

        void Command::fail( const std::string& reason )
        {
            // Ignore the case if it already was marked as somehow finished or if it is still waiting.
            if( isDone() || m_isWaiting )
            {
                return;
            }

            // Change state and notify
            m_isFailed = true;
            m_isAborted = true;
            m_isWaiting = false;
            m_isBusy = false;
            m_isSuccessful = false;
            m_failureReason = reason;
            if( m_observer )
            {
                m_observer->fail( shared_from_this() );
            }
        }

        void Command::fail( const std::exception& reason )
        {
            // For now, keep the exception message:
            fail( reason.what() );
        }

        bool Command::isFailed() const
        {
            return m_isFailed;
        }

        bool Command::isDone() const
        {
            return m_isSuccessful || m_isFailed || m_isAborted;
        }

        const std::string& Command::getFailureReason() const
        {
            return m_failureReason;
        }
    }
}

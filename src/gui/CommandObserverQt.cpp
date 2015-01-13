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

#define LogTag "gui/CommandObserverQt"
#include "core/Logger.h"

#include <QCoreApplication>

#include "events/CommandObserverEvent.h"

#include "CommandObserverQt.h"

namespace di
{
    namespace gui
    {
        CommandObserverQt::CommandObserverQt( QWidget* receiver, std::vector< QWidget* > affected ):
            di::core::CommandObserver(),
            std::enable_shared_from_this< CommandObserverQt >(),
            m_receiver( receiver ),
            m_affected( affected )
        {
            // nothing to do.
        }

        CommandObserverQt::CommandObserverQt( QWidget* receiver, std::function<void()> callback, std::vector< QWidget* > affected ):
            CommandObserverQt( receiver, affected )
        {
            m_hasCallback = true;
            m_callback = callback;
        }

        CommandObserverQt::~CommandObserverQt()
        {
            // nothing to do.
        }

        void CommandObserverQt::busy()
        {
            // the shared_from_this creates a shared_ptr of this instance. This ensures that the observer is valid as long as the event is in the Qt
            // queue ...
            QCoreApplication::postEvent( m_receiver, new CommandObserverEvent( shared_from_this(), CommandObserverEvent::BUSY ) );
        }

        void CommandObserverQt::waiting()
        {
            // the shared_from_this creates a shared_ptr of this instance. This ensures that the observer is valid as long as the event is in the Qt
            // queue ...
            QCoreApplication::postEvent( m_receiver, new CommandObserverEvent( shared_from_this(), CommandObserverEvent::WAITING ) );
        }

        void CommandObserverQt::success()
        {
            // the shared_from_this creates a shared_ptr of this instance. This ensures that the observer is valid as long as the event is in the Qt
            // queue ...
            QCoreApplication::postEvent( m_receiver, new CommandObserverEvent( shared_from_this(), CommandObserverEvent::SUCCESS ) );
        }

        void CommandObserverQt::abort()
        {
            // the shared_from_this creates a shared_ptr of this instance. This ensures that the observer is valid as long as the event is in the Qt
            // queue ...
            QCoreApplication::postEvent( m_receiver, new CommandObserverEvent( shared_from_this(), CommandObserverEvent::ABORT ) );
        }

        void CommandObserverQt::fail()
        {
            // the shared_from_this creates a shared_ptr of this instance. This ensures that the observer is valid as long as the event is in the Qt
            // queue ...
            QCoreApplication::postEvent( m_receiver, new CommandObserverEvent( shared_from_this(), CommandObserverEvent::FAIL ) );
        }

        void CommandObserverQt::notify()
        {
            // the shared_from_this creates a shared_ptr of this instance. This ensures that the observer is valid as long as the event is in the Qt
            // queue ...
            QCoreApplication::postEvent( m_receiver, new CommandObserverEvent( shared_from_this(), CommandObserverEvent::GENERIC ) );
        }

        QWidget* CommandObserverQt::getReceiver() const
        {
            return m_receiver;
        }

        const std::vector< QWidget* >& CommandObserverQt::getAffected() const
        {
            return m_affected;
        }

        void CommandObserverQt::callback()
        {
            if( m_hasCallback )
            {
                m_callback();
            }
        }
    }
}


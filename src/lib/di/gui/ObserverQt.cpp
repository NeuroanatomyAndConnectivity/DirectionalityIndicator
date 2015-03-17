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

#include <vector>
#include <memory>
#include <functional>

#include <QCoreApplication>

#include <di/gui/events/ObserverEvent.h>

#include "ObserverQt.h"

namespace di
{
    namespace gui
    {
        ObserverQt::ObserverQt( QWidget* receiver, std::vector< QWidget* > affected ):
            di::core::Observer(),
            std::enable_shared_from_this< ObserverQt >(),
            m_receiver( receiver ),
            m_affected( affected )
        {
            // nothing to do.
        }

        ObserverQt::ObserverQt( QWidget* receiver, std::function<void()> callback, std::vector< QWidget* > affected ):
            ObserverQt( receiver, affected )
        {
            m_hasCallback = true;
            m_callback = callback;
        }

        ObserverQt::~ObserverQt()
        {
            // nothing to do.
        }

        void ObserverQt::notify()
        {
            // the shared_from_this creates a shared_ptr of this instance. This ensures that the observer is valid as long as the event is in the Qt
            // queue ...
            QCoreApplication::postEvent( m_receiver, new ObserverEvent( shared_from_this() ) );
        }

        QWidget* ObserverQt::getReceiver() const
        {
            return m_receiver;
        }

        const std::vector< QWidget* >& ObserverQt::getAffected() const
        {
            return m_affected;
        }

        void ObserverQt::callback()
        {
            if( m_hasCallback )
            {
                m_callback();
            }
        }
    }
}


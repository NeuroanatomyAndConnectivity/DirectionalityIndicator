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

#define LogTag "gui/CommandObserverEvent"
#include "core/Logger.h"

#include "Events.h"
#include "CommandObserverEvent.h"

namespace di
{
    namespace gui
    {
        CommandObserverEvent::CommandObserverEvent( SPtr< CommandObserverQt > observer, CommandObserverStatus status ):
            QEvent( static_cast< QEvent::Type >( QT_COMMANDOBSERVER_EVENT ) ),
            m_observer( observer ),
            m_status( status )
        {
        }

        CommandObserverEvent::~CommandObserverEvent()
        {
            // do nothing
        }

        SPtr< CommandObserverQt > CommandObserverEvent::getObserver() const
        {
            return m_observer;
        }

        CommandObserverEvent::CommandObserverStatus CommandObserverEvent::getObserverStatus() const
        {
            return m_status;
        }
    }
}


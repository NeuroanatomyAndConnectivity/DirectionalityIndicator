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

#ifndef COMMANDOBSERVEREVENT_H
#define COMMANDOBSERVEREVENT_H

#include <QEvent>

#include "core/Command.h"

#include "../CommandObserverQt.h"

#include "Events.h"

#include "Types.h"

namespace di
{
    namespace gui
    {
        /**
         * A QEvent to handle \ref CommandObserverQt updates.
         */
        class CommandObserverEvent: public QEvent
        {
        public:
            /**
             * All CommandObserver events.
             */
            enum CommandObserverStatus{ BUSY, WAITING, SUCCESS, ABORT, FAIL, GENERIC };

            /**
             * Constructor. Does nothing.
             *
             * \param observer the observer that has sent the event.
             * \param issuer the command that issued this event.
             * \param status the current observer status change.
             */
            CommandObserverEvent( SPtr< CommandObserverQt > observer, SPtr< di::core::Command > issuer, CommandObserverStatus status );

            /**
             * Destructor. Does nothing.
             */
            virtual ~CommandObserverEvent();

            /**
             * Return the observer that has sent this event.
             *
             * \return the observer
             */
            SPtr< CommandObserverQt > getObserver() const;

            /**
             * The command that caused this notification.
             *
             * \tparam the type of the command. If this does not match the real type of the command, nullptr is returned.
             *
             * \return the command.
             */
            template< typename CommandType = di::core::Command >
            SPtr< CommandType > getIssuer() const
            {
                return std::dynamic_pointer_cast< CommandType >( m_issuer );
            }

            /**
             * Get the status at which the observer fired.
             *
             * \return the status
             */
            CommandObserverStatus getObserverStatus() const;
        protected:
        private:
            /**
             * The observer that has sent the event.
             */
            SPtr< CommandObserverQt > m_observer = nullptr;

            /**
             * Status
             */
            CommandObserverStatus m_status = GENERIC;

            /**
             * The command that caused this notification.
             */
            SPtr< di::core::Command > m_issuer = nullptr;
        };
    }
}

#endif  // COMMANDOBSERVEREVENT_H


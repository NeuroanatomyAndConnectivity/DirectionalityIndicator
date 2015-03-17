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

#ifndef DI_OBSERVEREVENT_H
#define DI_OBSERVEREVENT_H

#include <QEvent>

#include <di/gui/ObserverQt.h>
#include <di/gui/events/Events.h>

#include "Types.h"

namespace di
{
    namespace gui
    {
        /**
         * A QEvent to handle \ref ObserverQt updates.
         */
        class ObserverEvent: public QEvent
        {
        public:
            /**
             * Constructor. Does nothing.
             *
             * \param observer the observer that has sent the event.
             */
            explicit ObserverEvent( SPtr< ObserverQt > observer );

            /**
             * Destructor. Does nothing.
             */
            virtual ~ObserverEvent();

            /**
             * Return the observer that has sent this event.
             *
             * \return the observer
             */
            SPtr< ObserverQt > getObserver() const;
        protected:
        private:
            /**
             * The observer that has sent the event.
             */
            SPtr< ObserverQt > m_observer = nullptr;
        };
    }
}

#endif  // DI_OBSERVEREVENT_H


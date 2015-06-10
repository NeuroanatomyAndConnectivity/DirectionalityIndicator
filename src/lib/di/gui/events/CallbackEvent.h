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

#ifndef DI_CALLBACKEVENT_H
#define DI_CALLBACKEVENT_H

#include <QEvent>

#include <di/gui/events/Events.h>
#include <di/Types.h>

namespace di
{
    namespace gui
    {
        /**
         * A QEvent to defer a function call to the UI thread.
         */
        class CallbackEvent: public QEvent
        {
        public:
            /**
             * Constructor. Does nothing.
             *
             * \param func the function to call
             */
            explicit CallbackEvent( std::function< void() > func );

            /**
             * Destructor. Does nothing.
             */
            virtual ~CallbackEvent();

            /**
             * Call the function.
             */
            void call();
        protected:
        private:
            /**
             * The function call back in UI thread.
             */
            std::function< void() > m_function;
        };
    }
}

#endif  // DI_CALLBACKEVENT_H


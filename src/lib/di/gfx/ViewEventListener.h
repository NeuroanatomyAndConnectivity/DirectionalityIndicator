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

#ifndef DI_VIEWEVENTLISTENER_H
#define DI_VIEWEVENTLISTENER_H

#include <utility>
#include <vector>
#include <mutex>

#include <di/core/State.h>

#include <di/gfx/Camera.h>
#include <di/gfx/ViewEvent.h>
#include <di/GfxTypes.h>
#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
        * Class to react on events on a viewer. It gets called from inside the UI thread. Be aware that blocking operations block the UI too.
         */
        class ViewEventListener
        {
        public:
            /**
             * Constructor. Construct an empty listener that does nothing. To handle events, specify a functor
             */
            explicit ViewEventListener( std::function< void( SPtr< ViewEvent > ) > callback = []( SPtr< ViewEvent > ){} );

            /**
             * Clean up and destroy the listener.
             */
            virtual ~ViewEventListener();

            /**
             * Handle the specified event.
             *
             * \param event the event to handle
             */
            virtual void handle( SPtr< ViewEvent > event );
        protected:
        private:
            /**
             * Callback function
             */
            std::function< void( SPtr< ViewEvent > ) > m_callback;
        };
    }
}

#endif  // DI_VIEWEVENTLISTENER_H



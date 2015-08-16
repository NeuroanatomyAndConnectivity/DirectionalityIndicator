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

#ifndef DI_VIEW_H
#define DI_VIEW_H

#include <utility>
#include <vector>
#include <mutex>

#include <di/GfxTypes.h>
#include <di/Types.h>

#include <di/core/State.h>
#include <di/gfx/Camera.h>

namespace di
{
    namespace core
    {
        class ViewEventListener;
        class ViewEvent;

        /**
         * Class represents the rendering view. This includes the Viewport and the Camera.
         */
        class View
        {
        public:
            /**
             * Get the origin of the viewport.
             *
             * \return the origin
             */
            virtual glm::vec2 getViewportOrigin() const = 0;

            /**
             * Gets the size of the viewport in x and y directions. Keep in mind that the physical pixel coordinate of the upper-right corner will be
             * origin + size - (1,1). Alternatively, use \ref getViewport() to query this directly.
             *
             * \return
             */
            virtual glm::vec2 getViewportSize() const = 0;

            /**
             * Utility to get the coordinates of the viewport vertices in one call.
             *
             * \return the first pair element is the origin, the second element is \ref getViewportOrigin() + get getViewportSize() - ( 1, 1 ).
             */
            std::pair< glm::vec2, glm::vec2 > getViewport() const;

            /**
             * Calculate the aspect ratio by using the viewport information.
             *
             * \return the ratio width/height.
             */
            double getAspectRatio() const;

            /**
             * Get the camera of this view.
             *
             * \return the camera.
             */
            virtual const Camera& getCamera() const = 0;

            /**
             * Bind this view as rendering target. You should issue this if you use your own FBOs.
             */
            virtual void bind() const = 0;

            /**
             * If true, the rendering should provide high quality.
             *
             * \return true if HQ
             */
            bool isHQMode() const;

            /**
             * Activate HQ mode.
             *
             * \param hq true to activate.
             */
            void setHQMode( bool hq = true );

            /**
             * Get the state object representing this object at the moment of the call.
             *
             * \return  the state
             */
            virtual di::core::State getState() const = 0;

            /**
             * Apply the state to this instance.
             *
             * \param state the state to set
             *
             * \return  true if everything was fine.
             */
            virtual bool setState( const di::core::State& state ) = 0;

            /**
             * Register an event listener to get mouse/key/touch feedback. Do not call from inside a listener. This will cause a deadlock.
             *
             * \param listener the listener to add. If already added, nothing happens.
             */
            virtual void addEventListener( SPtr< ViewEventListener > listener );

            /**
             * Remove an event listener.
             *
             * \param listener the listener to remove. If not registered, nothing happens. Do not call from inside a listener. This will cause a
             * deadlock.
             */
            virtual void removeEventListener( SPtr< ViewEventListener > listener );

            /**
             * Get the list of listeners of this view. This makes a thread-safe copy.
             *
             * \return a copy of the event listener pointers.
             */
            virtual SPtrVec< ViewEventListener > getEventListener() const;

        protected:
            /**
             * Construct the view.
             */
            View();

            /**
             * Destruct. Cleanup.
             */
            virtual ~View();

            /**
             * The event to handle by the ViewEventHandlers.
             *
             * \param event the event.
             */
            virtual void pushEvent( SPtr< ViewEvent > event );
        private:

            /**
             * HQ Mode?
             */
            bool m_hqMode = false;

            /**
             * The actual list of event listeners.
             */
            SPtrVec< ViewEventListener > m_eventListeners;

            /**
             * Securing the event listener during processing.
             */
            mutable std::mutex m_eventListenerMutex;
        };
    }
}

#endif  // DI_VIEW_H


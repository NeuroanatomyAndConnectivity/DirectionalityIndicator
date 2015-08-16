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

#ifndef DI_VIEWEVENT_H
#define DI_VIEWEVENT_H

#include <memory>

#include <di/GfxTypes.h>

namespace di
{
    namespace core
    {
        /**
         * Class as base for all kind of viewer events.
         */
        class ViewEvent: public std::enable_shared_from_this< ViewEvent >
        {
        public:
            /**
             * Destruct event.
             */
            virtual ~ViewEvent() = default;

            /**
             * Check if this event is a certain type.
             *
             * \tparam T the type to check
             *
             * \return true if the event is of this type.
             */
            template< typename T >
            bool is() const
            {
                return ( dynamic_cast< T* >( this ) != nullptr );
            }

            /**
             * Get the event as event of a given type.
             *
             * \tparam T the type of the event to get
             *
             * \return  the event
             */
            template< typename T >
            bool as() const
            {
                return std::dynamic_pointer_cast< T >( shared_from_this() );
            }

        protected:
            /**
             * Construct event.
             */
            ViewEvent() = default;
        private:
        };

        /**
         * Namespace for all the events supported
         */
        namespace ViewEvents
        {
            /**
             * Mouse move event.
             */
            class MouseMove: public ViewEvent
            {
            public:
                /**
                 * Mouse was moved.
                 *
                 * \param x the x coord in view's pixels coordinates
                 * \param y the y coord in view's pixels coordinates
                 */
                MouseMove( int x, int y ):
                    ViewEvent(),
                    m_coord( x, y )
                {
                }

                /**
                 * Destructor.
                 */
                ~MouseMove() override = default;

                /**
                 * The x/y coordinate in the view's pixel coordinate
                 *
                 * \return coordinate
                 */
                glm::vec2 get() const;

            protected:
            private:
                /**
                 * Coordinate
                 */
                glm::vec2 m_coord;
            };
        }
    }
}

#endif  // DI_VIEWEVENT_H


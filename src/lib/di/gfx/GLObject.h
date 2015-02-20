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

#ifndef DI_GLOBJECT_H
#define DI_GLOBJECT_H

#include <di/gfx/OpenGL.h>

namespace di
{
    namespace core
    {
        /**
         * Basic class to represent an arbitrary OpenGL object. All of these objects can be realized and removed..
         */
        class GLObject
        {
        public:
            /**
             * Realize the object.
             *
             * \return true if successful.
             */
            virtual bool realize() = 0;

            /**
             * Check if the object was realized.
             *
             * \return true if realized
             */
            bool isRealized() const
            {
                return ( m_object != 0 );
            }

            /**
             * Return the object ID. This is an OpenGL ID.
             *
             * \return the id.
             */
            GLuint getObjectID() const
            {
                return m_object;
            }
        protected:
            /**
             * Constructor.
             */
            GLObject()
            {
            };

            /**
             * Constructor.
             */
            virtual ~GLObject()
            {
            };

            /**
             * The ID used by OpenGL.
             */
            GLuint m_object = 0;
        private:
        };
    }
}

#endif  // DI_GLOBJECT_H



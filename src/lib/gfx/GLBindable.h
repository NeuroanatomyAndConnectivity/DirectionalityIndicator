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

#ifndef GLBINDABLE_H
#define GLBINDABLE_H

#include "OpenGL.h"
#include "GLObject.h"

namespace di
{
    namespace core
    {
        /**
         * Basic class to represent an bindable OpenGL object.
         */
        class GLBindable: public GLObject
        {
        public:
            /**
             * Bind the object.
             */
            virtual void bind() = 0;
        protected:
            /**
             * Constructor.
             */
            GLBindable():
                GLObject()
            {
            };

            /**
             * Destructor.
             */
            virtual ~GLBindable()
            {
            };

        private:
        };
    }
}

#endif  // GLBINDABLE_H


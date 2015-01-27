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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <initializer_list>
#include <string>
#include <vector>
#include <map>

#include "OpenGL.h"
#include "GLBindable.h"

#include "Types.h"
#include "GfxTypes.h"

namespace di
{
    namespace core
    {
        class Shader;

        /**
         * This class collects shaders and links the final program.
         */
        class Program: public GLBindable
        {
        public:
            /**
             * Create program, consisting of the given shaders.
             *
             * \param shaders the shaders to attach.
             */
            explicit Program( const std::vector< SPtr< Shader > >& shaders );

            /**
             * Create program, consisting of the given shaders.
             *
             * \param shaders the shaders to attach.
             */
            explicit Program( std::initializer_list< SPtr< Shader > > shaders );

            /**
             * Destructor.
             */
            virtual ~Program();

            /**
             * Create and link the program. Realizes the shaders if not yet done.
             *
             * \return true if successful.
             */
            bool realize() override;

            /**
             * Bind the program.
             */
            void bind() override;

            /**
             * Get the location of the given attribute. This basically forwards to glGetAttribLocation.
             *
             * \param name the attribute name
             *
             * \return the location
             */
            GLint getAttribLocation( const std::string& name ) const;

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             *
             * \return true if successful
             */
            bool setUniform( const std::string name, const glm::mat4& value );

        protected:
        private:
            /**
             * The shaders to attach.
             */
            std::vector< SPtr< Shader > > m_shaders;

            /**
             * Keep track of queried uniform locations to avoid this every frame.
             */
            std::map< std::string, GLint > m_uniformLocationCache;
        };
    }
}

#endif  // PROGRAM_H


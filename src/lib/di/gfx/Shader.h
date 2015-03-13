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

#ifndef DI_SHADER_H
#define DI_SHADER_H

#include <string>

#include <di/gfx/OpenGL.h>
#include <di/gfx/GLObject.h>

namespace di
{
    namespace core
    {
        /**
         * Basic class to represent a single shader. Multiple shaders build a program.
         */
        class Shader: public GLObject
        {
        public:
            /**
             * The supported shader types. This usually maps the OpenGL shaders. The only exception is the "Library" shader. This only contains code
             * that can be re-used by other shaders. OpenGL dies not yet support this kind of shaders. The Engine will manage this for you.
             */
            enum class ShaderType
            {
                Vertex,     // Vertex shaders. Corresponds to GL_VERTEX_SHADER.
                Fragment,   // Fragment shaders. Corresponds to GL_FRAGMENT_SHADER.
                Geometry,   // Geometry shaders. Corresponds to GL_GEOMETRY_SHADER.
                TesselationEvaluation, // Tesselation shader - Evaluate. // Corresponds to GL_TESS_EVALUATION_SHADER.
                TesselationControl,    // Corresponds to GL_TESS_CONTROL_SHADER.
                Compute     // Compute shaders. Corresponds to GL_COMPUTE_SHADER.
            };

            /**
             * Create shader of a given type with a given code.
             *
             * \param shaderType
             * \param code
             */
            Shader( ShaderType shaderType, const std::string& code );

            /**
             * Destructor.
             */
            virtual ~Shader();

            /**
             * Create and compile the shader.
             *
             * \return true if successful.
             */
            bool realize() override;

            /**
             * Clean up. Delete Object.
             */
            void finalize() override;

        protected:
        private:
            /**
             * The type of this shader.
             */
            ShaderType m_shaderType;

            /**
             * The code of the shader.
             */
            std::string m_code;
        };
    }
}

#endif  // DI_SHADER_H


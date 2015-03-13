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

#include <string>
#include <vector>

#include "Shader.h"

#include <di/core/Logger.h>
#define LogTag "gfx/Shader"
#include <di/gfx/GLError.h>

namespace di
{
    namespace core
    {
        Shader::Shader( Shader::ShaderType shaderType, const std::string& code ):
            GLObject(),
            m_shaderType( shaderType ),
            m_code( code )
        {
            // Init
        }

        Shader::~Shader()
        {
            // Clean up.
            finalize();
        }

        bool Shader::realize()
        {
            if( isRealized() )
            {
                return true;
            }

            GLenum shaderType = GL_VERTEX_SHADER;
            switch( m_shaderType )
            {
                case ShaderType::Vertex:
                    shaderType = GL_VERTEX_SHADER;
                    break;
                case ShaderType::Fragment:
                    shaderType = GL_FRAGMENT_SHADER;
                    break;
                case ShaderType::Geometry:
                    shaderType = GL_GEOMETRY_SHADER;
                    break;
                case ShaderType::TesselationEvaluation:
                    shaderType = GL_TESS_EVALUATION_SHADER;
                    break;
                case ShaderType::TesselationControl:
                    shaderType = GL_TESS_CONTROL_SHADER;
                    break;
                case ShaderType::Compute:
                    shaderType = GL_COMPUTE_SHADER;
                    break;
            }

            m_object = glCreateShader( shaderType );
            logGLError();

            const char* codeStr = m_code.c_str();
            glShaderSource( m_object, 1, &codeStr, NULL );
            logGLError();
            glCompileShader( m_object );
            logGLError();

            GLint isCompiled = 0;
            glGetShaderiv( m_object, GL_COMPILE_STATUS, &isCompiled );
            if( isCompiled == GL_FALSE )
            {
                GLint maxLength = 0;
                glGetShaderiv( m_object, GL_INFO_LOG_LENGTH, &maxLength );

                // The maxLength includes the NULL character
                std::vector< GLchar > errorLog( maxLength );
                glGetShaderInfoLog( m_object, maxLength, &maxLength, &errorLog[0] );

                LogE << "Fragment shader compilation failed. Log: " << LogEnd
                LogE << errorLog.data() << LogEnd;

                // Provide the infolog in whatever manor you deem best.
                // Exit with failure.
                glDeleteShader( m_object ); // Don't leak the shader.

                m_object = 0;

                return false;
            }

            return true;
        }

        void Shader::finalize()
        {
            if( isRealized() )
            {
                glDeleteShader( m_object );
                m_object = 0;
            }
        }
    }
}


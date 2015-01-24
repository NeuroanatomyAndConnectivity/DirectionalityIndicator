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

#include <vector>

#define LogTag "gfx/Program"
#include "core/Logger.h"

#include "Program.h"
#include "Shader.h"
#include "GLError.h"

namespace di
{
    namespace core
    {
        Program::Program( std::initializer_list< SPtr< Shader > > shaders ):
            GLBindable(),
            m_shaders( shaders )
        {
        }

        Program::Program( const std::vector< SPtr< Shader > >& shaders ):
            GLBindable(),
            m_shaders( shaders )
        {
        }

        Program::~Program()
        {
            // Clean up.
        }

        bool Program::realize()
        {
            if( isRealized() )
            {
                return true;
            }

            m_object = glCreateProgram();
            logGLError();

            // Realize all shaders and attach.
            for( auto shader : m_shaders )
            {
                if( !shader->realize() )
                {
                    glDeleteProgram( m_object );
                    m_object = 0;
                    return false;
                }
                glAttachShader( m_object, shader->getObjectID() );
                logGLError();
            }
            glLinkProgram( m_object );
            logGLError();

            GLint success = GL_FALSE;
            glGetProgramiv( m_object, GL_LINK_STATUS, &success );
            if( success == GL_FALSE )
            {
                GLint maxLength = 0;
                glGetProgramiv( m_object, GL_INFO_LOG_LENGTH, &maxLength );

                // The maxLength includes the NULL character
                std::vector< GLchar > errorLog( maxLength );
                glGetProgramInfoLog( m_object, maxLength, &maxLength, &errorLog[0] );

                LogE << "Program linker failed. Log: " << LogEnd
                LogE << errorLog.data() << LogEnd;

                glDeleteProgram( m_object );
                m_object = 0;
                return false;
            }

            return true;
        }

        void Program::bind()
        {
            if( !isRealized() )
            {
                LogE << "Bind: un-realized GLObject." << LogEnd;
                return;
            }

            glUseProgram( m_object );
            logGLError();
        }

        GLint Program::getAttribLocation( const std::string& name ) const
        {
            if( !isRealized() )
            {
                LogE << "Attrib Location: un-realized GLObject." << LogEnd;
                return 0;
            }

            GLint result = glGetAttribLocation( m_object, name.c_str() );
            logGLError();
            return result;
        }
    }
}


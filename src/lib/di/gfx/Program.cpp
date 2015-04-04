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

#include <di/gfx/Shader.h>

#include "Program.h"

#include <di/core/Logger.h>
#define LogTag "gfx/Program"
#include <di/gfx/GLError.h>

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
            finalize();
        }

        bool Program::isAttached( SPtr< Shader > shader )
        {
            GLsizei count = 0;
            GLuint shaderIds[32];
            glGetAttachedShaders( m_object, 32, &count, &shaderIds[0] );

            bool found = false;
            for( int i = 0; i < count; ++i )
            {
                found = found || ( shader->getObjectID() == shaderIds[ i ] );
            }
            return found;
        }

        bool Program::compileAndLink()
        {
            if( !isRealized() )
            {
                return false;
            }

            m_needCompile = false;

            // Add all defines to the string:
            std::string prefixCode = "";
            for( auto define : m_defines )
            {
                if( !define.second.first )
                {
                    prefixCode += "#define " + define.first + " " + define.second.second + "\n";
                }
                else // only define name
                {
                    prefixCode += "#define " + define.first + "\n";
                }
            }

            // Realize all shaders and attach.
            for( auto shader : m_shaders )
            {
                shader->setPrefixCode( prefixCode );
                if( !shader->realize() )
                {
                    LogE << "Shader compilation failed." << LogEnd;
                    m_needCompile = true;
                    return false;
                }

                // Only attach if not yet done
                if( !isAttached( shader ) )
                {
                    glAttachShader( m_object, shader->getObjectID() );
                    logGLError();
                }
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

                m_object = 0;
                return false;
            }

            return true;
        }

        bool Program::realize()
        {
            if( isRealized() )
            {
                return true;
            }

            m_object = glCreateProgram();
            logGLError();

            return compileAndLink();
        }

        void Program::finalize()
        {
            if( isRealized() )
            {
                glDeleteProgram( m_object );
                m_object = 0;
            }
        }

        void Program::bind()
        {
            if( !realize() )
            {
                LogE << "Bind: un-realized GLObject." << LogEnd;
                return;
            }

            if( m_needCompile )
            {
                if( !compileAndLink() )
                {
                    LogE << "Bind: program could not be build." << LogEnd;
                    return;
                }
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

        GLint Program::getUniformLocation( const std::string& name ) const
        {
            if( !isRealized() )
            {
                LogE << "Uniform Location: un-realized GLObject." << LogEnd;
                return 0;
            }

            GLint loc = -1;

            // Check the cache if there is a location stored already
            if( !m_uniformLocationCache.count( name ) )
            {
                // Not yet existing. Add
                loc = glGetUniformLocation( m_object, name.c_str() );
                logGLError();
                if( loc < 0 )
                {
                    LogE << "Could not locate uniform \"" << name << "\". Ensure the spelling is correct and you need to use the uniform in the "
                        << "shader code." << LogEnd;
                    return -1;
                }

                // store.
                m_uniformLocationCache[ name ] = loc;
            }
            else
            {
                loc = m_uniformLocationCache[ name ];
            }

            return loc;
        }

        void Program::unsetDefine( const std::string& name )
        {
            // remove and recompile if something was removed
            if( m_defines.erase( name ) )
            {
                m_needCompile = true;
            }
        }

        void Program::setDefine( const std::string& name, bool value )
        {
            if( !value )
            {
                unsetDefine( name );
                return;
            }

            setDefine( name, true, "" );
        }

        void Program::setDefine( const std::string& name, bool defineOnly, std::string value )
        {
            // Only apply if different.
            if( ( m_defines[ name ].first != defineOnly ) || ( m_defines[ name ].second != value ) )
            {
                m_defines[ name ] = std::make_pair( defineOnly, value );
                m_needCompile = true;
                m_uniformLocationCache.clear();
            }
        }
    }
}


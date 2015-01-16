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

#include <GL/glew.h>
#include <GL/gl.h>

#define LogTag "algorithms/DirectionalityVisualization"
#include "core/Logger.h"

#include "core/data/TriangleDataSet.h"

#include "GfxTypes.h"

#include "DirectionalityVisualization.h"

namespace di
{
    namespace algorithms
    {
        DirectionalityVisualization::DirectionalityVisualization():
            Algorithm( "Directionality Visualization",
                       "This algorithm takes a triangle mesh and scalar information defined on the mesh. "
                       "It the creates directional information from the scalar data and displays it on "
                       "the mesh itself visually." ),
            Visualization()
        {
            // We require some inputs.

            // 1: the triangle mesh
            m_triangleDataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle mesh on which the directionality information should be shown."
            );
        }

        DirectionalityVisualization::~DirectionalityVisualization()
        {
            // nothing to clean up so far
        }

        void DirectionalityVisualization::process()
        {
            // Get input data
            auto data = m_triangleDataInput->getData();
            LogD << "Got data instance " << static_cast< const void* >( data.get() ) << "." << LogEnd;

            // Provide the needed information to the visualizer itself.
            bool changeVis = ( m_visTriangleData != data );
            m_visTriangleData = data;

            // As the rendering system does not render permanently, inform about the update.
            if( changeVis )
            {
                renderRequest();
            }
        }

        void DirectionalityVisualization::prepare()
        {
            LogD << "Vis Prepare" << LogEnd;

            // Create and bind the shader for it.
            const char* vertexShaderCode =
            "#version 330\n"
            ""
            "layout(location = 0) in vec3 position;"
            "layout(location = 1) in vec4 color;"
            "out vec4 v_color;"
            "void main () "
            "{"
            "  v_color = color;"
            "  gl_Position = vec4 (position * 0.005, 1.0);"
            "}";

            const char* fragmentShaderCode =
            "#version 330\n"
            ""
            "in vec4 v_color;"
            "out vec4 fragColor;"
            ""
            "void main () "
            "{"
            "    fragColor = vec4( v_color.rgb, 1.0 );"
            "}";

            // Create the vertex shader and compile
            GLuint vs = glCreateShader( GL_VERTEX_SHADER );
            glShaderSource( vs, 1, &vertexShaderCode, NULL );
            glCompileShader( vs );

            GLint isCompiled = 0;
            glGetShaderiv( vs, GL_COMPILE_STATUS, &isCompiled );
            if( isCompiled == GL_FALSE )
            {
                GLint maxLength = 0;
                glGetShaderiv( vs, GL_INFO_LOG_LENGTH, &maxLength );

                // The maxLength includes the NULL character
                std::vector< GLchar > errorLog( maxLength );
                glGetShaderInfoLog( vs, maxLength, &maxLength, &errorLog[0] );

                LogE << "Vertex shader compilation failed. Log: " << LogEnd
                LogE << errorLog.data() << LogEnd;

                // Provide the infolog in whatever manor you deem best.
                // Exit with failure.
                glDeleteShader( vs ); // Don't leak the shader.

                return;
            }

            // Same for the vertex shader
            GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
            glShaderSource( fs, 1, &fragmentShaderCode, NULL );
            glCompileShader( fs );

            isCompiled = 0;
            glGetShaderiv( fs, GL_COMPILE_STATUS, &isCompiled );
            if( isCompiled == GL_FALSE )
            {
                GLint maxLength = 0;
                glGetShaderiv( fs, GL_INFO_LOG_LENGTH, &maxLength );

                // The maxLength includes the NULL character
                std::vector< GLchar > errorLog( maxLength );
                glGetShaderInfoLog( fs, maxLength, &maxLength, &errorLog[0] );

                LogE << "Fragment shader compilation failed. Log: " << LogEnd
                LogE << errorLog.data() << LogEnd;

                // Provide the infolog in whatever manor you deem best.
                // Exit with failure.
                glDeleteShader( fs ); // Don't leak the shader.

                return;
            }

            // Link them to build the program itself
            m_shaderProgram = glCreateProgram();
            glAttachShader( m_shaderProgram, fs );
            glAttachShader( m_shaderProgram, vs );
            glLinkProgram( m_shaderProgram );
        }

        void DirectionalityVisualization::finalize()
        {
            LogD << "Vis Finalize" << LogEnd;
        }

        void DirectionalityVisualization::render()
        {
            if( !( m_VAO && m_triVBO && m_triIBO && m_shaderProgram ) )
            {
                return;
            }
            // LogD << "Vis Render" << LogEnd;

            glUseProgram( m_shaderProgram );
            glBindVertexArray( m_VAO );
            glDrawElements( GL_TRIANGLES, m_visTriangleData->getGrid()->getTriangles().size() * 3, GL_UNSIGNED_INT, NULL );
        }

        void DirectionalityVisualization::update()
        {
            if( !m_visTriangleData )
            {
                return;
            }

            if( !isRenderingRequested() )
            {
                return;
            }
            LogD << "Vis Update" << LogEnd;
            resetRenderingRequest();

            // get the location of attribute "position" in program
            GLint vertexLoc = glGetAttribLocation( m_shaderProgram, "position" );
            GLint colorLoc = glGetAttribLocation( m_shaderProgram, "color" );

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );

            // Create the triangle vertex buffer
            glGenBuffers( 1, &m_triVBO );

            // bind buffer for positions and copy data into buffer
            // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
            glBindBuffer( GL_ARRAY_BUFFER, m_triVBO );

            // feed the buffer, and let OpenGL know that we don't plan to
            // change it (STATIC) and that it will be used for drawing (DRAW)
            glBufferData( GL_ARRAY_BUFFER, sizeof( glm::vec3::value_type ) * 3 * m_visTriangleData->getGrid()->getVertices().size(),
                                           m_visTriangleData->getGrid()->getVertices().data(),
                                           GL_STATIC_DRAW );

            // Enable the attribute at that location
            glEnableVertexAttribArray( vertexLoc );

            // Tell OpenGL what the array contains:
            // it is a set of 3 floats for each vertex
            glVertexAttribPointer( vertexLoc, 3, GL_FLOAT, 0, 0, 0 );

            // Create the triangle color buffer
            glGenBuffers( 1, &m_colorVBO );

            // bind buffer for positions and copy data into buffer
            // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
            glBindBuffer( GL_ARRAY_BUFFER, m_colorVBO );

            // feed the buffer, and let OpenGL know that we don't plan to
            // change it (STATIC) and that it will be used for drawing (DRAW)
            glBufferData( GL_ARRAY_BUFFER, sizeof( RGBAArray::value_type::value_type ) * 4 * m_visTriangleData->getAttributes()->size(),
                                           m_visTriangleData->getAttributes()->data(),
                                           GL_STATIC_DRAW );

            // Enable the attribute at that location
            glEnableVertexAttribArray( colorLoc );

            // Tell OpenGL what the array contains:
            // it is a set of 3 floats for each vertex
            glVertexAttribPointer( colorLoc, 4, GL_FLOAT, 0, 0, 0 );

            glGenBuffers( 1, &m_triIBO );

            // bind buffer for positions and copy data into buffer
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_triIBO );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( glm::ivec3::value_type ) * 3 * m_visTriangleData->getGrid()->getTriangles().size(),
                                                   m_visTriangleData->getGrid()->getTriangles().data(),
                                                   GL_STATIC_DRAW );
        }
    }
}


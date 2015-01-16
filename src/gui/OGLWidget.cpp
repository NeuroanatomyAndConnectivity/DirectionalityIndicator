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

#include <algorithm>

#include <GL/glew.h>
#include <GL/gl.h>

#define LogTag "gui/OGLWidget"
#include "core/Logger.h"

#include "OGLWidget.h"

namespace di
{
    namespace gui
    {
        OGLWidget::OGLWidget( QWidget* parent ):
            QGLWidget( getDefaultFormat(), parent )
        {
            setAutoBufferSwap( true );
        }

        OGLWidget::~OGLWidget()
        {
        }

        QGLFormat OGLWidget::getDefaultFormat()
        {
            // Specify an OpenGL 3.3 format using the Core profile.
            QGLFormat glFormat;
            glFormat.setVersion( 3, 3 );
            glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0

            return glFormat;
        }

        void OGLWidget::initializeGL()
        {
            // Get some version info here.
            const GLubyte* renderer = glGetString( GL_RENDERER );
            const GLubyte* version = glGetString( GL_VERSION );

            // Enable "experimental" for 3.3 functionality
            glewExperimental = true;
            GLenum err = glewInit();
            if( GLEW_OK != err )
            {
                LogE << "Error during GLEW initialization: " << glewGetErrorString( err ) << ". Expect problems." << LogEnd;
                exit( 1 );
            }

            LogI << "GL Renderer: " << renderer << LogEnd;
            LogI << "GL Version: " << version << LogEnd;
            LogI << "GLEW Version: " << glewGetString( GLEW_VERSION ) << LogEnd;

            // Black background
            glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

            // We want depth testing
            glEnable( GL_DEPTH_TEST );
            // The commonly used default is to interpret smaller values as being closer:
            glDepthFunc( GL_LESS );

            // Create the full-screen quad for background rendering. Note: the camera is configured to project [-1,1] at each axis to the viewport.
            float points[] = {
             -1.0f,  1.0f,  0.0f,
              1.0f,  1.0f,  0.0f,
              1.0f, -1.0f,  0.0f,

              1.0f, -1.0f,  0.0f,
             -1.0f, -1.0f,  0.0f,
             -1.0f,  1.0f,  0.0f
            };

            // Create Vertex Array Object
            glGenVertexArrays( 1, &m_backgroundVAO );
            glBindVertexArray( m_backgroundVAO );

            // Create and fill the VBO for the BG
            glGenBuffers( 1, &m_backgroundVBO );
            glBindBuffer( GL_ARRAY_BUFFER, m_backgroundVBO );
            // Set the data
            glBufferData( GL_ARRAY_BUFFER, 2 * 9 * sizeof( float ), points, GL_STATIC_DRAW );

            // Create and bind the shader for it.
            const char* vertexShaderCode =
            "#version 330\n"
            ""
            "layout(location = 0) in vec3 vp;"
            "out float vert;"
            "void main () "
            "{"
            "  vert = ( vp.y + 1 ) / 2.0;"
            "  gl_Position = vec4 (vp, 1.0);"
            "}";

            const char* fragmentShaderCode =
            "#version 330\n"
            "in float vert;"
            ""
            "vec4 u_bottom1Color = vec4( 1.0 );"
            "vec4 u_bottom2Color = vec4( vec3( 0.82 ), 1.0 );"
            ""
            "vec4 u_top1Color = vec4( vec3( 0.85 ), 1.0 );"
            "vec4 u_top2Color = vec4( vec3( 0.95 ), 1.0 );"
            ""
            "float u_horizon = 33;"
            ""
            "out vec4 fragColor;"
            ""
            "void main () "
            "{"
            "    vec4 color = vec4( 1.0 );"
            "    float horizonLine = u_horizon / 100.0;"
            "    float bottom = float( vert <= horizonLine );"
            "    float horizon = horizonLine + 0.0001;"
            "    float topScale = ( vert - horizon ) / ( 1.0 - horizon );"
            "    color = bottom           * mix( u_bottom1Color, u_bottom2Color, vert / horizon ) +"
            "           ( 1.0 - bottom ) * mix( u_top1Color, u_top2Color, topScale );"
            "    fragColor = vec4( color.rgb, 1.0 );"
            "}";

            // Create the vertex shader and compile
            GLuint vs = glCreateShader( GL_VERTEX_SHADER );
            glShaderSource( vs, 1, &vertexShaderCode, NULL );
            glCompileShader( vs );

            // Same for the vertex shader
            GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
            glShaderSource( fs, 1, &fragmentShaderCode, NULL );
            glCompileShader( fs );

            // Link them to build the program itself
            m_backgroundShaderProgram = glCreateProgram();
            glAttachShader( m_backgroundShaderProgram, fs );
            glAttachShader( m_backgroundShaderProgram, vs );
            glLinkProgram( m_backgroundShaderProgram );
        }

        void OGLWidget::resizeGL( int w, int h )
        {
            // Use a basic viewport setup. Equal to window size.
            // NOTE: avoid issues with zero-sized widgets. This sometimes happen during creation.
            glViewport( 0, 0, std::max( 1, w ), std::max( 1, h ) );
        }

        void OGLWidget::paintGL()
        {
            // Cleanup buffers
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            // Draw background first:
            glUseProgram( m_backgroundShaderProgram );

            glBindVertexArray( m_backgroundVAO );
            glEnableVertexAttribArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, m_backgroundVBO );

            glVertexAttribPointer(
                0, // Attribute number.
                3, // It is a vec3
                GL_FLOAT, // floats
                GL_FALSE, // not normalized
                0,        // no stride
                nullptr   // no buffer offset
            );

            // Draw the BG
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle

            glDisableVertexAttribArray( 0 );

            // Draw scenes
        }

        void OGLWidget::closeEvent( QCloseEvent* event )
        {
            // Clean up properly
            glDeleteBuffers( 1, &m_backgroundVBO );
            // glDeleteVertexArrays( 1, &m_backgroundVAO );
            glDeleteProgram( m_backgroundShaderProgram );

            QGLWidget::closeEvent( event );
        }
    }
}


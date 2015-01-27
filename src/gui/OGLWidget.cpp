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

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <algorithm>
#include <string>
#include <cmath>

#include <QMouseEvent>

#define LogTag "gui/OGLWidget"
#include "core/Logger.h"
#include "core/Filesystem.h"
#include "core/BoundingBox.h"
#include "gfx/GL.h"

#include "Application.h"

#include "OGLWidget.h"

namespace di
{
    namespace gui
    {
        OGLWidget::OGLWidget( QWidget* parent ):
            QGLWidget( getDefaultFormat(), parent )
        {
            setAutoBufferSwap( true );
            setFocusPolicy( Qt::StrongFocus );

            m_redrawTimer = new QTimer();
            m_redrawTimer->setInterval( 33 );
            QObject::connect( m_redrawTimer, SIGNAL( timeout() ), this, SLOT( update() ), Qt::QueuedConnection );
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

            logGLError();

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

            std::string localShaderPath = core::getRuntimePath() + "/gfx/shaders/";
            m_bgVertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "CameraEffectHorizon-vertex.glsl" ) );
            m_bgFragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "CameraEffectHorizon-fragment.glsl" ) );

            // Link them to build the program itself
            // m_bgShaderProgram = std::make_shared< di::core::Program >( { m_bgVertexShader, m_bgFragmentShader } );
            // NOTE: the above code does not compile on CLang.
            m_bgShaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            m_bgVertexShader,
                            m_bgFragmentShader
                        }
            ) );
            m_bgShaderProgram->realize();

            // Allow all visualizations to prepare:
            Application::getProcessingNetwork()->visitVisualizations(
                []( SPtr< di::core::Visualization > vis )
                {
                    vis->prepare();
                }
            );

            m_redrawTimer->start();
        }

        void OGLWidget::resizeGL( int w, int h )
        {
            // Use a basic viewport setup. Equal to window size.
            // NOTE: avoid issues with zero-sized widgets. This sometimes happen during creation.
            glViewport( 0, 0, std::max( 1, w ), std::max( 1, h ) );
        }

        void OGLWidget::paintGL()
        {
            // Allow all visualizations to update:
            core::BoundingBox sceneBB;
            Application::getProcessingNetwork()->visitVisualizations(
                [ &sceneBB ]( SPtr< di::core::Visualization > vis )
                {
                    vis->update();
                    sceneBB.include( vis->getBoundingBox() );
                }
            );

            // Cleanup buffers
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            // Draw background first:
            m_bgShaderProgram->bind();

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

            // Calculate scene
            double maxExtend = sqrt( 3.0 ) *
                               std::max( sceneBB.getSize().x,
                                         std::max( sceneBB.getSize().y,
                                                   sceneBB.getSize().z ) );
            double near = 0.3; // the near plane
            double far = m_zoom * sqrt( 3.0 ) + near;    // the diagonal of the cube needs to fit (sqrt(3))

            // Move scene to rotation point
            glm::mat4 rotationPointTranslate = glm::translate( -sceneBB.getCenter() );
            // Scale scene down to match screen size. The scene is now inside a unit sized cube
            glm::mat4 scaleToFit = glm::scale( glm::vec3( 1.0 / ( 1.0 * maxExtend ) ) );
            // Zoom
            glm::mat4 zoom = glm::scale( glm::vec3( m_zoom ) );

            // move scene into the visible area
            glm::mat4 moveToVisibleArea = glm::translate( glm::vec3( 0.0, 0.0, -( m_zoom * 0.5 + near ) ) );

            m_camera.setViewMatrix( moveToVisibleArea * zoom * scaleToFit * m_arcballMatrix * rotationPointTranslate );

            // set a nice default projection matrix:
            m_camera.setProjectionMatrix(
                glm::ortho( 0.5 * -getAspectRatio(), 0.5 * getAspectRatio(), -0.5, 0.5, near, far )
            );

            // Draw scenes
            // Allow all visualizations to render:
            Application::getProcessingNetwork()->visitVisualizations(
                [ this ]( SPtr< di::core::Visualization > vis )
                {
                    vis->render( *this );
                }
            );
        }

        void OGLWidget::closeEvent( QCloseEvent* event )
        {
            m_redrawTimer->stop();

            // Allow all visualizations to finalize:
            Application::getProcessingNetwork()->visitVisualizations(
                []( SPtr< di::core::Visualization > vis )
                {
                    vis->finalize();
                }
            );

            // Clean up properly
            glDeleteBuffers( 1, &m_backgroundVBO );
            // glDeleteVertexArrays( 1, &m_backgroundVAO );
            // glDeleteProgram( m_backgroundShaderProgram );

            QGLWidget::closeEvent( event );
        }

        void OGLWidget::mousePressEvent( QMouseEvent* event )
        {
            if( event->button() == Qt::LeftButton )
            {
                m_arcballState = 1;
                m_arcballPrevMatrix = m_arcballMatrix;
            }
            event->accept();
        }

        void OGLWidget::mouseReleaseEvent( QMouseEvent* event )
        {
            if( event->button() == Qt::LeftButton )
            {
                m_arcballState = 0;
            }
            event->accept();
        }

        void OGLWidget::mouseMoveEvent( QMouseEvent* event )
        {
            if( m_arcballState == 0 )
            {
                event->ignore();
                return;
            }
            else if( m_arcballState == 1 )
            {
                // Store initial position only.
                m_arcballPrevPos = toScreenCoord( event->x(), event->y() );
                m_arcballState = 2;
                event->ignore();
                return;
            }

            // Tracking the subsequent
            m_arcballCurrPos = toScreenCoord( event->x(), event->y() );
            // Calculate the angle in radians, and clamp it between 0 and 90 degrees
            m_arcballAngle = acos( std::min( 1.0f, glm::dot( m_arcballPrevPos, m_arcballCurrPos ) ) );
            // Cross product to get the rotation axis, but it's still in camera coordinate
            m_arcballCamAxis = glm::cross( m_arcballPrevPos, m_arcballCurrPos );

            m_arcballMatrix = glm::rotate( glm::degrees( m_arcballAngle ) * m_arcballRollSpeed, -m_arcballCamAxis ) *
                              m_arcballPrevMatrix;

            event->accept();
        }

        void OGLWidget::wheelEvent( QWheelEvent* event )
        {
            const double stepSize = 0.075;
            if( event->delta() < 0 )
            {
                m_zoom -= stepSize;
            }
            else
            {
                m_zoom += stepSize;
            }

            // avoid values below 0.
            m_zoom = m_zoom < stepSize ? stepSize : m_zoom;
            event->accept();
        }

        void OGLWidget::keyReleaseEvent( QKeyEvent* event )
        {
            switch( event->key() )
            {
                case Qt::Key_Space:
                    m_arcballMatrix = glm::mat4();
                    break;
                case Qt::Key_X: // rotate along x
                    if( event->modifiers() == Qt::ShiftModifier )
                    {
                        m_arcballMatrix = glm::rotate( glm::radians( 90.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) ) * m_arcballMatrix;
                    }
                    else
                    {
                        m_arcballMatrix = glm::rotate( glm::radians( -90.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) ) * m_arcballMatrix;
                    }
                    break;
                case Qt::Key_Y: // rotate along y
                    if( event->modifiers() == Qt::ShiftModifier )
                    {
                        m_arcballMatrix = glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) * m_arcballMatrix;
                    }
                    else
                    {
                        m_arcballMatrix = glm::rotate( glm::radians( -90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) * m_arcballMatrix;
                    }
                    break;
                case Qt::Key_Z: // rotate along z
                    if( event->modifiers() == Qt::ShiftModifier )
                    {
                        m_arcballMatrix = glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) * m_arcballMatrix;
                    }
                    else
                    {
                        m_arcballMatrix = glm::rotate( glm::radians( -90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) * m_arcballMatrix;
                    }
                    break;
                case Qt::Key_D:
                    // restore default for our current data. Just a convenient shortcut for now. Later this will not be needed as project
                    // files store the cam too.
                    m_arcballMatrix = glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) )
                                      * glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) )
                                      ;
                    break;

            }

            event->accept();
        }

        glm::vec3 OGLWidget::toScreenCoord( double x, double y )
        {
            glm::vec3 coord( 0.0f );
            if( m_arcballXAxis )
            {
                coord.x = ( 2 * x - width() ) / width();
            }
            if( m_arcballYAxis )
            {
                coord.y = -( 2 * y - height() ) / height();
            }

            /* Clamp it to border of the windows, comment these codes to allow rotation when cursor is not over window */
            coord.x = glm::clamp( coord.x, -1.0f, 1.0f );
            coord.y = glm::clamp( coord.y, -1.0f, 1.0f );
            float length_squared = coord.x * coord.x + coord.y * coord.y;
            if( length_squared <= 1.0 )
            {
                coord.z = sqrt( 1.0 - length_squared );
            }
            else
            {
                coord = glm::normalize( coord );
            }
            return coord;
        }

        glm::vec2 OGLWidget::getViewportOrigin() const
        {
            return glm::vec2( 0, 0 );
        }

        glm::vec2 OGLWidget::getViewportSize() const
        {
            return glm::vec2( width(), height() );
        }

        const core::Camera& OGLWidget::getCamera() const
        {
            return m_camera;
        }
    }
}


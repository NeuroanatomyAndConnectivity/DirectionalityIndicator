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
#include <string>
#include <cmath>
#include <chrono>

#include <QMouseEvent>

#include <di/core/Filesystem.h>
#include <di/core/BoundingBox.h>
#include <di/gfx/GL.h>
#include <di/gfx/OffscreenView.h>
#include <di/MathTypes.h>

#include <di/gui/Application.h>
#include <di/gui/ScreenShotWidget.h>

#include "OGLWidget.h"

#include <di/core/Logger.h>
#define LogTag "gui/OGLWidget"

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

            std::string localShaderPath = core::getResourcePath() + "/gfx/shaders/";
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

            // Configure the screenshot widget
            if( m_screenShotWidget )
            {
                GLint maxSamples = 0;
                glGetIntegerv( GL_MAX_SAMPLES, &maxSamples );
                m_screenShotWidget->setMaxSamples( maxSamples );

                GLint maxSize = 0;
                glGetIntegerv(  GL_MAX_TEXTURE_SIZE, &maxSize );
                m_screenShotWidget->setMaxResolution( maxSize );
            }
            else
            {
                LogW << "No ScreenShotWidget defined during initializeGL." << LogEnd;
            }

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
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( now - m_fpsLastTime );
            auto durationLastShow = std::chrono::duration_cast< std::chrono::milliseconds >( now - m_fpsLastShowTime );
            m_fpsLastTime = now;

            float fps = 1000.0 / static_cast< float >( duration.count() );

            if( durationLastShow.count() >= 2000 )
            {
                m_fpsLastShowTime = now;
                LogI << "FPS " << fps << LogEnd;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Get scene BB
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // NOTE: this tries to lock the network first. We are nice and only try to run. If not, we skip the frame.

            // Allow all visualizations to update:
            core::BoundingBox sceneBB;
            Application::getProcessingNetwork()->visitVisualizations(
                [ &sceneBB ]( SPtr< di::core::Visualization > vis )
                {
                    sceneBB.include( vis->getBoundingBox() );
                }
            );

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Update Camera
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

            m_camera.setViewMatrix( m_dragMatrix * moveToVisibleArea * zoom * scaleToFit * m_arcballMatrix * rotationPointTranslate );

            // set a nice default projection matrix:
            m_camera.setProjectionMatrix(
                glm::ortho( 0.5 * -getAspectRatio(), 0.5 * getAspectRatio(), -0.5, 0.5, near, far )
            );

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Define render target
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            core::View* targetView = this;

            // Screenshot?
            if( m_screenShotRequest && !m_screenShotWidget  )
            {
                LogE << "Before requesting a screenshot, define a responsible ScreenShotWidget." << LogEnd;
                m_screenShotRequest = false;
            }

            SPtr< core::OffscreenView > screenshotView = nullptr;
            if( m_screenShotRequest )
            {
                LogD << "Screenshot requested. Creating offscreen view." << LogEnd;

                // Create the offscreen view
                screenshotView = std::make_shared< core::OffscreenView >(
                            glm::vec2( m_screenShotWidget->getWidth(),
                                       m_screenShotWidget->getHeight() ),
                            m_screenShotWidget->getSamples()
                        );

                // Force high quality
                screenshotView->setHQMode( true );

                // The screenshot view might have a different aspect:
                core::Camera offCam( m_camera );
                offCam.setProjectionMatrix(
                    glm::ortho( 0.5 * -screenshotView->getAspectRatio(), 0.5 * screenshotView->getAspectRatio(), -0.5, 0.5, near, far )
                );
                screenshotView->setCamera( offCam );

                // Init the FBO
                screenshotView->prepare();

                targetView = static_cast< core::View* >( screenshotView.get() );
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Update all visualizations
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Allow all visualizations to update:
            Application::getProcessingNetwork()->visitVisualizations(
                [ this, targetView ]( SPtr< di::core::Visualization > vis )
                {
                    vis->update( *targetView, m_forceReload );
                }
            );
            m_forceReload = false;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Draw
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            targetView->bind();
            glViewport( targetView->getViewport().first.x, targetView->getViewport().first.y,
                        targetView->getViewport().second.x, targetView->getViewport().second.y );


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Draw nice background

            // Black background by default
            glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
            // Cleanup buffers
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            // But only if not turned of for screenshots:
            if( m_screenShotRequest && m_screenShotWidget && m_screenShotWidget->getBackgroundOverride() )
            {
                auto col = m_screenShotWidget->getBackgroundColor();
                glClearColor( col.r, col.g, col.b, col.a );
                // Cleanup buffers again to use new clean color
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            }
            else
            {
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
                glDepthMask( GL_FALSE );
                glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Draw visualizations

            glDepthMask( GL_TRUE );
            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glEnable( GL_DEPTH_TEST );

            Application::getProcessingNetwork()->visitVisualizations(
                [ targetView ]( SPtr< di::core::Visualization > vis )
                {
                    if( vis->isRenderingActive() )
                    {
                        targetView->bind();
                        vis->render( *targetView );
                    }
                }
            );

            // thats it.
            m_screenShotRequest = false;
            if( screenshotView )
            {
                // get image and report back
                emit screenshotDone( screenshotView->read() );

                // cleanup the FBO
                screenshotView->finalize();
            }
        }

        void OGLWidget::bind() const
        {
            // unbind previously bound FBOs
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
            glDrawBuffer( GL_BACK );
            logGLError();
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
            if( event->button() == Qt::MiddleButton )
            {
                m_dragState = 1;
                m_dragPrevMatrix = m_dragMatrix;
            }
            if( event->button() == Qt::LeftButton )
            {
                m_arcballState = 1;
                m_arcballPrevMatrix = m_arcballMatrix;
            }
            event->accept();
        }

        void OGLWidget::mouseReleaseEvent( QMouseEvent* event )
        {
            if( event->button() == Qt::MiddleButton )
            {
                m_dragState = 0;
            }
            if( event->button() == Qt::LeftButton )
            {
                m_arcballState = 0;
            }
            event->accept();
        }

        void OGLWidget::mouseMoveEvent( QMouseEvent* event )
        {
            if( ( m_arcballState == 0 ) && ( m_dragState == 0 ) )
            {
                event->ignore();
                return;
            }
            else if( ( m_arcballState == 1 ) || ( m_dragState == 1 ) )
            {
                // Store initial position only.
                m_arcballPrevPos = toScreenCoord( event->x(), event->y() );
                if( m_arcballState == 1 )
                {
                    m_arcballState = 2;
                }
                else
                {
                    m_dragState = 2;
                }
                event->ignore();
                return;
            }
            else if( m_arcballState == 2 )
            {
                // Tracking the subsequent
                m_arcballCurrPos = toScreenCoord( event->x(), event->y() );
                // Calculate the angle in radians, and clamp it between 0 and 90 degrees
                m_arcballAngle = acos( std::min( 1.0f, glm::dot( m_arcballPrevPos, m_arcballCurrPos ) ) );
                // Cross product to get the rotation axis, but it's still in camera coordinate
                m_arcballCamAxis = glm::cross( m_arcballPrevPos, m_arcballCurrPos );

                m_arcballMatrix = glm::rotate( glm::degrees( m_arcballAngle ) * m_arcballRollSpeed, m_arcballCamAxis ) *
                                  m_arcballPrevMatrix;
            }
            else if( m_dragState == 2 )
            {
                auto currPos = toScreenCoord( event->x(), event->y() );
                auto diff = currPos - m_arcballPrevPos;
                m_dragMatrix = glm::translate( glm::vec3( diff.x, diff.y, 0.0 ) ) * m_dragPrevMatrix;
                LogD << "Not yet completed" << diff.x << " - " << diff.y << LogEnd;
            }

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
                case Qt::Key_D:
                    // restore default for our current data. Just a convenient shortcut for now. Later this will not be needed as project
                    // files store the cam too.
                    m_arcballMatrix = glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) *
                                      glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
                    break;
                case Qt::Key_Period:
                    // forced reload
                    m_forceReload = true;
                    break;
                case Qt::Key_F12:
                    m_screenShotRequest = true;
                    break;
            }

            event->accept();
        }

        void OGLWidget::resetView()
        {
            m_arcballMatrix = glm::mat4();
            m_dragMatrix = glm::mat4();
        }

        void OGLWidget::setViewAlongPosX()
        {
            resetView();
            m_arcballMatrix = glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) *
                              glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
        }

        void OGLWidget::setViewAlongNegX()
        {
            resetView();
            m_arcballMatrix = glm::rotate( glm::radians( -90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) *
                              glm::rotate( glm::radians( -90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
        }

        void OGLWidget::setViewAlongPosY()
        {
            resetView();
            m_arcballMatrix = glm::rotate( glm::radians( -90.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
        }

        void OGLWidget::setViewAlongNegY()
        {
            resetView();
            m_arcballMatrix =  glm::rotate( glm::radians( 180.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) *
                               glm::rotate( glm::radians( -90.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
        }

        void OGLWidget::setViewAlongPosZ()
        {
            resetView();
            m_arcballMatrix = glm::rotate( glm::radians( 180.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
        }

        void OGLWidget::setViewAlongNegZ()
        {
            resetView();
            // the default camera
            m_arcballMatrix = glm::mat4();
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

        void OGLWidget::screenshot()
        {
            m_screenShotRequest = true;
            LogD << "Requesting screenshot ..." << LogEnd;
        }

        void OGLWidget::setResponsibleScreenShotWidget( ScreenShotWidget* screenShotWidget )
        {
            m_screenShotWidget = screenShotWidget;
        }
    }
}


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

#ifndef DI_OGLWIDGET_H
#define DI_OGLWIDGET_H

#include <chrono>

#include <di/gfx/GL.h>

#include <QTimer>
#include <QWidget>
// NOTE: QGLWidget is obsolete in Qt5, but the replacement QOpenGLWidget is only available in Qt 5.4+ - we keep QGLWidget for now.
// #include <QOpenGLWidget>
#include <QGLWidget>

#include <di/gfx/PixelData.h>
#include <di/GfxTypes.h>

namespace di
{
    namespace gui
    {
        class ScreenShotWidget;

        /**
         * A wrapper around the Qt OpenGL widget. This specific widget implements the basic interaction and visualization functionalities of this
         * application.
         */
        class OGLWidget: public QGLWidget, public core::View
        {
            Q_OBJECT
        public:
            /**
             * Create the OpenGL widget.
             *
             * \param parent the parent widget.
             */
            explicit OGLWidget( QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~OGLWidget();

            /**
             * Get a default GL format. In our case, this is a OpenGL 3.3 core format.
             *
             * \return the format
             */
            static QGLFormat getDefaultFormat();

            /**
             * Get the origin of the viewport.
             *
             * \return the origin
             */
            glm::vec2 getViewportOrigin() const override;

            /**
             * Gets the size of the viewport in x and y directions. Keep in mind that the physical pixel coordinate of the upper-right corner will be
             * origin + size - (1,1). Alternatively, use \ref getViewport() to query this directly.
             *
             * \return
             */
            glm::vec2 getViewportSize() const override;

            /**
             * Get the camera of this view.
             *
             * \return the camera.
             */
            const core::Camera& getCamera() const override;

            /**
             * Bind this view as rendering target. You should issue this if you use your own FBOs.
             */
            void bind() const override;

            /**
             * Set the screenshot widget responsible for this view. You should set this before showing the OGLWidget.
             *
             * \param screenShotWidget the widget.
             */
            void setResponsibleScreenShotWidget( ScreenShotWidget* screenShotWidget );

            /**
             * Define a matrix as the default view. Default is identity.
             *
             * \param view the view to use as default
             */
            void setViewPreset( const glm::mat4& view = glm::mat4() );

        signals:
            /**
             * Issued whenever a screenshot is ready.
             *
             * \param image the pixel data of the image.
             */
            void screenshotDone( SPtr< core::RGBA8Image > image );

        public slots:

            /**
             * Call to take a screenshot.
             */
            void screenshot();

            /**
             * Reset the view to XY-Z
             */
            void resetView();

            /**
             * Set the camera transformation to view along positive x.
             */
            void setViewAlongPosX();

            /**
             * Set the camera transformation to view along negative x.
             */
            void setViewAlongNegX();

            /**
             * Set the camera transformation to view along positive y.
             */
            void setViewAlongPosY();

            /**
             * Set the camera transformation to view along negative y.
             */
            void setViewAlongNegY();

            /**
             * Set the camera transformation to view along positive z.
             */
            void setViewAlongPosZ();

            /**
             * Set the camera transformation to view along negative z.
             */
            void setViewAlongNegZ();
        protected:
            /**
             * Do the necessary setup.
             */
            void initializeGL() override;

            /**
             * Resize. Needed for camera setup, viewports and similar.
             *
             * \param w the width
             * \param h the height
             */
            void resizeGL( int w, int h ) override;

            /**
             * Paint. This iterates the visualizations and lets them draw.
             */
            void paintGL() override;

            /**
             * Widget is going to be destroyed.
             *
             * \param event the event
             */
            void closeEvent( QCloseEvent* event ) override;

            /**
             * Mouse button pressed.
             *
             * \param event the event
             */
            void mousePressEvent( QMouseEvent* event ) override;

            /**
             * Mouse button released.
             *
             * \param event the event
             */
            void mouseReleaseEvent( QMouseEvent* event ) override;

            /**
             * Mouse move.
             *
             * \param event the move event
             */
            void mouseMoveEvent( QMouseEvent* event ) override;

            /**
             * Mouse wheel - used for zoom
             *
             * \param event the wheel event
             */
            void wheelEvent( QWheelEvent* event ) override;

            /**
             * Key events. Used to implement interactions with cam and so on.
             *
             * \param event the event
             */
            void keyReleaseEvent( QKeyEvent* event ) override;

            /**
             * Convert the given mouse coordinates to normalized screen coordinates.
             *
             * \param x x mouse coordinate
             * \param y y mouse coordinate
             *
             * \return screen coordinate
             */
            glm::vec3 toScreenCoord( double x, double y );

        private:
            /**
             * If true, all visualizations are asked to reload.
             */
            bool m_forceReload = false;

            /**
             * Last time used for FPS counting.
             */
            std::chrono::time_point< std::chrono::system_clock > m_fpsLastTime;

            /**
             * Print FPS only once per second
             */
            std::chrono::time_point< std::chrono::system_clock > m_fpsLastShowTime;

            /**
             * The VBO used for the background.
             */
            GLuint m_backgroundVBO = 0;

            /**
             * The Vertex Attribute Array Object (VAO) used for the background.
             */
            GLuint m_backgroundVAO = 0;

            /**
             * The shader used for rendering
             */
            SPtr< di::core::Program > m_bgShaderProgram = nullptr;

            /**
             * Vertex shader.
             */
            SPtr< di::core::Shader > m_bgVertexShader = nullptr;

            /**
             * Fragment shader.
             */
            SPtr< di::core::Shader > m_bgFragmentShader = nullptr;

            /**
             * Redraw periodically. Only temporary solution.
             */
            QTimer* m_redrawTimer;

            /**
             * State of the mouse drag feature
             */
            int m_dragState = 0;

            /**
             * Use the arcball functionality.
             */
            int m_arcballState = 0;

            /**
             * Previous position.
             */
            glm::vec3 m_arcballPrevPos;

            /**
             * Current position.
             */
            glm::vec3 m_arcballCurrPos;

            /**
             * Camera axis to use.
             */
            glm::vec3 m_arcballCamAxis = glm::vec3( 0.0f, 1.0f, 0.0f );

            /**
             * Roll speed.
             */
            GLfloat m_arcballRollSpeed = 0.10;

            /**
             * Current angle.
             */
            GLfloat m_arcballAngle = 0.0;

            /**
             * Arcball on X axis.
             */
            bool m_arcballXAxis = true;

            /**
             * Arcball on Y axis.
             */
            bool m_arcballYAxis = true;

            /**
             * The default view.
             */
            glm::mat4 m_defaultView;

            /**
             * The current drag matrix.
             */
            glm::vec2 m_dragOffset;

            /**
             * The previous drag matrix.
             */
            glm::vec2 m_dragPrevOffset;

            /**
             * The current arcball matrix.
             */
            glm::mat4 m_arcballMatrix;

            /**
             * The previous arcball matrix.
             */
            glm::mat4 m_arcballPrevMatrix;

            /**
             * Camera zoom.
             *
             * The default is 1.732 (~sqrt(3)) - 0.1. This copes with the fact that the down-scaler to fit to the scene calculates the diagonal of
             * the scene cube. The default zoom ensures a maximally zoomed initial scene.
             */
            double m_zoom = 1.732;

            /**
             * The camera of the view.
             */
            core::Camera m_camera;

            /**
             * If true, a screenshot is requested.
             */
            bool m_screenShotRequest = false;

            /**
             * The widget responsible for setting up the screenshotter
             */
            ScreenShotWidget* m_screenShotWidget = nullptr;
        };
    }
}

#endif  // DI_OGLWIDGET_H


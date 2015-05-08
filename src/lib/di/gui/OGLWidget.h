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
#include <tuple>

#include <di/gfx/GL.h>

#include <QTimer>
#include <QWidget>
// NOTE: QGLWidget is obsolete in Qt5, but the replacement QOpenGLWidget is only available in Qt 5.4+ - we keep QGLWidget for now.
// #include <QOpenGLWidget>
#include <QGLWidget>

#include <di/core/State.h>
#include <di/core/BoundingBox.h>
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
             * \return the size of the viewport
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

            /**
             * Get the state object representing this object at the moment of the call.
             *
             * \return  the state
             */
            virtual di::core::State getState() const override;

            /**
             * Apply the state to this instance.
             *
             * \param state the state to set
             *
             * \return  true if everything was fine.
             */
            virtual bool setState( const di::core::State& state ) override;

            /**
             * Set the given list as the default views. Activate them by calling \ref useDefaultView. Note that only orientation matrices are useful
             * here.
             *
             * \param defaultViews the list of default views. The tuple is the matrix, a name as string and a boolean flag telling the system if the
             * matrix is a whole view matrix or only an orientation matrix. Set true for orientation-mode. It automatically adapts the view to contain
             * the whole scene.
             */
            void setDefaultViews( const std::vector< std::tuple< glm::mat4, bool, std::string > >& defaultViews );

            /**
             * Convenience function to get a view matrix matching the name of this method. Useful as base for further manipulations.
             *
             * \return the matrix.
             */
            glm::mat4 getOrientationMatrixAlongPosX() const;

            /**
             * Convenience function to get a orientation matrix matching the name of this method. Useful as base for further manipulations.
             *
             * \return the matrix.
             */
            glm::mat4 getOrientationMatrixAlongNegX() const;

            /**
             * Convenience function to get a orientation matrix matching the name of this method. Useful as base for further manipulations.
             *
             * \return the matrix.
             */
            glm::mat4 getOrientationMatrixAlongPosY() const;

            /**
             * Convenience function to get a orientation matrix matching the name of this method. Useful as base for further manipulations.
             *
             * \return the matrix.
             */
            glm::mat4 getOrientationMatrixAlongNegY() const;

            /**
             * Convenience function to get a orientation matrix matching the name of this method. Useful as base for further manipulations.
             *
             * \return the matrix.
             */
            glm::mat4 getOrientationMatrixAlongPosZ() const;

            /**
             * Convenience function to get a orientation matrix matching the name of this method. Useful as base for further manipulations.
             *
             * \return the matrix.
             */
            glm::mat4 getOrientationMatrixAlongNegZ() const;

            /**
             * Build a view matrix based on the _current_ scene. Use this method only if you want a matrix that is based on the current scene. If the
             * scene changes, this matrix might not be useful anymore.
             *
             * \param sceneBB the bounding box of the scene to view.
             * \param orientation orientation
             * \param zoom a zoom factor. Default = 1.0.
             * \param drag a drag offset. Defaults to glm::vec2( 0.0 ).
             *
             * \return the matrix.
             */
            glm::mat4 buildViewMatrix( const core::BoundingBox& sceneBB, const glm::mat4& orientation, float zoom = 1.0,
                                       const glm::vec2& drag = glm::vec2( 0.0 ) );

            /**
             * Build a projection matrix.
             *
             * \param near the near value
             * \param far the far value
             * \param aspect the aspect ratio
             *
             * \return the projection matrix.
             */
            glm::mat4 buildProjectionMatrix( float near, float far, float aspect );

        signals:
            /**
             * Issued whenever a screenshot is ready.
             *
             * \param image the pixel data of the image.
             * \param nameHint a hint how to name the screenshot.
             * \param pathOverride the path where to store the image. Can be empty to use the user specified path.
             */
            void screenshotDone( SPtr< core::RGBA8Image > image, const std::string& nameHint, const std::string& pathOverride = "" );

            /**
             * Called when screenshots where requested and successfully saved.
             */
            void allScreenshotsDone();

        public slots:

            /**
             * Call to take a screenshot.
             *
             * \param path override the user path. The path MUST be absolute. If empty, the default path is used.
             */
            void screenshot( std::string path = "" );

            /**
             * Reset the view to XY-Z
             */
            void resetView();

            /**
             * Set the view to match the given default.
             *
             * \param id the number of the default, as set by \ref setDefaultViews. If the number is too large, the last one is set.
             */
            void useDefaultView( size_t id );

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
             * The default view to use when resetting the view.
             */
            glm::mat4 m_viewPreset;

            /**
             * All the possible default views as matrix and name.
             */
            std::vector< std::tuple< glm::mat4, bool, std::string > > m_defaultViews;

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
             * Override the path of the screenshots.
             */
            std::string m_screenShotPathOverride = "";

            /**
             * The widget responsible for setting up the screenshotter
             */
            ScreenShotWidget* m_screenShotWidget = nullptr;

            /**
             * Render the whole scene to this view.
             *
             * \param view the view.
             */
            void renderToView( core::View* view );
        };
    }
}

#endif  // DI_OGLWIDGET_H


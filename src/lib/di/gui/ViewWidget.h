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

#ifndef DI_VIEWWIDGET_H
#define DI_VIEWWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QHBoxLayout>

#include <di/gfx/PixelData.h>
#include <di/Types.h>

namespace di
{
    namespace gui
    {
        class OGLWidget;
        class ScreenShotWidget;

        /**
         * A simple widget to provide the view to a scene.
         */
        class ViewWidget: public QDockWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the view widget.
             *
             * \param parent the parent widget.
             * \param title the window title
             */
            ViewWidget( const QString& title, QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~ViewWidget();

            /**
             * Sets the default views to be the medical view anterior, posterior, inferior, superior, left, right
             */
            void setMedicalDefaultViews();

            /**
             * Sets the default views to be the technical views +x,-x, ...
             */
            void setTechnicalDefaultViews();

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
            di::core::State getState() const;

            /**
             * Apply the state to this instance.
             *
             * \param state the state to set
             *
             * \return  true if everything was fine.
             */
            bool setState( const di::core::State& state );

            /**
             * Same as the \ref screenshotDone signal, but does not use the Qt mechanism. Useful when using from non-QObject classes.
             *
             * \param func function to notify
             */
            void screenshotDone( std::function< void() > func );

        signals:
            /**
             * Reports back whenever all screenshots were taken that have been requested by \ref screenshot().
             */
            void screenshotDone();

        public slots:
            /**
             * Take screenshot.
             *
             * \param path override the user path. The path MUST be absolute. If empty, the default path is used.
             */
            void screenshot( std::string path = "" );

        protected slots:
            /**
             * Reports back whenever a screenshot was taken.
             *
             * \param image the pixel data of the image.
             * \param nameHint a hint how to name the screenshot.
             * \param pathOverride the path where to store the image. Can be empty to use the user specified path.
             */
            void screenshotDone( SPtr< core::RGBA8Image > image, const std::string& nameHint, const std::string& pathOverride = "" );

            /**
             * Reports back whenever all screenshots were taken.
             */
            void allScreenshotsDone();

            /**
             * A default view was triggered.
             */
            void defaultViewTriggered();

        protected:
        private:
            /**
             * The contents
             */
            QVBoxLayout* m_contentLayout = nullptr;

            /**
             * The title widget
             */
            QHBoxLayout* m_titleLayout = nullptr;

            /**
             * The rendering
             */
            OGLWidget* m_oglWidget = nullptr;

            /**
             * The screenshot widget.
             */
            ScreenShotWidget* m_screenShotWidget = nullptr;

            /**
             * Button for screenshots
             */
            QToolButton* m_screenshotButton;

            /**
             * Button for the default views
             */
            QToolButton* m_defaultViewsButton;

            /**
             * The menu used for the view actions
             */
            QMenu* m_defaultViewsMenu;

            /**
             * The actions to add to the view menu
             */
            std::vector< QAction* > m_viewActions;

            /**
             * The callbacks to use when screenshots where done.
             */
            std::vector< std::function< void() > > m_screenshotDoneCallbacks;
        };
    }
}

#endif  // DI_VIEWWIDGET_H



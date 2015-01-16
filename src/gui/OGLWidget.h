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

#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <GL/gl.h>

#include <QWidget>
// NOTE: QGLWidget is obsolete in Qt5, but the replacement QOpenGLWidget is only available in Qt 5.4+ - we keep QGLWidget for now.
// #include <QOpenGLWidget>
#include <QGLWidget>
#include <QTimer>

namespace di
{
    namespace gui
    {
        /**
         * A wrapper around the Qt OpenGL widget. This specific widget implements the basic interaction and visualization functionalities of this
         * application.
         */
        class OGLWidget: public QGLWidget
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
        protected:
            /**
             * Do the necessary setup.
             */
            virtual void initializeGL();

            /**
             * Resize. Needed for camera setup, viewports and similar.
             *
             * \param w the width
             * \param h the height
             */
            virtual void resizeGL( int w, int h );

            /**
             * Paint. This iterates the visualizations and lets them draw.
             */
            virtual void paintGL();

            /**
             * Widget is going to be destroyed.
             *
             * \param event the event
             */
            virtual void closeEvent( QCloseEvent* event );
        private:
            /**
             * The VBO used for the background.
             */
            GLuint m_backgroundVBO = 0;

            /**
             * The Vertex Attribute Array Object (VAO) used for the background.
             */
            GLuint m_backgroundVAO = 0;

            /**
             * The shader used for bg rendering
             */
            GLuint m_backgroundShaderProgram = 0;

            /**
             * Redraw periodically. Only temporary solution.
             */
            QTimer* m_redrawTimer;
        };
    }
}

#endif  // OGLWIDGET_H


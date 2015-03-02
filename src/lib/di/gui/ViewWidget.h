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
#include <QHBoxLayout>

#include <di/gui/OGLWidget.h>

#include <di/Types.h>

namespace di
{
    namespace gui
    {
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
        };
    }
}

#endif  // DI_VIEWWIDGET_H



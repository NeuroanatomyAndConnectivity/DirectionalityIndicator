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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

#include <di/gui/OGLWidget.h>
#include <di/gui/ScaleLabel.h>

#include "ViewWidget.h"

namespace di
{
    namespace gui
    {
        ViewWidget::ViewWidget( const QString& title, QWidget* parent ):
            QDockWidget( parent )
        {
            setWindowTitle( title );
            setObjectName( "ViewWidget_" + title );    // needed for persistent GUI states

            // avoid closable docks.
            setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
            setFeatures( 0 );

            // NOTE: for now, the file widgets are hard-coded. This might be changed in the future to allow programmers to define the data they
            // require.

            // Create the content container
            QWidget* contentWidget( new QWidget( this ) );
            setWidget( contentWidget );
            m_contentLayout = new QVBoxLayout;
            contentWidget->setLayout( m_contentLayout );

            m_contentLayout->setMargin( 0 );
            m_contentLayout->setContentsMargins( 0, 0, 0, 0 );

            // The title bar
            QWidget* titleWidget( new QWidget( this ) );
            m_titleLayout = new QHBoxLayout();
            m_titleLayout->setContentsMargins( 5, 1, 1, 5 );
            titleWidget->setLayout( m_titleLayout );
            this->setTitleBarWidget( titleWidget );

            // Add title:
            m_titleLayout->addWidget( new di::gui::ScaleLabel( title ) );

            // The GL widget
            m_oglWidget = new di::gui::OGLWidget();
            m_contentLayout->addWidget( m_oglWidget );
        }

        ViewWidget::~ViewWidget()
        {
        }
    }
}


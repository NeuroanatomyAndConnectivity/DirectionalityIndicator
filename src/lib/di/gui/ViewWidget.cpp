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
#include <QWidgetAction>
#include <QMenu>
#include <QMessageBox>

#include <di/gui/OGLWidget.h>
#include <di/gui/ScaleLabel.h>
#include <di/gui/ScreenShotWidget.h>

#include "icons/screenshot.xpm"
#include "icons/configure.xpm"

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
            m_titleLayout->setContentsMargins( 5, 1, 5, 1 );
            titleWidget->setLayout( m_titleLayout );

            // Add title:
            this->setTitleBarWidget( titleWidget );

            // The GL widget
            m_oglWidget = new di::gui::OGLWidget();
            m_contentLayout->addWidget( m_oglWidget );

            // The screen-shot widget
            m_screenShotWidget = new ScreenShotWidget( this );
            m_oglWidget->setResponsibleScreenShotWidget( m_screenShotWidget );

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Title Bar Content

            // Title
            m_titleLayout->addWidget( new di::gui::ScaleLabel( title ) );
            m_titleLayout->addStretch( 1000 );

            // Close Button ?
            /* auto closeButton = new QToolButton( titleWidget );
            closeButton->setAutoRaise( true );
            closeButton->setIcon( QIcon( QPixmap( screenshot ) ) );
            m_titleLayout->addWidget( closeButton ); */

            m_screenshotButton = new QToolButton( titleWidget );
            m_screenshotButton->setAutoRaise( true );
            m_screenshotButton->setText( "Screenshot" );
            m_screenshotButton->setToolTip( "Take screenshot of the current scene." );
            m_screenshotButton->setIcon( QIcon( QPixmap( xpm_screenshot ) ) );
            m_titleLayout->addWidget( m_screenshotButton );

            auto screenshotMenu = new QMenu( "Screenshot Settings", m_screenshotButton );
            auto screenShowWidgetAction = new QWidgetAction( this );
            screenShowWidgetAction->setDefaultWidget( m_screenShotWidget );
            screenshotMenu->addAction( screenShowWidgetAction );
            m_screenshotButton->setMenu( screenshotMenu );
            m_screenshotButton->setPopupMode( QToolButton::MenuButtonPopup );

            /*
            auto configButton = new QToolButton( titleWidget );
            configButton->setAutoRaise( true );
            configButton->setText( "Configure" );
            configButton->setToolTip( "Show configuration options for this view." );
            configButton->setIcon( QIcon( QPixmap( xpm_configure ) ) );
            m_titleLayout->addWidget( configButton );
            */

            // Wire them:
            connect( m_screenshotButton, SIGNAL( released() ), this, SLOT( screenshot() ) );
            connect( m_oglWidget, SIGNAL( screenshotDone( SPtr< core::RGBA8Image > ) ),
                    this, SLOT( screenshotDone( SPtr< core::RGBA8Image > ) ) );
        }

        ViewWidget::~ViewWidget()
        {
        }

        void ViewWidget::screenshot()
        {
            m_screenshotButton->setDisabled( true );

            // and forward
            m_oglWidget->screenshot();
        }

        void ViewWidget::screenshotDone( SPtr< core::RGBA8Image > pixels )
        {
            m_screenshotButton->setDisabled( false );

            // and forward
            if( !m_screenShotWidget->saveScreenShot( pixels ) )
            {
                // report
                QMessageBox::critical( this, "Screenshot failed.", "Unable to write the screenshot to disk." );
            }
        }
    }
}


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
#include "icons/view.xpm"

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

            // View Menu
            m_defaultViewsButton = new QToolButton( titleWidget );
            m_defaultViewsButton->setAutoRaise( true );
            m_defaultViewsButton->setText( "Default Views" );
            m_defaultViewsButton->setToolTip( "Switch between different default views." );
            m_defaultViewsButton->setIcon( QIcon( QPixmap( xpm_view ) ) );
            m_titleLayout->addWidget( m_defaultViewsButton );

            m_defaultViewsMenu = new QMenu( "Default Views", m_defaultViewsButton );
            m_defaultViewsButton->setMenu( m_defaultViewsMenu );
            m_defaultViewsButton->setPopupMode( QToolButton::MenuButtonPopup );
            m_defaultViewsMenu->addAction( "Reset", m_oglWidget, SLOT( resetView() ), QKeySequence( Qt::Key_Space ) );
            setMedicalDefaultViews();

            // Wire them:
            connect( m_screenshotButton, SIGNAL( released() ), this, SLOT( screenshot() ) );
            connect( m_defaultViewsButton, SIGNAL( released() ), m_oglWidget, SLOT( resetView() ) );
            connect( m_oglWidget, SIGNAL( screenshotDone( SPtr< core::RGBA8Image >, const std::string& ) ),
                     this, SLOT( screenshotDone( SPtr< core::RGBA8Image >, const std::string& ) ) );
        }

        ViewWidget::~ViewWidget()
        {
        }

/*

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
            m_arcballMatrix = glm::rotate( glm::radians( 180.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) *
                              glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
        }

        void OGLWidget::setViewAlongNegZ()
        {
            resetView();
            // the default camera
            m_arcballMatrix = glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
        }
*/

        void ViewWidget::setTechnicalDefaultViews()
        {
            // Remove old actions first
            for( auto a : m_viewActions )
            {
                m_defaultViewsMenu->removeAction( a );
                delete a;
            }
            m_viewActions.clear();

            // build a list
            std::vector< std::tuple< glm::mat4, bool, std::string > > views;

            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongNegX(), true, "Along -X, YZ Plane" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongPosX(), true, "Along +X, YZ Plane" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongNegY(), true, "Along -Y, XZ Plane" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongPosY(), true, "Along +Y, XZ Plane" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongNegZ(), true, "Along -Z, XY Plane" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongPosZ(), true, "Along +Z, XY Plane" ) );

            m_oglWidget->setDefaultViews( views );

            // Create actions for it.

            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Along -X, YZ Plane", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_R ) ) );
            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Along +X, YZ Plane", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_L ) ) );

            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Along -Y, XZ Plane", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_A ) ) );
            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Along +Y, XZ Plane", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) ) );

            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Along -Z, XY Plane", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_S ) ) );

            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Along +Z, XY Plane", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) ) );
        }

        void ViewWidget::setMedicalDefaultViews()
        {
            // Remove old actions first
            for( auto a : m_viewActions )
            {
                m_defaultViewsMenu->removeAction( a );
                delete a;
            }
            m_viewActions.clear();

            // build a list
            std::vector< std::tuple< glm::mat4, bool, std::string > > views;

            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongNegY(), true, "Anterior" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongPosY(), true, "Posterior" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongPosX(), true, "Left" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongNegX(), true, "Right" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongNegZ(), true, "Superior" ) );
            views.push_back( std::make_tuple( m_oglWidget->getOrientationMatrixAlongPosZ(), true, "Inferior" ) );

            m_oglWidget->setDefaultViews( views );

            // Create actions for it.
            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Anterior", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_A ) ) );
            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Posterior", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) ) );

            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Left", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_L ) ) );
            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Right", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_R ) ) );

            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Superior", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_S ) ) );
            m_viewActions.push_back( m_defaultViewsMenu->addAction( "Inferior", this, SLOT( defaultViewTriggered() ),
                                                                    QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) ) );
        }

        void ViewWidget::screenshot()
        {
            m_screenshotButton->setDisabled( true );

            // and forward
            m_oglWidget->screenshot();
        }

        void ViewWidget::defaultViewTriggered()
        {
            QObject* obj = sender();
            auto action = dynamic_cast< QAction* >( obj );

            auto actionIt = std::find( m_viewActions.begin(), m_viewActions.end(), action );
            if( actionIt != m_viewActions.end() )
            {
                size_t id = actionIt - m_viewActions.begin();
                m_oglWidget->useDefaultView( id );
            }
        }

        void ViewWidget::screenshotDone( SPtr< core::RGBA8Image > pixels, const std::string& nameHint )
        {
            m_screenshotButton->setDisabled( false );

            // and forward
            if( !m_screenShotWidget->saveScreenShot( pixels, nameHint ) )
            {
                // report
                QMessageBox::critical( this, "Screenshot failed.", "Unable to write the screenshot to disk." );
            }
        }

        void ViewWidget::setViewPreset( const glm::mat4& view )
        {
            m_oglWidget->setViewPreset( view );
        }

        di::core::State ViewWidget::getState() const
        {
            // only store the camera. Everything else is managed by the rendering/windowing system
            return m_oglWidget->getState();
        }

        bool ViewWidget::setState( const di::core::State& state )
        {
            return m_oglWidget->setState( state );
        }
    }
}


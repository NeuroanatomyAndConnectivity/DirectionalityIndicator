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
#include <QFileDialog>
#include <QDir>

#include "Application.h"
#include "ScaleLabel.h"

#define LogTag "gui/DataWidget"
#include "core/Logger.h"

// include some icons as XPM. This will be replaced by a proper file loading.
#include "icons/iconMesh.xpm"
#include "icons/iconLabels.xpm"

#include "CommandObserverQt.h"
#include "events/CommandObserverEvent.h"

#include "DataWidget.h"

namespace di
{
    namespace gui
    {
        DataWidget::DataWidget( QWidget* parent ):
            QDockWidget( parent )
        {
            setWindowTitle( tr( "Dataset" ) );
            setObjectName( "DataWidget" );    // needed for persistent GUI states

            // avoid closable docks.
            setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

            // NOTE: for now, the file widgets are hard-coded. This might be changed in the future to allow programmers to define the data they
            // require.

            // Create the content container
            QWidget* contentWidget( new QWidget( this ) );
            setWidget( contentWidget );
            QVBoxLayout* contentLayout( new QVBoxLayout );
            contentWidget->setLayout( contentLayout );

            // Add the mesh-load-button thingy
            QWidget* meshLoad( new QWidget );
            QHBoxLayout* meshLoadLayout( new QHBoxLayout );
            meshLoad->setLayout( meshLoadLayout );
            m_meshLoadBtn = new QToolButton;
            m_meshLoadBtn->setIcon( QIcon( QPixmap( iconMesh_xpm ) ) );
            m_meshLoadBtn->setIconSize( QSize( 64, 64 ) );
            m_meshLoadLabel = new ScaleLabel;
            meshLoadLayout->addWidget( m_meshLoadBtn );
            meshLoadLayout->addWidget( m_meshLoadLabel );
            m_meshLoadLabel->setText( tr( "No Data Loaded" ) );

            // Add the label-load-button thingy
            QWidget* labelLoad( new QWidget );
            QHBoxLayout* labelLoadLayout( new QHBoxLayout );
            labelLoad->setLayout( labelLoadLayout );
            m_labelLoadBtn = new QToolButton;
            m_labelLoadBtn->setIcon( QIcon( QPixmap( iconLabels_xpm ) ) );
            m_labelLoadBtn->setIconSize( QSize( 64, 64 ) );
            m_labelLoadLabel = new ScaleLabel;
            labelLoadLayout->addWidget( m_labelLoadBtn );
            labelLoadLayout->addWidget( m_labelLoadLabel );
            m_labelLoadLabel->setText( tr( "No Data Loaded" ) );

            // Add the data status indicator
            QWidget* statusIndicator( new QWidget );
            QHBoxLayout* statusIndicatorLayout( new QHBoxLayout );
            statusIndicator->setLayout( statusIndicatorLayout );

            m_statusLabel = new ScaleLabel;
            m_statusLabel->setText( tr( "Not Complete" ) );
            ScaleLabel* statusInfoLabel = new ScaleLabel;   // only shows the static text
            statusInfoLabel->setText( tr( "Data Status: " ) );

            statusIndicatorLayout->addWidget( statusInfoLabel );
            statusIndicatorLayout->addWidget( m_statusLabel );

            contentLayout->addWidget( meshLoad );
            contentLayout->addWidget( labelLoad );
            contentLayout->addWidget( statusIndicator );
            contentLayout->setAlignment( Qt::AlignTop );

            // setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );

            // Connect the tool buttons to an actual file dialog
            connect( m_meshLoadBtn, SIGNAL( clicked( bool ) ), this, SLOT( loadMesh() ) );
            connect( m_labelLoadBtn, SIGNAL( clicked( bool ) ), this, SLOT( loadLabels() ) );
        }

        DataWidget::~DataWidget()
        {
        }

        void DataWidget::prepareProcessingNetwork()
        {
            // We use DataInject algorithms to inject data we have loaded (or will load)
        }

        void DataWidget::loadMesh()
        {
            QString lastPath = Application::getSettings()->value( "LastMeshPath", "" ).toString();
            QString selected = QFileDialog::getOpenFileName( this, "Load Mesh File", lastPath,
                                                                   "Stanford Poly Format (*.ply)" );
            /* Not yet implemented
                                                                   "Mesh File (*.gii *.asc *.ply);;
                                                                   GIfTI File (*.gii);; " +
                                                                   "ASCII Mesh File (*.asc);; " +
                                                                   "Stanford Poly Format (*.ply)" );
                                                                   */
            if( selected == "" )
            {
                return;
            }

            // Store the last path
            QFileInfo fi( selected );
            Application::getSettings()->setValue( "LastMeshPath", fi.path() );

            // Use deferred loading:
            Application::getProcessingNetwork()->loadFile( selected.toStdString(),
                                                       SPtr< CommandObserverQt >(
                                                           new CommandObserverQt( this, { m_meshLoadLabel, m_meshLoadLabel } )
                                                       )
                                                      );
        }

        void DataWidget::loadLabels()
        {
            QString lastPath = Application::getSettings()->value( "LastLabelPath", "" ).toString();
            QString selected = QFileDialog::getOpenFileName( this, "Load Label File", lastPath,
                                                                   "Label File (*.dpv)" );
            if( selected == "" )
            {
                return;
            }

            // Store the last path
            QFileInfo fi( selected );
            Application::getSettings()->setValue( "LastLabelPath", fi.path() );

            // Use deferred loading:
            Application::getProcessingNetwork()->loadFile( selected.toStdString(),
                                                       SPtr< CommandObserverQt >(
                                                           new CommandObserverQt( this, { m_labelLoadLabel, m_labelLoadLabel } )
                                                       )
                                                      );
        }

        bool DataWidget::event( QEvent* event )
        {
            // QT_COMMANDOBSERVER_EVENT?
            if( event->type() == QT_COMMANDOBSERVER_EVENT )
            {
                CommandObserverEvent* coe = dynamic_cast< CommandObserverEvent* >( event );

                // status of a CommandObserver changed
                if( coe->getObserverStatus() == CommandObserverEvent::GENERIC )
                {
                    // do not know what to do.
                }

                if( coe->getObserverStatus() == CommandObserverEvent::BUSY )
                {
                    coe->getObserver()->getReceiver()->setEnabled( false );

                    // Update Text
                    ScaleLabel* label = dynamic_cast< ScaleLabel* >( coe->getObserver()->getAffected()[1] );
                    label->setText( "Busy" );
                }

                if( coe->getObserverStatus() == CommandObserverEvent::WAITING )
                {
                    coe->getObserver()->getReceiver()->setEnabled( false );

                    // Update Text
                    ScaleLabel* label = dynamic_cast< ScaleLabel* >( coe->getObserver()->getAffected()[1] );
                    label->setText( "Waiting" );
                }

                if( coe->getObserverStatus() == CommandObserverEvent::SUCCESS )
                {
                    coe->getObserver()->getReceiver()->setEnabled( true );

                    // Update Text
                    ScaleLabel* label = dynamic_cast< ScaleLabel* >( coe->getObserver()->getAffected()[1] );
                    label->setText( "Success" );
                }

                if( coe->getObserverStatus() == CommandObserverEvent::ABORT )
                {
                    coe->getObserver()->getReceiver()->setEnabled( true );

                    // Update Text
                    ScaleLabel* label = dynamic_cast< ScaleLabel* >( coe->getObserver()->getAffected()[1] );
                    label->setText( "Abort" );

                }

                if( coe->getObserverStatus() == CommandObserverEvent::FAIL )
                {
                    coe->getObserver()->getReceiver()->setEnabled( true );

                    // Update Text
                    ScaleLabel* label = dynamic_cast< ScaleLabel* >( coe->getObserver()->getAffected()[1] );
                    label->setText( "Failed" );
                }
            }

            return QDockWidget::event( event );
        }
    }
}


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

#define LogTag "gui/FileWidget"
#include "core/Logger.h"

#include "CommandObserverQt.h"
#include "events/CommandObserverEvent.h"

#include "algorithms/DataInject.h"

#include "FileWidget.h"

namespace di
{
    namespace gui
    {
        FileWidget::FileWidget( const QIcon& icon, const QString& fileFilter, QWidget* parent ):
            QWidget( parent ),
            m_fileFilter( fileFilter )
        {
            // Add the load-button thingy
            QHBoxLayout* fileLoadLayout( new QHBoxLayout );
            setLayout( fileLoadLayout );
            m_fileLoadBtn = new QToolButton;
            m_fileLoadBtn->setIcon( icon );
            m_fileLoadBtn->setIconSize( QSize( 64, 64 ) );
            m_fileLoadLabel = new ScaleLabel;
            fileLoadLayout->addWidget( m_fileLoadBtn );
            fileLoadLayout->addWidget( m_fileLoadLabel );
            m_fileLoadLabel->setText( tr( "No Data Loaded" ) );

            // Connect the tool buttons to an actual file dialog
            connect( m_fileLoadBtn, SIGNAL( clicked( bool ) ), this, SLOT( loadFile() ) );
        }

        FileWidget::~FileWidget()
        {
        }

        void FileWidget::prepareProcessingNetwork()
        {
            // We use DataInject algorithms to inject data we have loaded (or will load)
            m_dataInject = SPtr< di::algorithms::DataInject >( new di::algorithms::DataInject() );
            Application::getProcessingNetwork()->addAlgorithm( m_dataInject );
        }

        void FileWidget::loadFile()
        {
            QString lastPath = Application::getSettings()->value( "LastFilePath", "" ).toString();
            QString selected = QFileDialog::getOpenFileName( this, "Load File", lastPath, m_fileFilter );
            if( selected == "" )
            {
                return;
            }

            // Store the last path
            QFileInfo fi( selected );
            Application::getSettings()->setValue( "LastFilePath", fi.path() );

            // Use deferred loading:
            Application::getProcessingNetwork()->loadFile( selected.toStdString(),
                                                       SPtr< CommandObserverQt >(
                                                           new CommandObserverQt( this, { m_fileLoadLabel, m_fileLoadLabel } )
                                                       )
                                                      );
        }

        bool FileWidget::event( QEvent* event )
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

                    // Inject to network
                    m_dataInject->inject( coe->getIssuer< di::commands::ReadFile >()->getResult() );
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
                    label->setText( "Failed: " + QString::fromStdString( coe->getIssuer()->getFailureReason() ) );
                }
            }

            return QWidget::event( event );
        }
    }
}


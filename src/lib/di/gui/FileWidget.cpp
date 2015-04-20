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

#include <di/gui/Application.h>
#include <di/gui/ScaleLabel.h>

#include <di/gui/CommandObserverQt.h>
#include <di/gui/events/CommandObserverEvent.h>

#include <di/algorithms/DataInject.h>

#include <di/core/Logger.h>
#define LogTag "gui/FileWidget"

#include "FileWidget.h"

namespace di
{
    namespace gui
    {
        FileWidget::FileWidget( const QString& title, const QIcon& icon, const QString& fileFilter, QWidget* parent ):
            QWidget( parent ),
            m_fileFilter( fileFilter ),
            m_title( title )
        {
            // Add the load-button thingy
            QHBoxLayout* fileLoadLayout( new QHBoxLayout );
            setLayout( fileLoadLayout );
            m_fileLoadBtn = new QToolButton;
            m_fileLoadBtn->setIcon( icon );
            m_fileLoadBtn->setIconSize( QSize( 48, 48 ) );
            m_fileLoadLabel = new ScaleLabel;
            fileLoadLayout->addWidget( m_fileLoadBtn );
            fileLoadLayout->setMargin( 5 );

            // different layout for title and text
            QVBoxLayout* fileInfoLayout( new QVBoxLayout );
            fileLoadLayout->addLayout( fileInfoLayout );

            auto titleLabel = new ScaleLabel;
            titleLabel->setText( title );
            titleLabel->setStyleSheet( "font-weight:bold;" );

            fileInfoLayout->addWidget( titleLabel );
            fileInfoLayout->addWidget( m_fileLoadLabel );
            m_fileLoadLabel->setText( tr( "No Data Loaded" ) );

            // Connect the tool buttons to an actual file dialog
            connect( m_fileLoadBtn, SIGNAL( clicked( bool ) ), this, SLOT( loadFile() ) );
        }

        FileWidget::FileWidget( SPtr< core::Reader > reader, const QString& title, const QIcon& icon, const QString& fileFilter, QWidget* parent ):
            FileWidget( title, icon, fileFilter, parent )
        {
            m_reader = reader;
        }

        FileWidget::~FileWidget()
        {
        }

        std::string FileWidget::getTitle() const
        {
            return m_title.toStdString();
        }

        di::core::State FileWidget::getState() const
        {
            LogD << "Storing file loader state \"" << getTitle() << "\"." << LogEnd;

            di::core::State state;
            state.set( "Filename", m_currentFile.toStdString() );
            return state;
        }

        bool FileWidget::setState( const di::core::State& state )
        {
            LogD << "Restoring file loader state \"" << getTitle() << "\"." << LogEnd;

            // Ignore empty states. But signal success since we did not fail setting anything.
            if( state.empty() )
            {
                LogD << "State empty. Ignoring." << LogEnd;
                return true;
            }

            // get filename and done
            auto fn = state.getValue( "Filename", "" );
            if( !fn.empty() )
            {
                m_currentFile = QString::fromStdString( fn );
                // trigger load
                postLoadCommand();
            }
            else
            {
                LogW << "No filename stored." << LogEnd;
            }

            return true;
        }

        void FileWidget::postLoadCommand()
        {
            // Use deferred loading:
            Application::getProcessingNetwork()->loadFile(
                m_reader, // NOTE: nullptr is allowed by loadFile.
                m_currentFile.toStdString(), SPtr< CommandObserverQt >( new CommandObserverQt( this, { m_fileLoadLabel, m_fileLoadLabel } ) ) );
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

            m_currentFile = selected;
            postLoadCommand();
        }

        bool FileWidget::event( QEvent* event )
        {
            // QT_COMMANDOBSERVER_EVENT?
            if( event->type() == QT_COMMANDOBSERVER_EVENT )
            {
                CommandObserverEvent* coe = dynamic_cast< CommandObserverEvent* >( event );
                LogD << "huhu" << this << " . " << coe->getObserverStatus() << LogEnd;

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
                    // NOTE: done using the dirty callback
                    // Application::getProcessingNetwork()->runNetwork();
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

        ConstSPtr< di::algorithms::DataInject > FileWidget::getDataInject() const
        {
            return m_dataInject;
        }

        bool FileWidget::canLoad( const std::string& filename ) const
        {
            if( m_reader )
            {
                return m_reader->canLoad( filename );
            }

            return false;
        }

        bool FileWidget::canLoad( const QString& filename ) const
        {
            return canLoad( filename.toStdString() );
        }

        void FileWidget::load( const QString& filename )
        {
            m_currentFile = filename;
            postLoadCommand();
        }

        void FileWidget::load( const std::string& filename )
        {
            load( QString::fromStdString( filename ) );
        }
    }
}


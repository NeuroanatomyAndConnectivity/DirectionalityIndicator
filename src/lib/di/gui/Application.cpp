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

#include <clocale>

#include <string>

#include <QApplication>
#include <QWidget>
#include <QLocale>
#include <QSurfaceFormat>

#include <di/core/ProcessingNetwork.h>
#include <di/core/Connection.h>
#include <di/core/Filesystem.h>
#include <di/core/ObserverCallback.h>

#include <di/gui/events/CallbackEvent.h>

#include "Application.h"

namespace di
{
    namespace gui
    {
        Application* Application::m_instance = nullptr;

        Application::Application( std::string name, int argc, char** argv ):
            m_name( name ),
            m_argc( argc ),
            m_argv( argv )
        {
            // Init the singleton ...
            m_instance = this;
        }

        Application::~Application()
        {
            // all the cleanup was done during run().
        }

        std::string Application::getName() const
        {
            return m_name;
        }

        bool Application::handleCommandLine( const std::vector< std::string >& /* arguments */, int argc, char** /* argv */ )
        {
            if( argc > 1 )
            {
                LogW << "Command line parameters ignored. Not supported by this application." << LogEnd;
            }
            return true;
        }

        int Application::run()
        {
            // Handle Parameters first
            std::vector< std::string > arguments;
            for( int i = 1; i < m_argc; ++i )
            {
                arguments.push_back( std::string( m_argv[ i ] ) );
            }
            auto result = handleCommandLine( arguments, m_argc, m_argv );
            if( !result )
            {
                return -1;
            }

            // NOTE: this is an important call to ensure 3.3 Core OpenGL on OSX. It has to be called before QApplication.
            QSurfaceFormat format33Core;
            //format33Core.setSamples( 32 );
            format33Core.setVersion( 3, 3 );
            format33Core.setProfile( QSurfaceFormat::CoreProfile );
            QSurfaceFormat::setDefaultFormat( format33Core );

            // Create QApplication
            QApplication application( m_argc, m_argv, true );

            // Force C locale to avoid issues with comma vs dot in different languages
            QLocale::setDefault( QLocale::c() );
            setlocale( LC_ALL, "C" );

            // We need this for shader loading and others.
            di::core::initRuntimePath( QCoreApplication::applicationDirPath().toStdString() );

            // Load settings
            m_settings = new QSettings( "SE", QString::fromStdString( m_name ) );

            // Create main window to use
            m_mainWindow = new MainWindow( QString::fromStdString( m_name ) );
            m_mainWindow->resize( 1024, 768 );

            // call users code:
            prepareUI();

            // start the processing container
            m_processingNetwork = SPtr< core::ProcessingNetwork >( new core::ProcessingNetwork() );
            m_processingNetwork->start();
            m_processingNetwork->observeOnDirty(
                std::make_shared< core::ObserverCallback >( [ this ]()
                    {
                        onDirtyNetwork();
                    }
                )
            );

            // Call network code
            prepareNetwork();

            // restore stored states/sizes
            m_mainWindow->loadStates();

            // Call show code
            show();

            // Finally, show the UI
            m_mainWindow->show();

            // run.
            int retVal = application.exec();

            // Stop if not yet done already.
            m_processingNetwork->stop();

            // Clean up and return.
            return retVal;
        }

        void Application::onShutdown()
        {
            close();
        }

        QSettings* Application::getSettings()
        {
            return Application::getInstance()->m_settings;
        }

        Application* Application::getInstance()
        {
            return m_instance;
        }

        di::SPtr< di::core::ProcessingNetwork > Application::getProcessingNetwork()
        {
            return Application::getInstance()->m_processingNetwork;
        }

        MainWindow* Application::getMainWindow()
        {
            return m_mainWindow;
        }

        void Application::loadProject( const QString& /* filename */ )
        {
        }

        void Application::saveProject( const QString& /* filename */, bool /* all */, bool /* viewOnly */, bool /* paramsOnly */,
                                       bool /* dataOnly */ )
        {
        }

        void Application::setUIEnabled( bool enable )
        {
            auto mw = getMainWindow();
            if( mw )
            {
                mw->setEnabled( enable );
            }
        }

        std::function< void() > Application::runInUIThread( std::function< void() > func )
        {
            auto ev = new CallbackEvent( func );
            return [ ev, this ](){ QCoreApplication::postEvent( getMainWindow(), ev ); };
        }
    }
}

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

#include <string>

#include <QApplication>
#include <QWidget>

#include <di/core/ProcessingNetwork.h>
#include <di/core/Connection.h>
#include <di/core/Filesystem.h>

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

        int Application::run()
        {
            // Initialize logger

            // Create QApplication
            QApplication application( m_argc, m_argv, true );

            // We need this for shader loading and others.
            di::core::initRuntimePath( QCoreApplication::applicationDirPath().toStdString() );

            // Load settings
            m_settings = new QSettings( "SE", QString::fromStdString( m_name ) );

            // Create main window to use
            m_mainWindow = new MainWindow( "DirectionalityIndicator" );
            m_mainWindow->resize( 1024, 768 );

            // call users code:
            prepareUI();

            // start the processing container
            m_processingNetwork = SPtr< core::ProcessingNetwork >( new core::ProcessingNetwork() );
            m_processingNetwork->start();

            // Call network code
            prepareNetwork();

            // Call show code
            show();

            // restore stored states/sizes
            m_mainWindow->loadStates();

            // Finally, show the UI
            m_mainWindow->show();

            // run.
            int retVal = application.exec();

            // Stop if not yet done already.
            m_processingNetwork->stop();

            // Clean up and return.
            return retVal;
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
    }
}

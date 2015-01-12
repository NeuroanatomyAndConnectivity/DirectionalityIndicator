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

#include <QApplication>

#include "OGLWidget.h"
#include "DataWidget.h"
#include "ParameterWidget.h"
#include "MainWindow.h"

#include "Application.h"

namespace di
{
    namespace gui
    {
        Application* Application::m_instance = nullptr;

        Application::Application( int argc, char** argv ):
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

        int Application::run()
        {
            // Initialize logger

            // Create QApplication
            QApplication application( m_argc, m_argv, true );

            // Load settings
            m_settings = new QSettings( "SE", "DirectionalityIndicator" );

            // Create the QMainWindow
            m_mainWindow = new MainWindow();
            m_mainWindow->resize( 1024, 768 );

            // Create the GL output:
            m_mainGLWidget = new OGLWidget( m_mainWindow );
            m_mainWindow->setCentralWidget( m_mainGLWidget );

            // Create the data widget:
            m_dataWidget = new DataWidget( m_mainWindow );
            m_mainWindow->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, m_dataWidget );

            // Create the parameter widget:
            m_parameterWidget = new ParameterWidget( m_mainWindow );
            m_mainWindow->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, m_parameterWidget );

            // restore stored states/sizes
            m_mainWindow->loadStates();

            // start the visualization container
            m_visualization = SPtr< core::Visualization >( new core::Visualization() );
            m_visualization->start();

            // Finally, show the UI
            m_mainWindow->show();

            // run.
            int retVal = application.exec();

            // Stop if not yet done already.
            m_visualization->stop();

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

        di::SPtr< di::core::Visualization > Application::getVisualization()
        {
            return Application::getInstance()->m_visualization;
        }
    }
}

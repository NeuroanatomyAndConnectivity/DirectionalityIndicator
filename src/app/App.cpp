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

#include <QDockWidget>
#include <QToolBox>
#include <QWidget>

#include "core/ProcessingNetwork.h"
#include "core/Connection.h"
#include "core/Filesystem.h"

#include "algorithms/SurfaceLIC.h"
#include "algorithms/RenderTriangles.h"
#include "algorithms/RenderLines.h"
#include "algorithms/ExtractRegions.h"

#include "gui/OGLWidget.h"
#include "gui/AlgorithmStrategies.h"
#include "gui/AlgorithmStrategy.h"
#include "gui/AlgorithmWidget.h"
#include "gui/DataWidget.h"
#include "gui/FileWidget.h"

#include "MainWindow.h"

// include some icons as XPM. This will be replaced by a proper file loading.
#include "icons/iconMesh.xpm"
#include "icons/iconLabels.xpm"

#include "App.h"

namespace di
{
    namespace app
    {
        App::App( int argc, char** argv ):
            di::gui::Application( "DirectionalityIndicator", argc, argv )
        {
        }

        App::~App()
        {
        }

        void App::show()
        {
            // restore stored states/sizes
            m_mainWindow->loadStates();

            // Finally, show the UI
            m_mainWindow->show();
        }

        void App::prepareUI()
        {
            // Create the QMainWindow
            m_mainWindow = new MainWindow();
            m_mainWindow->resize( 1024, 768 );

            // Create the GL output:
            m_mainGLWidget = new di::gui::OGLWidget( m_mainWindow );
            m_mainWindow->setCentralWidget( m_mainGLWidget );

            // Create the data widget:
            m_dataWidget = new di::gui::DataWidget( m_mainWindow );
            m_mainWindow->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, m_dataWidget );

            // The dock with all the parameters and stuff
            QDockWidget* tbDock = new QDockWidget( "Algorithm Parameters", m_mainWindow );
            m_algorithmStrategies = new di::gui::AlgorithmStrategies( tbDock );
            tbDock->setWidget( m_algorithmStrategies );
            tbDock->setObjectName( "AlgorithmParameters" );    // needed for persistent GUI states
            // avoid closable docks.
            tbDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
            m_mainWindow->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, tbDock );
        }

        void App::prepareNetwork()
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Hard-coded processing network ... ugly but working for now. The optimal solution would be a generic UI which provides this to the user
            // BEGIN:

            // Load mesh:
            m_dataWidget->addFileWidget( new di::gui::FileWidget( QIcon( QPixmap( iconMesh_xpm ) ),
                                                                  QString( "Stanford Poly Format (*.ply)" ) ) );

            // ALgorithm Temporaries ... needed to hard-code connections
            di::gui::AlgorithmWidget* algo1;
            di::gui::AlgorithmWidget* algo2;
            di::gui::AlgorithmStrategy* s;

            // Create the strategies:
            s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface with Region Boundaries" ) );
            s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::algorithms::RenderTriangles >( new di::algorithms::RenderTriangles ) ) );
            algo1 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::algorithms::ExtractRegions >( new di::algorithms::ExtractRegions ) ) );
            algo2 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::algorithms::RenderLines >( new di::algorithms::RenderLines ) ) );

            s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface LIC" ) );
            s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::algorithms::SurfaceLIC >( new di::algorithms::SurfaceLIC ) ) );

            // Tell the data widget that the processing network is ready.
            m_dataWidget->prepareProcessingNetwork();
            m_algorithmStrategies->prepareProcessingNetwork();

            // Connect
            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Regions", algo2->getAlgorithm(), "Lines" );

            // END:
            // Hard-coded processing network ... ugly but working for now. The optimal solution would be a generic UI which provides this to the user
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Hard-code a connection here. This should be done by a GUI or nice "use-case" class or something. For now, we need to get a VIS up and
            // running.
            m_dataWidget->connectDataToStrategies( m_algorithmStrategies );
        }
    }
}

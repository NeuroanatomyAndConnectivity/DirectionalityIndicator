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

#include <di/core/ProcessingNetwork.h>
#include <di/core/Connection.h>
#include <di/core/Filesystem.h>

#include <di/algorithms/SurfaceLIC.h>
#include <di/algorithms/RenderTriangles.h>
#include <di/algorithms/RenderLines.h>
#include <di/algorithms/RenderPoints.h>
#include <di/algorithms/ExtractRegions.h>

#include <di/gui/OGLWidget.h>
#include <di/gui/AlgorithmStrategies.h>
#include <di/gui/AlgorithmStrategy.h>
#include <di/gui/AlgorithmWidget.h>
#include <di/gui/DataWidget.h>
#include <di/gui/FileWidget.h>
#include <di/gui/MainWindow.h>

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
        }

        void App::prepareUI()
        {
            // Create the GL output:
            m_mainGLWidget = new di::gui::OGLWidget( getMainWindow() );
            getMainWindow()->setCentralWidget( m_mainGLWidget );

            // Create the data widget:
            m_dataWidget = new di::gui::DataWidget( getMainWindow() );
            getMainWindow()->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, m_dataWidget );

            // The dock with all the parameters and stuff
            QDockWidget* tbDock = new QDockWidget( "Algorithm Parameters", getMainWindow() );
            m_algorithmStrategies = new di::gui::AlgorithmStrategies( tbDock );
            tbDock->setWidget( m_algorithmStrategies );
            tbDock->setObjectName( "AlgorithmParameters" );    // needed for persistent GUI states
            // avoid closable docks.
            tbDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
            getMainWindow()->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, tbDock );
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
            di::gui::AlgorithmWidget* algo3;
            di::gui::AlgorithmStrategy* s;

            // Create the strategies:
            // Strategy 1:
            s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface with Region Boundaries" ) );
            s->addAlgorithm(         new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderTriangles ) ) );
            algo1 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::ExtractRegions ) ) );
            algo2 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderLines ) ) );
            algo3 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderPoints ) ) );

            // Strategy 2:
            s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface LIC" ) );
            s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::algorithms::SurfaceLIC >( new di::algorithms::SurfaceLIC ) ) );

            // Tell the data widget that the processing network is ready.
            m_dataWidget->prepareProcessingNetwork();
            m_algorithmStrategies->prepareProcessingNetwork();

            // Connect everything in strategy 1
            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Regions", algo2->getAlgorithm(), "Lines" );
            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Region Centers", algo3->getAlgorithm(), "Points" );

            // END:
            // Hard-coded processing network ... ugly but working for now. The optimal solution would be a generic UI which provides this to the user
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Hard-code a connection here. This should be done by a GUI or nice "use-case" class or something. For now, we need to get a VIS up and
            // running.
            m_dataWidget->connectDataToStrategies( m_algorithmStrategies );
        }
    }
}

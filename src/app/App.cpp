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
#include <di/algorithms/Voxelize.h>
#include <di/algorithms/Dilatate.h>
#include <di/algorithms/GaussSmooth.h>
#include <di/algorithms/RenderRegionMeshAndArrows.h>

#include <di/gui/ViewWidget.h>
#include <di/gui/AlgorithmStrategies.h>
#include <di/gui/AlgorithmStrategy.h>
#include <di/gui/AlgorithmWidget.h>
#include <di/gui/DataWidget.h>
#include <di/gui/FileWidget.h>
#include <di/gui/MainWindow.h>

// include some icons as XPM. This will be replaced by a proper file loading.
#include "icons/iconMesh.xpm"
// #include "icons/iconLabels.xpm"

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
            getMainWindow()->setCentralWidget( new di::gui::ViewWidget( "Visualization" ) );

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
            di::gui::AlgorithmWidget* algoT;
            di::gui::AlgorithmWidget* algo1;
            di::gui::AlgorithmWidget* algo2;
            di::gui::AlgorithmWidget* algo3;
            di::gui::AlgorithmWidget* algo4;
            di::gui::AlgorithmWidget* algo5;
            di::gui::AlgorithmWidget* algo6;
            di::gui::AlgorithmWidget* algo7;
            di::gui::AlgorithmWidget* algo8;
            di::gui::AlgorithmWidget* algo9;
            di::gui::AlgorithmWidget* algo10;
            di::gui::AlgorithmStrategy* s;

            // Create the strategies:
            // Strategy 1:
            s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface with Region Boundaries" ) );

            // Take the mesh data and extract the region information needed
            algo1 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::ExtractRegions ) ) );

            // Take the mesh data and voxelize
            algo6 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::Voxelize ) ) );
            // Take voxelized mesh -> "inflate" the data to build a proper hull around the mesh
            algo7 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::Dilatate ) ) );
            algo8 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::GaussSmooth ) ) );

            // Useful debug outputs -> render results
            algoT = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderTriangles ) ) );
            algo2 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderLines ) ) );
            algo3 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderPoints ) ) );
            algo4 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderLines ) ) );
            algo5 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderLines ) ) );
            algo10 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderLines ) ) );

            // Create arrow rendering:
            algo9 = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderRegionMeshAndArrows ) ) );

            // Strategy 2:
            s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface LIC" ) );
            s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::algorithms::SurfaceLIC >( new di::algorithms::SurfaceLIC ) ) );

            // Tell the data widget that the processing network is ready.
            m_dataWidget->prepareProcessingNetwork();
            m_algorithmStrategies->prepareProcessingNetwork();

            // Connect everything in strategy 1
            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Regions", algo2->getAlgorithm(), "Lines" );
            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Region Centers", algo3->getAlgorithm(), "Points" );
            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Connections", algo4->getAlgorithm(), "Lines" );
            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Region Meshes", algo5->getAlgorithm(), "Lines" );
            getProcessingNetwork()->connectAlgorithms( algo6->getAlgorithm(), "Voxel Mask", algo7->getAlgorithm(), "Input" );
            getProcessingNetwork()->connectAlgorithms( algo7->getAlgorithm(), "Dilatated", algo8->getAlgorithm(), "Input" );

            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Neighbour Arrows", algo10->getAlgorithm(), "Lines" );

            getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Region Information", algo9->getAlgorithm(), "Region Information" );

            // Connect all modules with a "Triangle Mesh" input.
            m_dataWidget->connectDataToStrategies( m_algorithmStrategies );

            // END:
            // Hard-coded processing network ... ugly but working for now. The optimal solution would be a generic UI which provides this to the user
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            //algo2->setActive( false );
            algo3->setActive( false );
            algo9->setActive( false );
            algo4->setActive( false );
            algo6->setActive( false );
            algo7->setActive( false );
           // algo5->setActive( false );
            // algoT->setActive( false );
        }

        void App::close()
        {
            LogD << "Shutdown. Bye!" << LogEnd;
        }
    }
}

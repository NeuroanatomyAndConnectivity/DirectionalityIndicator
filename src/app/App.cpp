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
#include <di/core/StringUtils.h>

#include <di/algorithms/SurfaceLIC.h>
#include <di/algorithms/RenderTriangles.h>
#include <di/algorithms/RenderLines.h>
#include <di/algorithms/RenderIllustrativeLines.h>
#include <di/algorithms/RenderPoints.h>
#include <di/algorithms/ExtractRegions.h>
#include <di/algorithms/Voxelize.h>
#include <di/algorithms/Dilatate.h>
#include <di/algorithms/GaussSmooth.h>

#include <di/io/RegionLabelReader.h>
#include <di/io/PlyReader.h>

#include <di/gui/ViewWidget.h>
#include <di/gui/AlgorithmStrategies.h>
#include <di/gui/AlgorithmStrategy.h>
#include <di/gui/AlgorithmWidget.h>
#include <di/gui/DataWidget.h>
#include <di/gui/FileWidget.h>
#include <di/gui/MainWindow.h>

#include <di/core/Logger.h>
#define LogTag "app/App"

// include some icons as XPM. This will be replaced by a proper file loading.
#include "icons/iconMesh.xpm"
#include "icons/iconLabels.xpm"
#include "icons/iconLabelOrder.xpm"

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

        bool App::handleCommandLine( const std::vector< std::string >& arguments, int /* argc */, char** /* argv */ )
        {
            // handle some special arguments first
            for( auto arg : arguments )
            {
                auto argument = di::core::toLower( arg );
                if( argument == "--screenshot" )
                {
                    m_screenShotMode = true;
                    LogD << "Commandline: screenshot mode activated." << LogEnd;
                }
                else if( argument.find( "--screenshot-path=" ) != std::string::npos )
                {
                    auto len = std::string( "--screenshot-path=" ).length();
                    m_screenShotPath = argument.substr( len, argument.length() - len );
                    LogD << "Commandline: screenshot-path set to \"" << m_screenShotPath << "\"." << LogEnd;
                }
                else
                {
                    // We assume all arguments to be filenames
                    // NOTE: use the original arg, no lower case argument.
                    m_deferLoad.push_back( arg );
                    LogD << "Commandline: file specified - \"" << arg << "\"." << LogEnd;
                }
            }
            return true;
        }

        void App::show()
        {
            // ensure visibility of the docks
            m_dataWidget->show();
            m_tbDock->show();
        }

        void App::onDirtyNetwork()
        {
            if( m_screenShotMode )
            {
                LogD << "Network marked dirty. Ignoring since screenshot mode is active." << LogEnd;
            }
            else
            {
                LogD << "Network marked dirty. Requesting update." << LogEnd;
                getProcessingNetwork()->runNetwork();
            }
        }

        void App::prepareUI()
        {
            // Create the GL output:
            m_viewWidget = new di::gui::ViewWidget( "Visualization" );
            getMainWindow()->setCentralWidget( m_viewWidget );

            // Use the di::gui::Application build a nice menu for us:
            getMainWindow()->addMenu();

            // We want a custom rotation as default
            m_viewWidget->setViewPreset( glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) *
                                         glm::rotate( glm::radians( 90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) );

            // Create the data widget:
            m_dataWidget = new di::gui::DataWidget( getMainWindow() );
            getMainWindow()->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, m_dataWidget );

            // The dock with all the parameters and stuff
            m_tbDock = new QDockWidget( "Algorithm Parameters", getMainWindow() );
            auto layout = new QVBoxLayout();
            layout->setAlignment( Qt::AlignTop );
            layout->setMargin( 0 );
            layout->setContentsMargins( 0, 0, 0, 0 );

            auto algoWidget = new QWidget();
            algoWidget->setLayout( layout );
            m_tbDock->setWidget( algoWidget );

            // Take the mesh data and extract the region information needed
            m_extractRegions = new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::ExtractRegions ) );

            // Handle different vis strategies here
            m_algorithmStrategies = new di::gui::AlgorithmStrategies( algoWidget );

            layout->addWidget( m_extractRegions );
            layout->addWidget( m_algorithmStrategies );

            m_tbDock->setObjectName( "AlgorithmParameters" );    // needed for persistent GUI states
            // avoid closable docks.
            m_tbDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
            getMainWindow()->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, m_tbDock );
        }

        void App::prepareNetwork()
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Hard-coded processing network ... ugly but working for now. The optimal solution would be a generic UI which provides this to the user
            // BEGIN:

            // Load mesh
            m_meshFile = new di::gui::FileWidget( std::make_shared< di::io::PlyReader >(),
                                                  "Mesh",
                                                  QIcon( QPixmap( iconMesh_xpm ) ),
                                                  QString( "Stanford Poly Format (*.ply)" ) );
            m_dataWidget->addFileWidget( m_meshFile );

            // Load Labels
            m_labelFile = new di::gui::FileWidget( std::make_shared< di::io::RegionLabelReader >(),
                                                   "Region Labels",
                                                   QIcon( QPixmap( iconLabels_xpm ) ),
                                                   QString( "Region Labels File (*.labels)" ) );
            m_dataWidget->addFileWidget( m_labelFile );

            // Load Label Order List
            m_labelOrderFile = new di::gui::FileWidget( std::make_shared< di::io::RegionLabelReader >(),
                                                        "Label Ordering",
                                                        QIcon( QPixmap( iconLabelOrder_xpm ) ),
                                                        QString( "Region Label Order File (*.labelorder)" ) );
            m_dataWidget->addFileWidget( m_labelOrderFile );

            // Create the strategies:
            // Strategy 1:
            auto s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface with Region Boundaries" ) );

            auto renderArrows = s->addAlgorithm(
                new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderIllustrativeLines ) )
            );

            // auto renderMeshAsLines = s->addAlgorithm(
            //    new di::gui::AlgorithmWidget( SPtr< di::core::Algorithm >( new di::algorithms::RenderLines ) )
            // );

            // Strategy 2:
            s = m_algorithmStrategies->addStrategy( new di::gui::AlgorithmStrategy( "Surface LIC" ) );
            auto lic = s->addAlgorithm( new di::gui::AlgorithmWidget( SPtr< di::algorithms::SurfaceLIC >( new di::algorithms::SurfaceLIC ) ) );

            // Tell the data widget that the processing network is ready.
            m_dataWidget->prepareProcessingNetwork();
            m_extractRegions->prepareProcessingNetwork();
            m_algorithmStrategies->prepareProcessingNetwork();

            // Connect everything in strategy 1
            // getProcessingNetwork()->connectAlgorithms( algo1->getAlgorithm(), "Neighbour Arrows", algo10->getAlgorithm(), "Lines" );

            // Connect all modules with a "Triangle Mesh" input.
            getProcessingNetwork()->connectAlgorithms( m_meshFile->getDataInject(), "Data",
                                                       m_extractRegions->getAlgorithm(), "Triangle Mesh" );
            getProcessingNetwork()->connectAlgorithms( m_labelFile->getDataInject(), "Data",
                                                       m_extractRegions->getAlgorithm(), "Triangle Labels" );
            getProcessingNetwork()->connectAlgorithms( m_labelOrderFile->getDataInject(), "Data",
                                                       m_extractRegions->getAlgorithm(), "Label Ordering" );

            getProcessingNetwork()->connectAlgorithms( m_labelFile->getDataInject(), "Data",
                                                       renderArrows->getAlgorithm(), "Labels" );

            getProcessingNetwork()->connectAlgorithms( m_meshFile->getDataInject(), "Data", renderArrows->getAlgorithm(), "Triangle Mesh" );
            getProcessingNetwork()->connectAlgorithms( m_meshFile->getDataInject(), "Data", lic->getAlgorithm(), "Triangle Mesh" );
            getProcessingNetwork()->connectAlgorithms( m_extractRegions->getAlgorithm(), "Directionality",
                                                       renderArrows->getAlgorithm(), "Directions" );
            // getProcessingNetwork()->connectAlgorithms( m_extractRegions->getAlgorithm(), "Region Mesh as Lines",
            //                                            renderMeshAsLines->getAlgorithm(), "Lines" );

            getProcessingNetwork()->connectAlgorithms( m_extractRegions->getAlgorithm(), "Directionality", lic->getAlgorithm(), "Directions" );

            // END:
            // Hard-coded processing network ... ugly but working for now. The optimal solution would be a generic UI which provides this to the user
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Process each filename. As some readers allow multiple file formats, it might light to ambiguous situations if we have multiple file
            // widgets that use a single reader on different formats. As we (50 lines ago) build our file widgets manually, we can manually define the
            // loader for each incoming file. We are forced to use extension.
            for( auto filename : m_deferLoad )
            {
                auto ext = di::core::toLower( di::core::getFileExtension( filename ) );
                if( ext == "project" )
                {
                    loadProject( QString::fromStdString( filename ) );
                    continue;
                }
                if( ext == "labelorder" )
                {
                    m_labelOrderFile->load( filename );
                    continue;
                }
                if( ext == "labels" )
                {
                    m_labelFile->load( filename );
                    continue;
                }
                if( ext == "ply" )
                {
                    m_meshFile->load( filename );
                    continue;
                }
            }

            // if screenshot mode is active, the network needs to be updated explicitly:
            if( m_screenShotMode )
            {
                // We want to close the app when done. To achieve this, we need to connect the ViewWidget::screenshotDone signal with the main
                // window's close slot. Problem: we cannot connect in Application derived classes since they are no QObjects. So we use a convenience
                // method to use std::function.
                //m_viewWidget->screenshotDone( std::bind( &di::gui::MainWindow::close, getMainWindow() ) );

                // Disable UI in this mode:
                LogD << "Disabling UI user input." << LogEnd;
                setUIEnabled( false );

                LogD << "Issuing update command." << LogEnd;
                getProcessingNetwork()->runNetwork();

                // Trigger the screenshot function when done. But use the runInUIThread adapter to ensure this is done in the UI thread.
                getProcessingNetwork()->callback( runInUIThread( std::bind( &di::gui::ViewWidget::screenshot, m_viewWidget, m_screenShotPath ) ) );
            }
        }

        void App::close()
        {
            LogD << "Shutdown. Bye!" << LogEnd;
        }

        void App::loadProject( const QString& filename )
        {
            di::core::State s = di::core::State::fromFile( filename.toStdString() );

            // Get States
            auto viewState = s.getState( "view1" );
            auto networkState = s.getState( "parameters" );
            auto fileState = s.getState( "files" );

            // Set them. These methods are very fault tolerant. They set what they can set.
            m_viewWidget->setState( viewState );
            getProcessingNetwork()->setState( networkState );
            m_dataWidget->setState( fileState );
        }

        void App::saveProject( const QString& filename, bool all, bool viewOnly, bool paramsOnly, bool dataOnly )
        {
            // Create a encapsulating state
            di::core::State main;
            main.set( "program", "DirectionalityIndicator" );
            main.set( "version", "1.0" );

            if( viewOnly || all )
            {
                // Collect everything. First: the view:
                auto viewState = m_viewWidget->getState();
                main.set( "view1", viewState );
            }

            if( paramsOnly || all )
            {
                // And network parameters
                auto networkState = getProcessingNetwork()->getState();
                main.set( "parameters", networkState );
            }

            if( dataOnly || all )
            {
                // And the files to load
                auto filesState = m_dataWidget->getState();
                main.set( "files", filesState );
            }

            // Done collecting data. Save ...
            main.toFile( filename.toStdString() );
        }
    }
}

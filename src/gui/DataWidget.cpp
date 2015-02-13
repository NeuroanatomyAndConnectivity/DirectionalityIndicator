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
#include "AlgorithmStrategies.h"
#include "ScaleLabel.h"

#include "core/Algorithm.h"
#define LogTag "gui/DataWidget"
#include "core/Logger.h"

// include some icons as XPM. This will be replaced by a proper file loading.
#include "icons/iconMesh.xpm"
#include "icons/iconLabels.xpm"

#include "CommandObserverQt.h"
#include "events/CommandObserverEvent.h"
#include "FileWidget.h"

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
            m_meshLoad = new FileWidget( QIcon( QPixmap( iconMesh_xpm ) ),
                                         QString( "Stanford Poly Format (*.ply)"
                                         /* Not yet implemented
                                                                   "Mesh File (*.gii *.asc *.ply);;
                                                                   GIfTI File (*.gii);; " +
                                                                   "ASCII Mesh File (*.asc);; " +
                                                                   "Stanford Poly Format (*.ply)" );
                                                                   */
                                         ),
                                         contentWidget );

            contentLayout->addWidget( m_meshLoad );
            contentLayout->setAlignment( Qt::AlignTop );
        }

        DataWidget::~DataWidget()
        {
        }

        void DataWidget::prepareProcessingNetwork()
        {
            // We use DataInject algorithms to inject data we have loaded (or will load). Let the FileWidgets do it:
            m_meshLoad->prepareProcessingNetwork();
        }

        void DataWidget::connectDataToStrategies( AlgorithmStrategies* to )
        {
            auto meshAlgo = m_meshLoad->getDataInject();
            to->connectToAll( meshAlgo, "Data", "Triangle Mesh" );
        }
    }
}


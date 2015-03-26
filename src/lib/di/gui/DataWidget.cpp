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
#include <di/gui/AlgorithmStrategies.h>
#include <di/gui/ScaleLabel.h>

#include <di/core/Algorithm.h>

#include <di/gui/CommandObserverQt.h>
#include <di/gui/events/CommandObserverEvent.h>
#include <di/gui/FileWidget.h>

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
            m_contentLayout = new QVBoxLayout;
            contentWidget->setLayout( m_contentLayout );
            m_contentLayout->setMargin( 0 );
            m_contentLayout->setContentsMargins( 0, 0, 0, 0 );
            m_contentLayout->setAlignment( Qt::AlignTop );
        }

        DataWidget::~DataWidget()
        {
        }

        void DataWidget::addFileWidget( FileWidget* widget )
        {
            m_loaders.push_back( widget );
            m_contentLayout->addWidget( widget );
        }

        void DataWidget::prepareProcessingNetwork()
        {
            // forward
            for( auto l : m_loaders )
            {
                l->prepareProcessingNetwork();
            }
        }

        void DataWidget::connectDataToStrategies( AlgorithmStrategies* to )
        {
            // forward
            for( auto l : m_loaders )
            {
                auto lAlgo = l->getDataInject();
                to->connectToAll( lAlgo, "Data", "Triangle Mesh" );
            }
        }
    }
}


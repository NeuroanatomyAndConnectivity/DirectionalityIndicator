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

#include "Application.h"

#include "algorithms/DirectionalityVisualization.h"

#include "ParameterWidget.h"

namespace di
{
    namespace gui
    {
        ParameterWidget::ParameterWidget( QWidget* parent ):
            QDockWidget( parent )
        {
            setWindowTitle( tr( "Algorithm Parameters" ) );
            setObjectName( "ParameterWidget" );    // needed for persistent GUI states

            // avoid closable docks.
            setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
        }

        ParameterWidget::~ParameterWidget()
        {
        }

        void ParameterWidget::prepareProcessingNetwork()
        {
            // Please keep in mind that this is a temporary solution. Algorithms should be handled by generic widgets.
            // To achieve our vis-goal now, we hard-code the right algorithm here.
            m_algorithm = SPtr< di::algorithms::DirectionalityVisualization >( new di::algorithms::DirectionalityVisualization() );
            Application::getProcessingNetwork()->addAlgorithm( m_algorithm );

        }
    }
}


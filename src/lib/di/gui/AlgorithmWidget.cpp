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

#include <di/core/Algorithm.h>
#include <di/core/Visualization.h>
#include <di/gui/Application.h>

#include "AlgorithmWidget.h"

namespace di
{
    namespace gui
    {
        AlgorithmWidget::AlgorithmWidget( SPtr< di::core::Algorithm > algorithm, QWidget* parent ):
            QWidget( parent ),
            m_algorithm( algorithm )
        {
            // Init
        }

        AlgorithmWidget::~AlgorithmWidget()
        {
        }

        void AlgorithmWidget::prepareProcessingNetwork()
        {
            Application::getProcessingNetwork()->addAlgorithm( m_algorithm );
        }

        ConstSPtr< di::core::Algorithm > AlgorithmWidget::getAlgorithm() const
        {
            return m_algorithm;
        }

        void AlgorithmWidget::setActive( bool active )
        {
            m_algorithm->setActive( active );

            SPtr< di::core::Visualization > vis = std::dynamic_pointer_cast< di::core::Visualization >( m_algorithm );
            if( vis )
            {
                vis->setRenderingActive( active );
            }
        }
    }
}


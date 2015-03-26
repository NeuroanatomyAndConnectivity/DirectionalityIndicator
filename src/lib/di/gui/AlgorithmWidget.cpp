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
#include <di/gui/ScaleLabel.h>

#include <di/core/ParameterBase.h>
#include <di/gui/ParameterWidget.h>

#include <di/core/Logger.h>
#define LogTag "gui/AlgorithmWidget"

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
            m_layout = new QGridLayout();
            m_layout->setAlignment( Qt::AlignTop );
            m_layout->setMargin( 0 );
            m_layout->setContentsMargins( 0, 0, 0, 0 );
            setLayout( m_layout );

            // get parameters
            auto params = algorithm->getParameters();
            size_t row = 0;
            for( auto param : params )
            {
                auto label = new ScaleLabel( QString::fromStdString( param->getName() ), this );
                label->setToolTip( QString::fromStdString( param->getDescription() ) );
                label->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
                m_layout->addWidget( label, row, 0 );

                auto paramWidget = ParameterWidget::build( param, this );
                m_layout->addWidget( paramWidget, row, 1 );

                row++;
            }
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


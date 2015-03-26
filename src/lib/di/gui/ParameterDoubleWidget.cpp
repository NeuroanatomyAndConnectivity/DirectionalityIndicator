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

#include <memory>

#include <QHBoxLayout>
#include <QDoubleValidator>

#include <di/core/Parameter.h>

#include <di/core/Logger.h>
#define LogTag "gui/ParameterDoubleWidget"

#include "ParameterDoubleWidget.h"

namespace di
{
    namespace gui
    {
        ParameterDoubleWidget::ParameterDoubleWidget( SPtr< core::ParameterBase > parameter, QWidget* parent ):
            ParameterWidget( parameter, parent )
        {
            // The capsule:
            auto widget = new QWidget( parent );
            auto layout = new QHBoxLayout( widget );
            layout->setMargin( 0 );
            layout->setContentsMargins( 0, 0, 0, 0 );
            widget->setLayout( layout );

            auto param = getParameter< core::ParamDouble >();
            if( param->hasRangeHint() )
            {
                m_slider = new QSlider( Qt::Horizontal, widget );
                m_slider->setRange( 0, 100 );
                layout->addWidget( m_slider );
            }

            m_edit = new QLineEdit( widget );
            if( param->hasRangeHint() )
            {
                m_edit->setValidator( new QDoubleValidator( param->getRangeHint().first, param->getRangeHint().second, 5 ) );
            }
            else
            {
                m_edit->setValidator( new QDoubleValidator() );
            }
            layout->addWidget( m_edit );

            update();

            setWidget( widget );

            connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( changedSlider() ) );
            connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( changedEdit() ) );
        }

        void ParameterDoubleWidget::update()
        {
            auto param = getParameter< core::ParamDouble >();
            if( m_slider )
            {
                auto val = static_cast< double >( param->get() );
                auto width = param->getRangeHint().second - param->getRangeHint().first;
                auto min = param->getRangeHint().first;

                m_slider->setValue( 100.0 * ( val - min ) / width );
            }
            m_edit->setText( QString::fromStdString( std::to_string( param->get() ) ) );
        }

        void ParameterDoubleWidget::changedSlider()
        {
            auto param = getParameter< core::ParamDouble >();
            auto val = static_cast< double >( m_slider->value() ) / 100.0;
            auto width = param->getRangeHint().second - param->getRangeHint().first;
            auto min = param->getRangeHint().first;
            getParameter< core::ParamDouble >()->set( min + val * width );
        }

        void ParameterDoubleWidget::changedEdit()
        {
            getParameter< core::ParamDouble >()->set( m_edit->text().toDouble() );
        }
    }
}


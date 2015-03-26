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
#include <QIntValidator>

#include <di/core/Parameter.h>

#include <di/core/Logger.h>
#define LogTag "gui/ParameterIntWidget"

#include "ParameterIntWidget.h"

namespace di
{
    namespace gui
    {
        ParameterIntWidget::ParameterIntWidget( SPtr< core::ParameterBase > parameter, QWidget* parent ):
            ParameterWidget( parameter, parent )
        {
            // The capsule:
            auto widget = new QWidget( parent );
            auto layout = new QHBoxLayout( widget );
            layout->setMargin( 0 );
            layout->setContentsMargins( 0, 0, 0, 0 );
            widget->setLayout( layout );

            auto param = getParameter< core::ParamInt >();
            if( param->hasRangeHint() )
            {
                m_slider = new QSlider( Qt::Horizontal, widget );
                m_slider->setRange( param->getRangeHint().first, param->getRangeHint().second );
                layout->addWidget( m_slider );
            }

            m_edit = new QLineEdit( widget );
            if( param->hasRangeHint() )
            {
                m_edit->setValidator( new QIntValidator( param->getRangeHint().first, param->getRangeHint().second  ) );
            }
            else
            {
                m_edit->setValidator( new QIntValidator() );
            }
            layout->addWidget( m_edit );

            update();

            setWidget( widget );

            connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( changedSlider() ) );
            connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( changedEdit() ) );
        }

        void ParameterIntWidget::update()
        {
            auto param = getParameter< core::ParamInt >();
            if( m_slider )
            {
                m_slider->setValue( param->get() );
            }
            m_edit->setText( QString::fromStdString( std::to_string( param->get() ) ) );
        }

        void ParameterIntWidget::changedSlider()
        {
            getParameter< core::ParamInt >()->set( m_slider->value() );
        }

        void ParameterIntWidget::changedEdit()
        {
            getParameter< core::ParamInt >()->set( m_edit->text().toInt() );
        }
    }
}


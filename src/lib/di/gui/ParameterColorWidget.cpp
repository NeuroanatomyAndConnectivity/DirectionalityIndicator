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

#include <di/core/Parameter.h>

#include <di/core/Logger.h>
#define LogTag "gui/ParameterColorWidget"

#include "ParameterColorWidget.h"

namespace di
{
    namespace gui
    {
        ParameterColorWidget::ParameterColorWidget( SPtr< core::ParameterBase > parameter, QWidget* parent ):
            ParameterWidget( parameter, parent )
        {
            m_widget = new ColorPicker( QColor( Qt::black ), parent );
            m_widget->setTracking( true );
            setWidget( m_widget );
            update();

            connect( m_widget, SIGNAL( onColorChange( QColor ) ), this, SLOT( changed( QColor ) ) );
        }

        void ParameterColorWidget::update()
        {
            auto paramColor= getParameter< core::ParamColor >();
            auto col = paramColor->get();
            m_widget->setColor( QColor( static_cast< int >( 255.0f * col.r ),
                                        static_cast< int >( 255.0f * col.g ),
                                        static_cast< int >( 255.0f * col.b ),
                                        static_cast< int >( 255.0f * col.a ) ) );
        }

        void ParameterColorWidget::changed( QColor color )
        {
            auto newColor = di::Color( color.redF(), color.greenF(), color.blueF(), color.alphaF() );
            getParameter< core::ParamColor >()->set( newColor );
        }
    }
}


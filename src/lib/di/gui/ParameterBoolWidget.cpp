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
#define LogTag "gui/ParameterBoolWidget"

#include "ParameterBoolWidget.h"

namespace di
{
    namespace gui
    {
        ParameterBoolWidget::ParameterBoolWidget( SPtr< core::ParameterBase > parameter, QWidget* parent ):
            ParameterWidget( parameter, parent )
        {
            m_widget = new QCheckBox( parent );
            setWidget( m_widget );

            connect( m_widget, SIGNAL( stateChanged( int ) ), this, SLOT( changed() ) );
        }

        void ParameterBoolWidget::update()
        {
            auto paramBool = getParameter< bool >();
            m_widget->setChecked( paramBool->get() );
        }

        void ParameterBoolWidget::changed()
        {
            getParameter< bool >()->set( m_widget->isChecked() );
        }
    }
}


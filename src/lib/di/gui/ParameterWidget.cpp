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

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <di/core/Parameter.h>
#include <di/core/ParameterTypes.h>

#include <di/gui/Application.h>
#include <di/gui/ScaleLabel.h>

#include <di/gui/CommandObserverQt.h>
#include <di/gui/events/CommandObserverEvent.h>

#include <di/gui/ParameterBoolWidget.h>
#include <di/gui/ParameterIntWidget.h>
#include <di/gui/ParameterColorWidget.h>
#include <di/gui/ParameterDoubleWidget.h>
#include <di/gui/ParameterIntListWidget.h>

#include "ParameterWidget.h"

namespace di
{
    namespace gui
    {
        ParameterWidget::ParameterWidget( SPtr< core::ParameterBase > parameter , QWidget* parent ):
            QWidget( parent ),
            m_param( parameter ),
            m_observer( std::make_shared< ObserverQt >( this ) ),
            m_layout( new QVBoxLayout )
        {
            m_param->observe( m_observer );

            m_layout->setMargin( 0 );
            m_layout->setContentsMargins( 0, 0, 0, 0 );
            setLayout( m_layout );
        }

        ParameterWidget::~ParameterWidget()
        {
        }

        ParameterWidget* ParameterWidget::build( SPtr< core::ParameterBase > parameter, QWidget* parent )
        {
            auto paramBool = core::ParameterBase::as< core::ParamBool >( parameter );
            if( paramBool )
            {
                return new ParameterBoolWidget( parameter, parent );
            }
            auto paramInt = core::ParameterBase::as< core::ParamInt >( parameter );
            if( paramInt )
            {
                return new ParameterIntWidget( parameter, parent );
            }
            auto paramDouble = core::ParameterBase::as< core::ParamDouble >( parameter );
            if( paramDouble )
            {
                return new ParameterDoubleWidget( parameter, parent );
            }
            auto paramColor = core::ParameterBase::as< core::ParamColor >( parameter );
            if( paramColor )
            {
                return new ParameterColorWidget( parameter, parent );
            }
            auto paramIntList = core::ParameterBase::as< core::ParamIntList >( parameter );
            if( paramIntList )
            {
                return new ParameterIntListWidget( parameter, parent );
            }

            LogW << "Your parameter type is not yet implemented!" << LogEnd;

            return nullptr;
        }

        bool ParameterWidget::event( QEvent* event )
        {
            // QT_OBSERVER_EVENT?
            if( event->type() == QT_OBSERVER_EVENT )
            {
                // auto oe = dynamic_cast< ObserverEvent* >( event );
                update();
            }
            return QWidget::event( event );
        }

        void ParameterWidget::setWidget( QWidget* widget )
        {
            m_layout->addWidget( widget );
        }

        SPtr< core::ParameterBase > ParameterWidget::getParameter() const
        {
            return m_param;
        }
    }
}


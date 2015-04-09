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
#include <string>
#include <vector>
#include <algorithm>

#include <QHBoxLayout>
#include <QRegExpValidator>

#include <di/core/Parameter.h>
#include <di/core/StringUtils.h>

#include <di/core/Logger.h>
#define LogTag "gui/ParameterIntListWidget"

#include "ParameterIntListWidget.h"

namespace di
{
    namespace gui
    {
        ParameterIntListWidget::ParameterIntListWidget( SPtr< core::ParameterBase > parameter, QWidget* parent ):
            ParameterWidget( parameter, parent )
        {
            // The capsule:
            auto widget = new QWidget( parent );
            auto layout = new QHBoxLayout( widget );
            layout->setMargin( 0 );
            layout->setContentsMargins( 0, 0, 0, 0 );
            widget->setLayout( layout );

            auto param = getParameter< core::ParamIntList >();

            m_edit = new QLineEdit( widget );
            layout->addWidget( m_edit );

            QRegExp rx( "(^$)|(^(\\d+)(,\\s*\\d+)*(,\\s*)*$)" );
            m_edit->setValidator( new QRegExpValidator( rx ) );

            update();

            setWidget( widget );

            connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( changedEdit() ) );
        }

        void ParameterIntListWidget::update()
        {
            auto param = getParameter< core::ParamIntList >();
            std::string text;
            if( !param->get().empty() )
            {
                for( size_t i = 0; i < param->get().size() - 1; ++i )
                {
                    text += std::to_string( param->get().at( i ) ) + ", ";
                }

                text += std::to_string( param->get().back() ) ;
            }
            m_edit->setText( QString::fromStdString( text ) );
        }

        void ParameterIntListWidget::changedEdit()
        {
            std::string text = m_edit->text().toStdString();
            auto stringList = di::core::split( text, ',' );

            std::vector< int > result;
            for( auto itemAsString : stringList )
            {
                auto i = di::core::trim( itemAsString );
                if( !i.empty() )
                {
                    result.push_back( std::stoi( i ) );
                }
            }

            // Avoid multiple:
            std::sort( result.begin(), result.end() );
            result.erase( std::unique( result.begin(), result.end() ), result.end() );
            getParameter< core::ParamIntList >()->set( result );
        }
    }
}


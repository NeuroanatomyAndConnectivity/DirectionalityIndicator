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

#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

#include <di/gui/ScaleLabel.h>

#include "ColorPicker.h"

namespace di
{
    namespace gui
    {
        ColorPicker::ColorPicker( QColor defaultColor, QWidget* parent ):
            QWidget( parent ),
            m_color( defaultColor )
        {
            auto layout = new QHBoxLayout();
            layout->setContentsMargins( 0, 0, 0, 0 );
            setLayout( layout );

            // Use a standard toolbutton to choose and "draw" the color
            m_colorBtn = new QToolButton( this );
            layout->addWidget( m_colorBtn );

            // And a label for printing RGBA values
            m_label = new ScaleLabel( this );
            m_label->setTextInteractionFlags( Qt::NoTextInteraction );
            layout->addWidget( m_label );

            connect( m_colorBtn, SIGNAL( released() ), this, SLOT( selectColor() ) );

            m_colorDialog = new QColorDialog( this );
            m_colorDialog->setOption( QColorDialog::ShowAlphaChannel );
            connect( m_colorDialog, SIGNAL( accepted() ), this, SLOT( accepted() ) );
            connect( m_colorDialog, SIGNAL( rejected() ), this, SLOT( rejected() ) );
            connect( m_colorDialog, SIGNAL( currentColorChanged( const QColor& ) ), this, SLOT( colorChanged( const QColor& ) ) );

            // Color everything
            update();
        }

        const QColor& ColorPicker::getColor() const
        {
            return m_color;
        }

        void ColorPicker::setColor( const QColor& color )
        {
            m_color = color;
            update();
        }

        void ColorPicker::accepted()
        {
            auto newColor = m_colorDialog->currentColor();
            if( newColor.isValid() )
            {
                m_color = newColor;
                update();
            }
        }

        void ColorPicker::rejected()
        {
            m_color = m_oldColor;
            update();
        }

        void ColorPicker::colorChanged( const QColor& color )
        {
            if( m_tracking )
            {
                m_color = color;
                update();
            }
        }

        void ColorPicker::selectColor()
        {
            m_colorDialog->show();
            m_oldColor = m_color;
        }

        void ColorPicker::update()
        {
            // Update the label
            std::string rgbaString = "RGBA: (" + std::to_string( m_color.red() );
            rgbaString += ", " + std::to_string( m_color.green() );
            rgbaString += ", " + std::to_string( m_color.blue() );
            rgbaString += ", " + std::to_string( m_color.alpha() );
            rgbaString += ")";

            m_label->setText( QString::fromStdString( rgbaString ) );

            // Update the toolbutton
            QPixmap px( 20, 20 );
            px.fill( m_color );
            m_colorBtn->setIcon( px );

            emit onColorChange( m_color );
        }

        void ColorPicker::setTracking( bool track )
        {
            m_tracking = track;
        }
    }
}


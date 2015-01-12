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

#include <cmath>

#include "ScaleLabel.h"

namespace di
{
    namespace gui
    {
        ScaleLabel::ScaleLabel( size_t length, QWidget* parent ):
            QLabel( parent ),
            m_additionalWidth( 0 ),
            m_minLength( length )
        {
            construct();
        }

        ScaleLabel::ScaleLabel( const QString &text, size_t length, QWidget* parent ) :
            QLabel( text, parent ),
            m_orgText( text ),
            m_additionalWidth( 0 ),
            m_minLength( length )
        {
            construct();
        }

        ScaleLabel::ScaleLabel( QWidget* parent ):
            QLabel( parent ),
            m_additionalWidth( 0 ),
            m_minLength( PREFERRED_LABEL_LENGTH )
        {
            construct();
        }

        ScaleLabel::ScaleLabel( const QString &text, QWidget* parent ):
            QLabel( text, parent ),
            m_orgText( text ),
            m_additionalWidth( 0 ),
            m_minLength( PREFERRED_LABEL_LENGTH )
        {
            construct();
        }

        void ScaleLabel::construct()
        {
            setMinimumWidth( fontMetrics().width( m_orgText.left( m_minLength ) + tr( "..." ) ) + m_additionalWidth );
            setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
            setMargin( 0 );
            setTextInteractionFlags( Qt::TextSelectableByMouse );
        }

        void ScaleLabel::resizeEvent( QResizeEvent* /*event*/ )
        {
            fitTextToSize();
        }

        QSize ScaleLabel::sizeHint() const
        {
            return QSize( calculateSize( m_orgText.length() ), QLabel::sizeHint().height() );
        }

        QSize ScaleLabel::minimumSizeHint() const
        {
            return QSize( calculateSize( m_minLength ), QLabel::minimumSizeHint().height() );
        }

        size_t ScaleLabel::calculateSize( size_t chars ) const
        {
            return fontMetrics().width( m_orgText.left( chars ) + tr( "..." ) ) + 2 * margin() + m_additionalWidth;
        }

        void ScaleLabel::setText( const QString &text )
        {
            m_orgText = text;
            setMinimumWidth( fontMetrics().width( m_orgText.left( m_minLength ) + tr( "..." ) ) + 2 * margin() + m_additionalWidth );
            fitTextToSize();
        }

        void ScaleLabel::fitTextToSize()
        {
            QString useText = fontMetrics().elidedText( m_orgText, Qt::ElideRight, width() );
            QLabel::setText( useText );
        }

        void ScaleLabel::addAdditionalWidth( int w )
        {
            m_additionalWidth = w;
        }

        void ScaleLabel::setMinimalLength( size_t chars )
        {
            setText( m_orgText );
            m_minLength = chars;
        }

        size_t ScaleLabel::getMinimalLength() const
        {
            return m_minLength;
        }
    }
}


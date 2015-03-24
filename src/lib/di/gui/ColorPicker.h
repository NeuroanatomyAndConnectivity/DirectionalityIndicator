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

#ifndef DI_COLORPICKER_H
#define DI_COLORPICKER_H

#include <QWidget>
#include <QToolButton>

namespace di
{
    namespace gui
    {
        class ScaleLabel;

        /**
         * A widget to provide color-chooser functionality.
         */
        class ColorPicker: public QWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the widget.
             *
             * \param parent the parent widget.
             */
            explicit ColorPicker( QColor defaultColor, QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~ColorPicker() = default;

            /**
             * Get the current color.
             *
             * \return the color.
             */
            const QColor& getColor() const;

            /**
             * Set the color.
             *
             * \param color the color
             */
            void setColor( const QColor& color );
        private slots:
            /**
             * Update the color information everywhere.
             */
            virtual void update();

            /**
             * Choose the color
             */
            virtual void selectColor();

        private:
            /**
             * The color button itself
             */
            QToolButton* m_colorBtn = nullptr;

            /**
             * The RGBA values
             */
            ScaleLabel* m_label = nullptr;

            /**
             * Color
             */
            QColor m_color;
        };
    }
}

#endif  // DI_COLORPICKER_H


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

#ifndef DI_PARAMETERINTWIDGET_H
#define DI_PARAMETERINTWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>

#include <di/gui/ParameterWidget.h>

#include <di/Types.h>

namespace di
{
    namespace gui
    {
        /**
         * A simple widget to provide the parameter to the user - int specialization.
         */
        class ParameterIntWidget: public ParameterWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the parameter widget.
             *
             * \param parent the parent widget
             * \param parameter the parameter itself
             */
            ParameterIntWidget( SPtr< core::ParameterBase > parameter, QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~ParameterIntWidget() = default;

        protected:
            /**
             * Update the widget. The parameter has notified.
             */
            void update() override;

        protected slots:

            /**
             * Called when the user changed something.
             */
            virtual void changedEdit();

            /**
             * Called when the user changed something.
             */
            virtual void changedSlider();

        private:
            /**
             * The widget
             */
            QLineEdit* m_edit = nullptr;

            /**
             * The slider
             */
            QSlider* m_slider = nullptr;
        };
    }
}

#endif  // DI_PARAMETERINTWIDGET_H


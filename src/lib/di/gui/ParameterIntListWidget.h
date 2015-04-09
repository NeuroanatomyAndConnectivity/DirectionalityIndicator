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

#ifndef DI_PARAMETERINTLISTWIDGET_H
#define DI_PARAMETERINTLISTWIDGET_H

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
         * A simple widget to provide the parameter to the user - int-list specialization.
         */
        class ParameterIntListWidget: public ParameterWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the parameter widget.
             *
             * \param parent the parent widget
             * \param parameter the parameter itself
             */
            ParameterIntListWidget( SPtr< core::ParameterBase > parameter, QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~ParameterIntListWidget() = default;

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

        private:
            /**
             * The widget
             */
            QLineEdit* m_edit = nullptr;
        };
    }
}

#endif  // DI_PARAMETERINTLISTWIDGET_H


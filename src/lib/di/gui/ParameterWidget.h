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

#ifndef DI_PARAMETERWIDGET_H
#define DI_PARAMETERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include <di/core/Parameter.h>

#include <di/gui/ObserverQt.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        class ParameterBase;
    }

    namespace gui
    {
        /**
         * A simple widget to provide the parameter to the user.
         */
        class ParameterWidget: public QWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the parameter widget.
             *
             * \param parent the parent widget
             * \param parameter the parameter itself
             */
            ParameterWidget( SPtr< core::ParameterBase > parameter , QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~ParameterWidget();

            /**
             * Build a parameter widget.
             *
             * \param parameter the parameter
             * \param parent the parent widget. Can be nullptr.
             *
             * \return the widget
             */
            static ParameterWidget* build( SPtr< core::ParameterBase > parameter, QWidget* parent = nullptr );

            /**
             * Event handler. We use it to handle \ref ObserverQt updates.
             *
             * \param event the event to handle
             *
             * \return true on success.
             */
            virtual bool event( QEvent* event );

            /**
             * Get the parameter itself
             *
             * \return the parameter
             */
            SPtr< core::ParameterBase > getParameter() const;

            /**
             * Get the parameter itself
             *
             * \return the parameter
             */
            template< typename ValueType >
            SPtr< core::Parameter< ValueType > > getParameter() const
            {
                return std::dynamic_pointer_cast< core::Parameter< ValueType > >( m_param );
            }

        protected:
            /**
             * Update the widget. The parameter has notified.
             */
            virtual void update() = 0;

            /**
             * Call to comfortably add a widget.
             *
             * \param widget the widget.
             */
            void setWidget( QWidget* widget );
        private:
            /**
             * The parameter.
             */
             SPtr< core::ParameterBase > m_param;

             /**
              * Observes the parameter.
              */
             SPtr< ObserverQt > m_observer;

             /**
              * Widget layout
              */
             QVBoxLayout* m_layout;
        };
    }
}

#endif  // DI_PARAMETERWIDGET_H


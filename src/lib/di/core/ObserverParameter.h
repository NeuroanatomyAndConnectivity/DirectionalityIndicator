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

#ifndef DI_OBSERVERPARAMETER_H
#define DI_OBSERVERPARAMETER_H

#include <functional>

#include <di/core/ObserverCallback.h>
#include <di/Types.h>

namespace di
{
    namespace core
    {
        class ParameterBase;

        /**
         * Implements a class to monitor changes. It implements the observer pattern and calls a specified function. This specific version forwards
         * a given parameter to the callback.
         */
        class ObserverParameter: public Observer
        {
        public:
            /**
             * Constructor. Does nothing.
             *
             * \param callback the callback that will be called on notify.
             * \param parameter the parameter to forward to the callback. If the weak_ptr cannot be locked, no callback is called.
             */
            ObserverParameter( std::function< void( SPtr< ParameterBase > ) > callback, WPtr< ParameterBase > parameter );

            /**
             * Destructor. Does nothing.
             */
            virtual ~ObserverParameter() = default;

            /**
             * Notification. The meaning depends on the caller.
             */
            void notify() override;

        protected:
        private:
            /**
             * Callback function
             */
            std::function< void( SPtr< ParameterBase > ) > m_callback;

            /**
             * The parameter
             */
            WPtr< ParameterBase > m_parameter;
        };
    }
}

#endif  // DI_OBSERVERPARAMETER_H


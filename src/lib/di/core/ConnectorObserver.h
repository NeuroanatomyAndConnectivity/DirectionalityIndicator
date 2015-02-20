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

#ifndef DI_CONNECTOROBSERVER_H
#define DI_CONNECTOROBSERVER_H

#include "Observer.h"

#include <di/Types.h>

namespace di
{
    namespace core
    {
        class Command;

        /**
         * Implements a class to monitor changes in a \ref ConnectorBase. It uses the observer pattern to inform anyone interested in changes.
         * This class is useful for implementing GUI feedback. Its main-purpose though, is the propagation of changes to the ProcessNetwork's
         * scheduler. Please also refer to the other observers for more details (\ref Observer or \ref CommandObserver).
         *
         */
        class ConnectorObserver: public Observer
        {
        public:
            /**
             * Method is called inside the Connectors whenever a new dataset was set.
             */
            virtual void dataChanged() = 0;
        protected:
            /**
             * Constructor. Does nothing.
             */
            ConnectorObserver();

            /**
             * Destructor. Does nothing.
             */
            virtual ~ConnectorObserver();

        private:
        };
    }
}

#endif  // DI_CONNECTOROBSERVER_H


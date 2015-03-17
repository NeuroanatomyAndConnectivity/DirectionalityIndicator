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

#ifndef DI_OBSERVABLE_H
#define DI_OBSERVABLE_H

#include <vector>
#include <mutex>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        class Observer;

        /**
         * Implements a base class to handle observers conveniently. Allows deriving classes to notify anyone interested about changes.
         */
        class Observable
        {
        public:
            /**
             * Observe changes on this parameter.
             *
             * \param observer the observer
             */
            void observe( SPtr< Observer > observer );

            /**
             * Remove the observer from the list
             *
             * \param observer the observer to remove
             */
            void removeObserver( SPtr< Observer > observer );
        protected:
            /**
             * Constructor. Does nothing.
             */
            Observable() = default;

            /**
             * Destructor. Does nothing.
             */
            virtual ~Observable() = default;

            /**
             * Notify all observers.
             */
            virtual void notify();
        private:
            /**
             * Securing the observer list.
             */
            std::mutex m_observersMutex;

            /**
             * The observers
             */
            std::vector< SPtr< Observer > > m_observers;
        };
    }
}

#endif  // DI_OBSERVABLE_H


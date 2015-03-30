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
#include <string>

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
             * Observe changes on this object.
             *
             * \param observer the observer
             */
            void observe( SPtr< Observer > observer );

            /**
             * Observe changes on this object. Note that you have to take care of removal when deleting the observer
             * yourself as this is not shared pointer.
             *
             * \param observer the observer
             */
            void observe( Observer* observer );

            /**
             * Remove the observer from the list
             *
             * \param observer the observer to remove
             */
            void removeObserver( SPtr< Observer > observer );

            /**
             * Remove the observer from the list
             *
             * \param observer the observer to remove
             */
            void removeObserver( Observer* observer );

            /**
             * Provide some instance information on this observable. The purpose is mostly for logging and debug output.
             *
             * \return the instance info string.
             */
            virtual std::string getInstanceInfo() const;

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

            /**
             * The observers as plain pointers.
             */
            std::vector< Observer* > m_observersPtr;
        };
    }
}

#endif  // DI_OBSERVABLE_H


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

#ifndef OBSERVER_H
#define OBSERVER_H

namespace di
{
    namespace core
    {
        /**
         * Implements a class to monitor changes. It implements the observer pattern and has no further functionality. Derive from this class to add a
         * specific meaning.
         */
        class Observer
        {
        public:
            /**
             * Notification. The meaning depends on the caller.
             */
            virtual void notify() = 0;

        protected:
            /**
             * Constructor. Does nothing.
             */
            Observer();

            /**
             * Destructor. Does nothing.
             */
            virtual ~Observer();

        private:
        };
    }
}

#endif  // OBSERVER_H


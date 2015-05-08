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

#ifndef DI_CALLBACK_H
#define DI_CALLBACK_H

#include <string>

#include <di/core/CommandObserver.h>
#include <di/core/Command.h>

#include <di/Types.h>

namespace di
{
    namespace commands
    {
        /**
         * Implements a command to get informed when run. It only triggers the observer and finishes. Very useful to synchronize with the network.
         */
        class Callback: public di::core::Command
        {
        public:
            /**
             * Create a command to trigger the observer.
             *
             * \param observer an object that gets notified upon changes in this command's state
             */
            explicit Callback( SPtr< di::core::CommandObserver > observer = nullptr );

            /**
             * Create a command to trigger the observer.
             *
             * \param callback a function that gets notified upon command's execution.
             */
            explicit Callback( std::function< void() > callback );

            /**
             * Clean up.
             */
            virtual ~Callback();

            /**
             * Get the human-readable title of this command. This should be something like "Adding Algorithm".
             *
             * \return the title
             */
            virtual std::string getName() const;

            /**
             * Get the human-readable description of this command. This is a more detailed description of what is going on, like "Adding a algorithm
             * to the network without connecting them".
             *
             * \return the description
             */
            virtual std::string getDescription() const;

            /**
             * Call the callback if defined.
             */
            void call();

        protected:
        private:
            /**
             * Callback.
             */
            std::function< void() > m_function;

            /**
             * If true, a function was set.
             */
            bool m_functionDefined = false;
        };
    }
}

#endif  // DI_CALLBACK_H


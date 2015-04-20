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

#ifndef DI_QUERYSTATE_H
#define DI_QUERYSTATE_H

#include <string>

#include <di/core/CommandObserver.h>
#include <di/core/Command.h>
#include <di/core/State.h>

#include <di/Types.h>

namespace di
{
    namespace commands
    {
        /**
         * Implements a command to query the current network state.
         */
        class QueryState: public di::core::Command
        {
        public:
            /**
             * Create a command to re-run the whole network.
             *
             * \param observer an object that gets notified upon changes in this command's state
             */
            explicit QueryState( SPtr< di::core::CommandObserver > observer = nullptr );

            /**
             * Clean up.
             */
            virtual ~QueryState();

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
             * Get the query result
             *
             * \return the state
             */
            const di::core::State& getState() const;

            /**
             * Set the result state
             *
             * \param state the state
             */
            void setState( const di::core::State& state );

        protected:
        private:
            /**
             * The result state
             */
            di::core::State m_state;
        };
    }
}

#endif  // DI_QUERYSTATE_H


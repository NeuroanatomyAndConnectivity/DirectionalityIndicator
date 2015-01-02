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
// along with DirectionalityIndicator. If not, see <http:#www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------------------

#ifndef COMMAND_H
#define COMMAND_H

#include <exception>

namespace di
{
    namespace core
    {
        /**
         * Implements a command. Derive to implement specific commands.
        */
        class Command
        {
        public:
            /**
             * Create an empty command. Derive to add a meaning.
             */
            Command();

            /**
             * Clean up.
             */
            virtual ~Command();

            /**
             * Called after finishing the command.
             */
            virtual void processed();

            /**
             * Called of the command was aborted.
             */
            virtual void abort();

            /**
             * Called if an exception occured.
             */
            virtual void exception();

            /**
             * Called if an exception occurred.
             *
             * \param e the exception
             */
            virtual void exception( const std::exception& e );

        protected:
        private:
        };
    }
}

#endif  // COMMAND_H


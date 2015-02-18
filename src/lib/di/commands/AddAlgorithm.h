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

#ifndef DI_ADDALGORITHM_H
#define DI_ADDALGORITHM_H

#include <string>

#include <di/core/CommandObserver.h>
#include <di/core/Command.h>
#include <di/core/Algorithm.h>

#include <di/Types.h>

namespace di
{
    namespace commands
    {
        /**
         * Implements a command to add algorithms to the a \ref di::core::ProcessingNetwork.
         */
        class AddAlgorithm: public di::core::Command
        {
        public:
            /**
             * Create an loader command to load the specified file.
             *
             * \param observer an object that gets notified upon changes in this command's state
             * \param algorithm the algorithm to add
             */
            AddAlgorithm( SPtr< di::core::Algorithm > algorithm, SPtr< di::core::CommandObserver > observer = nullptr );

            /**
             * Clean up.
             */
            virtual ~AddAlgorithm();

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
             * Get the algorithm associated with this command.
             *
             * \return the algorithm
             */
            virtual SPtr< di::core::Algorithm > getAlgorithm() const;

        protected:
        private:
            /**
             * The algorithm to add.
             */
            SPtr< di::core::Algorithm > m_algorithm;
        };
    }
}

#endif  // DI_ADDALGORITHM_H


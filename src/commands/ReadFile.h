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

#ifndef READFILECOMMAND_H
#define READFILECOMMAND_H

#include <exception>
#include <string>

#include "core/CommandObserver.h"
#include "core/Command.h"

#include "Types.h"

namespace di
{
    namespace commands
    {
        /**
         * Implements a command to read files from disk.
         */
        class ReadFile: public di::core::Command
        {
        public:
            /**
             * Create an loader command to load the specified file.
             *
             * \param observer an object that gets notified upon changes in this command's state.
             * \param filename the file to load
             */
            ReadFile( const std::string& filename,  SPtr< di::core::CommandObserver > observer = nullptr );

            /**
             * Clean up.
             */
            virtual ~ReadFile();

            /**
             * Get the human-readable title of this command. This should be something like "Calculating Gradients".
             *
             * \return the title
             */
            virtual std::string getTitle() const;

            /**
             * Get the human-readable description of this command. This is a more detailed description of what is going on, like "Calculating the
             * gradients using the differential quotient on scalar data.".
             *
             * \return the description
             */
            virtual std::string getDescription() const;

            /**
             * Get the filename specified.
             *
             * \return the filename
             */
            std::string getFilename() const;

        protected:
        private:
            /**
             * The filename.
             */
            std::string m_filename;
        };
    }
}

#endif  // READFILECOMMAND_H


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

#ifndef DI_READFILE_H
#define DI_READFILE_H

#include <exception>
#include <string>

#include <di/core/CommandObserver.h>
#include <di/core/Command.h>
#include <di/core/data/DataSetBase.h>

#include <di/algorithms/DataInject.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        class Reader;
    }

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
             * Create an loader command to load the specified file.
             *
             * \param reader the reader to use. Can be nullptr.
             * \param observer an object that gets notified upon changes in this command's state.
             * \param filename the file to load
             * \param inject the injector to use to insert the data into the network.
             */
            ReadFile( SPtr< core::Reader > reader, const std::string& filename, SPtr< di::core::CommandObserver > observer = nullptr,
                      SPtr< di::algorithms::DataInject > inject = nullptr );

            /**
             * Clean up.
             */
            virtual ~ReadFile();

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
             * Get the filename specified.
             *
             * \return the filename
             */
            std::string getFilename() const;

            /**
             * Get the result if any. This is useful as readers do not automatically add results to a visualization network.
             *
             * \return the result.
             */
            SPtr< di::core::DataSetBase > getResult() const;

            /**
             * Set the result if any. Always set before command processing finished. This ensures that the result is visible on success-notification.
             *
             * \param result the result
             */
            void setResult( SPtr< di::core::DataSetBase > result );

            /**
             * Get the recommended reader for this command. This might return NULL and means that the processing queue needs to find a reader.
             *
             * \return the reader instance or nullptr.
             */
            SPtr< core::Reader > getReader() const;

            /**
             * Return the injector specified during construction. Might be nullptr.
             *
             * \return the injector.
             */
            SPtr< di::algorithms::DataInject > getDataInject() const;
        protected:
        private:
            /**
             * The filename.
             */
            std::string m_filename = "";

            /**
             * The read result.
             */
            SPtr< di::core::DataSetBase > m_result = nullptr;

            /**
             * The reader to use. Can be null.
             */
            SPtr< core::Reader > m_reader = nullptr;

            /**
             * The injector to use. Can be null.
             */
            SPtr< di::algorithms::DataInject > m_inject = nullptr;
        };
    }
}

#endif  // DI_READFILE_H


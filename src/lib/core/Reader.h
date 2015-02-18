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

#ifndef READER_H
#define READER_H

#include <string>

#include "Types.h"

#include "data/DataSetBase.h"

namespace di
{
    namespace core
    {
        /**
         * Interface to all file loader implementations. There will be an instance for each specific implementation and each instance is const. This
         * means, that all load operations do not have side-effects and that the reader is not the owner of the data once it returned from the load
         * function.
         */
        class Reader
        {
        public:
            /**
             * Check whether the specified file can be loaded.
             *
             * \param filename the file to load
             *
             * \return true if this implementation is able to load the data.
             */
            virtual bool canLoad( const std::string& filename ) const = 0;

            /**
             * Load the specified file. This throws an exception if something went wrong.
             *
             * \param filename the file to load
             *
             * \return the data
             */
            virtual SPtr< DataSetBase > load( const std::string& filename ) const = 0;

        protected:
            /**
             * Constructor.
             */
            Reader();

            /**
             * Destructor.
             */
            virtual ~Reader();
        private:
        };
    }
}

#endif  // READER_H




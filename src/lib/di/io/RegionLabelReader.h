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

#ifndef DI_REGIONLABELREADER_H
#define DI_REGIONLABELREADER_H

#include <string>
#include <vector>

#include <di/Types.h>

#include <di/core/Reader.h>
#include <di/core/data/DataSetBase.h>
#include <di/core/data/DataSetCollection.h>

namespace di
{
    namespace io
    {
        /**
         * Implements a loader for the region label data format. It implements the \ref di::core::Reader interface.
         */
        class RegionLabelReader: public di::core::Reader
        {
        public:
            /**
             * The resulting data set type after loading.
             */
            typedef core::DataSetCollection< std::vector< uint32_t > > DataSetType;

            /**
             * Constructor;
             */
            RegionLabelReader();

            /**
             * Destructor.
             */
            virtual ~RegionLabelReader();

            /**
             * Check whether the specified file can be loaded.
             *
             * \param filename the file to load
             *
             * \return true if this implementation is able to load the data.
             */
            virtual bool canLoad( const std::string& filename ) const;

            /**
             * Load the specified file. This throws an exception if something went wrong.
             *
             * \param filename the file to load
             *
             * \return the data
             */
            virtual di::SPtr< di::core::DataSetBase > load( const std::string& filename ) const;
        protected:
        private:
        };
    }
}

#endif  // DI_REGIONLABELREADER_H


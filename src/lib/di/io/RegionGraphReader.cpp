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

#include <iostream>
#include <string>
#include <stdexcept>

#include <di/core/Filesystem.h>
#include <di/core/StringUtils.h>

#include "RegionGraphReader.h"

#include <di/core/Logger.h>
#define LogTag "di/io/RegionGraphReader"

namespace di
{
    namespace io
    {
        RegionGraphReader::RegionGraphReader():
            Reader()
        {
        }

        RegionGraphReader::~RegionGraphReader()
        {
        }

        bool RegionGraphReader::canLoad( const std::string& filename ) const
        {
            std::string ext = di::core::getFileExtension( filename );
            return ( di::core::toLower( ext ) == "regiongraph" );
        }

        di::SPtr< di::core::DataSetBase > RegionGraphReader::load( const std::string& filename ) const
        {
            LogD << "Loading \"" << filename << "\"." << LogEnd;
            auto file = core::readTextFile( filename );

            // each line
            auto lines = core::splitLines( file );
            for( auto line : lines )
            {
                // A line contains region-region relations

            }

            return nullptr;
        }
    }
}



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
#include <limits>
#include <algorithm>

#include <di/core/Filesystem.h>
#include <di/core/StringUtils.h>

#include "RegionLabelReader.h"

#include <di/core/Logger.h>
#define LogTag "di/io/RegionLabelReader"

namespace di
{
    namespace io
    {
        RegionLabelReader::RegionLabelReader():
            Reader()
        {
        }

        RegionLabelReader::~RegionLabelReader()
        {
        }

        bool RegionLabelReader::canLoad( const std::string& filename ) const
        {
            std::string ext = di::core::getFileExtension( filename );
            return ( di::core::toLower( ext ) == "labelorder" ) ||
                   ( di::core::toLower( ext ) == "labels" ) ||
                   ( di::core::toLower( ext ) == "csv" );
        }

        di::SPtr< di::core::DataSetBase > RegionLabelReader::load( const std::string& filename ) const
        {
            LogD << "Loading \"" << filename << "\"." << LogEnd;
            auto file = core::readTextFile( filename );

            auto labels = std::make_shared< std::vector< uint32_t > >();
            uint32_t min = std::numeric_limits< uint16_t >::max();
            uint32_t max = 0;

            // each line
            auto items = core::split( file, ',' );
            if( items.size() <= 1 )
            {
                items = core::split( file );
            }
            for( auto line : items )
            {
                uint32_t l = std::atoi( line.c_str() );
                // A line contains a region label - line number corresponds to the mesh vertex IDs
                labels->push_back( l );

                min = std::min( min, l );
                max = std::max( max, l );
            }
            LogD << "Read " << labels->size() << " labels from file. Labels range: [" << min << ", " << max << "]" << LogEnd;

            return std::make_shared< DataSetType >( "Mesh Labels", labels );
        }
    }
}



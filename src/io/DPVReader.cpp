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

#include <string>
#include <locale>
#include <algorithm>

#define LogTag "io/DPVReader"
#include "core/Logger.h"
#include "core/Filesystem.h"
#include "core/StringUtils.h"

#include "DPVReader.h"

namespace di
{
    namespace io
    {
        DPVReader::DPVReader():
            Reader()
        {
        }

        DPVReader::~DPVReader()
        {
        }

        bool DPVReader::canLoad( const std::string& filename ) const
        {
            std::string ext = di::core::getFileExtension( filename );
            return ( di::core::toLower( ext ) == "dpv" );
        }

        SPtr< di::core::DataSetBase > DPVReader::load( const std::string& filename ) const
        {
            LogD << "Loading \"" << filename << "\"." << LogEnd;
            return nullptr;
        }
    }
}


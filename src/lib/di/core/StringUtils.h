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

#ifndef DI_STRINGUTILS_H
#define DI_STRINGUTILS_H

#include <string>
#include <vector>

// This file implements some utils we all love from boost::filesystem

namespace di
{
    namespace core
    {
        /**
         * Convert string to lower case.
         *
         * \param str the string
         *
         * \return the lower case string.
         */
        std::string toLower( const std::string& str );

        /**
         * Split a string into lines. Very useful for parsing text files.
         *
         * \param theString the string to split
         *
         * \return the lines as vector.
         */
        std::vector< std::string > splitLines( const std::string& theString );
    }
}

#endif  // DI_STRINGUTILS_H


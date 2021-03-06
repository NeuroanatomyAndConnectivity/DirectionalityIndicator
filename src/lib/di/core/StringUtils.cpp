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

#include <string> // std::string
#include <algorithm> // std::transform
#include <cctype>
#include <sstream>

#include "StringUtils.h"

namespace di
{
    namespace core
    {
        std::string toLower( const std::string& str )
        {
            std::string result = str;
            std::transform( result.begin(), result.end(), result.begin(), tolower );
            return result;
        }

        std::vector< std::string > split( const std::string& theString, const char& delim )
        {
            // NOTE: this can be done nicely with std::sregex_token_iterator but crashes on GCC 4.9

            // So use a string stream
            std::stringstream ss( theString );
            std::string item;
            std::vector< std::string > elems;
            while( std::getline( ss, item, delim ) )
            {
                elems.push_back( item );
            }
            return elems;
        }

        std::string trim( const std::string& theString )
        {
            size_t first = theString.find_first_not_of( ' ' );
            size_t last = theString.find_last_not_of( ' ' );

            // If not found, the string is completely "space".
            if( ( first == std::string::npos ) || ( last == std::string::npos ) )
            {
                return "";
            }
            return theString.substr( first, ( last - first + 1 ) );
        }
    }
}

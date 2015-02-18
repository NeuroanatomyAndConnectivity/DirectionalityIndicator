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
#include <fstream>
#include <streambuf>
#include <stdexcept>

#include "Filesystem.h"

namespace di
{
    namespace core
    {
        std::string getFileExtension( const std::string& filename )
        {
            // NOTE: not very robust. Needs to be improved
            // TODO(Sebastian) improve and make robust
            return filename.substr( filename.find_last_of( "." ) + 1 );
        }

        std::string readTextFile( const std::string& filename )
        {
            std::ifstream t( filename );
            std::string str;

            if( !t.good() )
            {
                throw std::invalid_argument( "File \"" + filename + "\" could not be opened for reading." );
            }

            // We use this to reserve the required mem beforehand
            t.seekg( 0, std::ios::end );
            str.reserve( t.tellg() );
            t.seekg( 0, std::ios::beg );

            // Finally, read.
            str.assign( ( std::istreambuf_iterator< char >( t ) ),
                          std::istreambuf_iterator< char >() );

            return str;
        }

        static std::string g_runtimePath = "";

        std::string getRuntimePath()
        {
            return g_runtimePath;
        }

        std::string getResourcePath()
        {
            // NOTE: ResourceName needs to be set by the build system
            return getRuntimePath() + "../share/" + ResourceName + "/";
        }

        void initRuntimePath( const std::string& path )
        {
            if( ( path.back() == '/' ) || ( path.back() == '\\' ) )
            {
                g_runtimePath = path;
            }
            else
            {
                g_runtimePath = path + "/";
            }
        }
    }
}

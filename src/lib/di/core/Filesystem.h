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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>

// This file implements some utils we all love from boost::filesystem

namespace di
{
    namespace core
    {
        /**
         * Get the extension if a filename.
         *
         * \param filename the filename
         *
         * \return the extension. Can be empty.
         */
        std::string getFileExtension( const std::string& filename );

        /**
         * Read a whole text file in to a string.
         *
         * \param filename the filename
         *
         * \return the string
         *
         * \throw std::invalid_argument if the file could not be read.
         */
        std::string readTextFile( const std::string& filename );

        /**
         * The runtime path of the program. Guaranteed to end with a directory separator.
         *
         * \return the path.
         */
        std::string getRuntimePath();

        /**
         * The runtime path of the program. Guaranteed to end with a directory separator.
         *
         * \return the resource path
         */
        std::string getResourcePath();

        /**
         * Initialize runtime path. Call this as soon as possible.
         *
         * \param path the path to use as system path. Use absolute paths.
         */
        void initRuntimePath( const std::string& path );
    }
}

#endif  // FILESYSTEM_H


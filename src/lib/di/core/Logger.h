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

#ifndef DI_LOGGER_H
#define DI_LOGGER_H

#include <iostream>

// This file contains the preprocessor based logger. This is a rather simple logger, but as it uses a simple ostream interface, it can be replaced
// later without the need for changes in the code.

#define LogStream std::cout
#define LogEnd std::endl;
#define LogD LogStream << "DEBUG [" << LogTag << "]: "
#define LogI LogStream << "INFO  [" << LogTag << "]: "
#define LogW LogStream << "WARN  [" << LogTag << "]: "
#define LogE LogStream << "ERROR [" << LogTag << "]: "
#define LogGL LogStream

#endif  // DI_LOGGER_H

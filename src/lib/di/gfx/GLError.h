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

#ifndef GLERROR_H
#define GLERROR_H

#include <iostream>
#include <string>

#include <di/core/Logger.h>

#ifndef LogTag
#define LogTag "Unknown LogTag"
#endif

/**
 * Simply forward GL errors to a stream.
 *
 * \param to the stream to log to
 * \param tag as this log call uses the Logger functionality, we need the tag
 * \param file the filename of caller
 * \param line linenumber of caller
 */
void logGLErrorImpl( std::ostream& to, const std::string& tag, const char* file, int line );

#define logGLError() logGLErrorImpl( LogGL, LogTag, __FILE__, __LINE__ )

#endif  // GLERROR_H


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

#ifndef GFXTYPES_H
#define GFXTYPES_H

// NOTE: This file is mostly used to include some standard graphics types, like vector arrays and similar.

#include <vector>

// we need the math types
#include "MathTypes.h"

namespace di
{
    typedef std::vector< glm::vec2 > Vec2Array;
    typedef std::vector< glm::vec3 > Vec3Array;
    typedef std::vector< glm::vec4 > Vec4Array;

    typedef std::vector< glm::ivec2 > IndexVec2Array;
    typedef std::vector< glm::ivec3 > IndexVec3Array;
    typedef std::vector< glm::ivec4 > IndexVec4Array;
}

#endif  // GFXTYPES_H



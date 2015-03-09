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

#ifndef DI_DATABUILDER_H
#define DI_DATABUILDER_H

#include <di/core/BoundingBox.h>
#include <di/core/data/TriangleMesh.h>
#include <di/MathTypes.h>

#include <di/core/Logger.h>
#define LogTag "core/DataBuilder"

namespace di
{
    namespace core
    {
        /**
         * Build a solid cuboid by using the given BoundingBox. Note that this function creates the edge vertices 3 times to allow a normal for each
         * side-plane.
         *
         * \param bb the bounding box
         *
         * \return the resulting cuboid.
         */
        SPtr< TriangleMesh > buildCuboid( const BoundingBox& bb );
    }
}

#endif  // DI_DATABUILDER_H


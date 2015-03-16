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

#ifndef DI_DATASETTYPES_H
#define DI_DATASETTYPES_H

#include <vector>

#include <di/core/data/DataSet.h>
#include <di/core/data/GridRegular.h>
#include <di/core/data/GridTransformation.h>
#include <di/core/data/GridBuilders.h>

#include <di/core/data/LineDataSet.h>
#include <di/core/data/PointDataSet.h>
#include <di/core/data/TriangleDataSet.h>
#include <di/core/data/TriangleMesh.h>

// Now, define a bunch of default dataset types
namespace di
{
    namespace core
    {
        /**
         * Dataset in a 3D regular grid. The "d" in the name stands for "double".
         */
        typedef DataSet< GridRegular3, std::vector< double > > DataSetScalarRegular3d;

        /**
         * Dataset in a 3D regular grid. The "v3" in the name stands for "vector 3".
         */
        typedef DataSet< GridRegular3, std::vector< glm::vec3 > > DataSetScalarRegular3v3;

        /**
         * Dataset in a 3D regular grid as masks.
         */
        typedef DataSet< GridRegular3, std::vector< bool > > DataSetScalarRegular3b;

        /**
         * A vector field given on a triangle mesh
         */
        typedef di::core::DataSet< TriangleMesh, di::Vec3Array > TriangleVectorField;
    }
}

#endif  // DI_DATASETTYPES_H


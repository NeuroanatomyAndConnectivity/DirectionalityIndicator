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

#include <map>
#include <utility>

#include <di/core/data/TriangleDataSet.h>

#define LogTag "algorithms/Voxelize"
#include <di/core/Logger.h>

#include "Voxelize.h"

namespace di
{
    namespace algorithms
    {
        Voxelize::Voxelize():
            Algorithm( "Voxelize",
                       "Create a voxel-version of the input data." )
        {
            // 1: the output

            // 2: the input
            m_dataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle data to voxelize."
            );

            /*m_resoultion = addProperty< unsigned int >(
                    "Resolution",
                    "The number of voxels per direction used for sampling.",
                    128 );*/
        }

        Voxelize::~Voxelize()
        {
            // nothing to clean up so far
        }

        void Voxelize::process()
        {
            // Get input data
            auto triangleDataSet = m_dataInput->getData();
        }
    }
}


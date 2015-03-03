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

#include <fstream>
#include <map>
#include <vector>
#include <utility>

#include <di/core/data/TriangleDataSet.h>

#include "Voxelize.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/Voxelize"

namespace di
{
    namespace algorithms
    {
        Voxelize::Voxelize():
            Algorithm( "Voxelize",
                       "Create a voxel-version of the input data." )
        {
            // 1: the output
            m_dataOutput = addOutput< di::core::DataSetScalarRegular3d >(
                    "Voxel Mask",
                    "The triangle data as bunch of voxels."
            );

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

            // Create the grid with the desired resolution:
            auto grid = regularGridForBoundingBox( triangleDataSet->getGrid()->getBoundingBox(), m_resoultion, 10 );
            auto values = std::make_shared< std::vector< double > >( grid->getSize() );

            LogD << "Using grid: " << *grid << LogEnd;

            // Iterate each triangle
            for( auto tri : triangleDataSet->getGrid()->getTriangles() )
            {
                // Note: this is probably the worst way of rasterizing a triangle. Sufficient for now.
                auto v1 = triangleDataSet->getGrid()->getVertex( tri.x );
                auto v2 = triangleDataSet->getGrid()->getVertex( tri.y );
                auto v3 = triangleDataSet->getGrid()->getVertex( tri.z );

                ( *values )[ grid->voxelIndex( v1 ) ] = 1.0;
                ( *values )[ grid->voxelIndex( v2 ) ] = 1.0;
                ( *values )[ grid->voxelIndex( v3 ) ] = 1.0;

                // Raster in between .. the ugly way:
                auto v21m = v1 + 0.5f * ( v2 - v1 );
                auto v31m = v1 + 0.5f * ( v3 - v1 );
                auto v32m = v1 + 0.5f * ( v3 - v2 );

                ( *values )[ grid->voxelIndex( v21m ) ] = 1.0;
                ( *values )[ grid->voxelIndex( v31m ) ] = 1.0;
                ( *values )[ grid->voxelIndex( v32m ) ] = 1.0;
            }

            // Construct result dataset:
            m_dataOutput->setData( std::make_shared< di::core::DataSetScalarRegular3d >( "Voxels", grid, values ) );
        }
    }
}


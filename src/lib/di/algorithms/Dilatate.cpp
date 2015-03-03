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

#include "Dilatate.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/Dilatate"

namespace di
{
    namespace algorithms
    {
        Dilatate::Dilatate():
            Algorithm( "Dilatate",
                       "Apply a morphological dilatation to the input data." )
        {
            // 1: the output
            m_dataOutput = addOutput< di::core::DataSetScalarRegular3d >(
                    "Dilatated",
                    "The dilatated input data."
            );

            // 2: the input
            m_dataInput = addInput< di::core::DataSetScalarRegular3d >(
                    "Input",
                    "The data to process."
            );
        }

        Dilatate::~Dilatate()
        {
            // nothing to clean up so far
        }

        void Dilatate::process()
        {
            // Get input data
            auto inputData = m_dataInput->getData();
            auto inputValues = inputData->getAttributes<0>();

            auto grid = inputData->getGrid();
            auto values = std::make_shared< std::vector< double > >( grid->getSize() );

            // Iterate each voxel:
            for( size_t z = 1; z < grid->getSizeZ() - 1; ++z )
            {
                for( size_t y = 1; y < grid->getSizeY() - 1; ++y )
                {
                    for( size_t x = 1; x < grid->getSizeX() - 1; ++x )
                    {
                        // center
                        auto cIdx = grid->index( x, y, z );
                        // neighbour
                        bool neighbourFilled = false;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y, z ) ] != 0.0 ? true : neighbourFilled;

                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y - 1, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y + 1, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y, z + 1 ) ] != 0.0 ? true : neighbourFilled;

                        // Include vertex-neighbours?
                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y - 1, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y - 1, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y - 1, z + 1 ) ] != 0.0 ? true : neighbourFilled;

                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y, z + 1 ) ] != 0.0 ? true : neighbourFilled;

                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y + 1, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y + 1, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x - 1, y + 1, z + 1 ) ] != 0.0 ? true : neighbourFilled;


                        neighbourFilled = ( *inputValues )[ grid->index( x, y - 1, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y - 1, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y - 1, z + 1 ) ] != 0.0 ? true : neighbourFilled;

                        neighbourFilled = ( *inputValues )[ grid->index( x, y + 1, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x, y + 1, z + 1 ) ] != 0.0 ? true : neighbourFilled;


                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y - 1, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y - 1, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y - 1, z + 1 ) ] != 0.0 ? true : neighbourFilled;

                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y, z + 1 ) ] != 0.0 ? true : neighbourFilled;

                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y + 1, z ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y + 1, z - 1 ) ] != 0.0 ? true : neighbourFilled;
                        neighbourFilled = ( *inputValues )[ grid->index( x + 1, y + 1, z + 1 ) ] != 0.0 ? true : neighbourFilled;

                        ( *values )[ cIdx ] = neighbourFilled ? 1.0 : 0.0;
                    }
                }
            }

            // Construct result dataset:
            m_dataOutput->setData( std::make_shared< di::core::DataSetScalarRegular3d >( "Dilatetd", grid, values ) );
        }
    }
}


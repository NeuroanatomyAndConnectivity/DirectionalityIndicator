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
#include <utility>

#include "GaussSmooth.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/GaussSmooth"

namespace di
{
    namespace algorithms
    {
        GaussSmooth::GaussSmooth():
            Algorithm( "Gauss Smooth",
                       "Apply a Gaussian filter to the input data." )
        {
            // 1: the output
            m_dataOutput = addOutput< di::core::DataSetScalarRegular3d >(
                    "Gaussed",
                    "The Gaussed input data."
            );

            // 2: the input
            m_dataInput = addInput< di::core::DataSetScalarRegular3d >(
                    "Input",
                    "The data to process."
            );
        }

        GaussSmooth::~GaussSmooth()
        {
            // nothing to clean up so far
        }

        template< typename ValueT, typename GridT >
        void filterField1D( SPtr< ValueT > values, const ValueT& valuesIn, SPtr< GridT > grid,
                                                     size_t dx, size_t dy, size_t dz )
        {
            for( size_t z = 1; z < grid->getSizeZ() - 1; ++z )
            {
                for( size_t y = 1; y < grid->getSizeY() - 1; ++y )
                {
                    for( size_t x = 1; x < grid->getSizeX() - 1; ++x )
                    {
                        auto before = grid->index( x - dx, y - dy, z - dz );
                        auto after  = grid->index( x + dx, y + dy, z + dz );
                        auto center = grid->index( x, y, z );

                        ( *values )[ center ] = 0.25 * ( valuesIn[ before ] + 2.0 * valuesIn[ center ] + valuesIn[ after ] );
                    }
                }
            }
        }

        template< typename ValueT, typename GridT >
        SPtr< std::vector< double > > filterField( const ValueT& valuesIn, SPtr< GridT > grid )
        {
            // apply 1D gauss three times. Ping-Pong data:
            auto values1 = std::make_shared< std::vector< double > >( grid->getSize() );
            auto values2 = std::make_shared< std::vector< double > >( grid->getSize() );

            filterField1D( values1, valuesIn, grid, 1, 0, 0 ); // run in X direction
            filterField1D( values2, *values1, grid, 0, 1, 0 ); // run in Y direction
            filterField1D( values1, *values2, grid, 0, 0, 1 ); // run in Z direction

            return values1;
        }

        void GaussSmooth::process()
        {
            // Get input data
            auto inputData = m_dataInput->getData();
            auto inputValues = inputData->getAttributes<0>();

            auto grid = inputData->getGrid();

            // Iterate as often as requested
            size_t m_iterations = 10;
            auto src = inputValues;
            for( size_t i = 0; i < m_iterations; ++i )
            {
                LogD << "Gauss filter - iteration: " << i + 1 << LogEnd;
                src = filterField( *src, grid );
            }
            /*
            std::ofstream f( "/home/seth/smooth222t" + std::to_string( grid->getSizeX() ) + "_" +
                                                 std::to_string( grid->getSizeY() ) + "_" +
                                                 std::to_string( grid->getSizeZ() ) +
                    ".raw" , std::ios::out | std::ios::binary );
            if( f.good() )
            {
                f.write( reinterpret_cast<const char*>(&src->data()[0]), src->size() * sizeof( double ) );
            }

            f.close();
            */

            // Construct result dataset:
            m_dataOutput->setData( std::make_shared< di::core::DataSetScalarRegular3d >( "Gaussed", grid, src ) );
        }
    }
}


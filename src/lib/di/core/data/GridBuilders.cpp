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

#include "GridBuilders.h"

namespace di
{
    namespace core
    {
        SPtr< core::GridRegular3 > regularGridForBoundingBox( const BoundingBox& bb, size_t maxRes, size_t additional )
        {
            auto min = bb.getMin();
            auto max = bb.getMax();

            // find the largest dimension:
            auto diag = max - min;
            auto maxSize = std::max( diag.x, std::max( diag.y, diag.z ) );

            auto matrix = glm::translate( glm::dvec3( static_cast< glm::dvec3::value_type >( additional ) ) ) *
                          glm::scale(
                            glm::dvec3( static_cast< glm::dvec3::value_type >( ( maxRes - 1 ) / maxSize ) )
                          ) *
                          glm::translate( static_cast< glm::dvec3 >( -min ) );


            core::GridTransformation< 3 > transform( matrix );

            // Calc the optimal resolutions
            auto factor = static_cast< double >( maxRes ) / static_cast< double >( maxSize );
            return std::make_shared< core::GridRegular3 >( transform, ( 2 * additional ) + ( factor * diag.x ),
                                                                      ( 2 * additional ) + ( factor * diag.y ),
                                                                      ( 2 * additional ) + ( factor * diag.z ) );
        }
    }
}

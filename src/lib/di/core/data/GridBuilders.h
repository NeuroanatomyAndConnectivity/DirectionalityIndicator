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

#ifndef DI_GRIDBUILDERS_H
#define DI_GRIDBUILDERS_H

#include <di/core/BoundingBox.h>
#include <di/core/data/GridTransformation.h>
#include <di/core/data/GridRegular.h>

#include <di/MathTypes.h> // needed for storing physical bounds
#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * Construct a grid matching the specified bounding box.
         *
         * \param bb the bb to match
         * \param maxRes max resolution along the longest axis of the BB.
         * \param additional add additional space around the grid.
         *
         * \return the gird.
         */
        SPtr< core::GridRegular3 > regularGridForBoundingBox( const BoundingBox& bb, size_t maxRes, size_t additional = 0 );
    }
}

#endif  // DI_GRIDBUILDERS_H



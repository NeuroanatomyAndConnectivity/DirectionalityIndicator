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

#ifndef TRIANGLEDATASET_H
#define TRIANGLEDATASET_H

#include <string>
#include <tuple>
#include <vector>

#include "core/data/DataSet.h"
#include "core/data/TriangleMesh.h"

#include "MathTypes.h"
#include "GfxTypes.h"
#include "Types.h"

namespace di
{
    namespace core
    {
        /**
         * A basic encapsulation of a triangle mesh in combination with a RGBA color attribute.
         */
        class TriangleDataSet: public DataSet< TriangleMesh, RGBAArray > // triangle mesh as grid, no attributes
        {
        public:
            /**
             * Create a dataset containing only the triangle mesh as grid. Attributes are optional.
             *
             * \param triangles the triangle grid
             * \param color a color array
             * \param name the name of the dataset
             */
            TriangleDataSet( const std::string name, ConstSPtr< TriangleMesh > triangles, ConstSPtr< RGBAArray > color );

            /**
             * Destructor.
             */
            virtual ~TriangleDataSet();
        protected:
        private:
        };
    }
}

#endif  // TRIANGLEDATASET_H


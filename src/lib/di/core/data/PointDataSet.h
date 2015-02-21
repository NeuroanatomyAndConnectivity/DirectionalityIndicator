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

#ifndef DI_POINTDATASET_H
#define DI_POINTDATASET_H

#include <string>

#include <di/core/data/DataSet.h>
#include <di/core/data/Points.h>

#include <di/MathTypes.h>
#include <di/GfxTypes.h>
#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * A basic encapsulation of a bunch of points in combination with a RGBA color attribute.
         */
        class PointDataSet: public DataSet< Points, RGBAArray > // points as "grid", no attributes
        {
        public:
            /**
             * Create a dataset containing only the points as grid. Attributes are optional.
             *
             * \param points the point grid
             * \param color a color array
             * \param name the name of the dataset
             */
            PointDataSet( const std::string name, ConstSPtr< Points > points, ConstSPtr< RGBAArray > color );

            /**
             * Destructor.
             */
            virtual ~PointDataSet();
        protected:
        private:
        };
    }
}

#endif  // DI_POINTDATASET_H


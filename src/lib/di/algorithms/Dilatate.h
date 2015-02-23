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

#ifndef DI_DILATATE_H
#define DI_DILATATE_H

#include <mutex>

#include <di/core/Algorithm.h>
#include <di/core/data/DataSetTypes.h>

namespace di
{
    namespace algorithms
    {
        /**
         * Dilatate the given scalar data.
         */
        class Dilatate: public di::core::Algorithm
        {
        public:
            /**
             * Constructor. Initialize all inputs, outputs and parameters.
             */
            Dilatate();

            /**
             * Destructor. Clean up if needed.
             */
            virtual ~Dilatate();

            /**
             * Does nothing in this case, besides setting the injection data.
             */
            virtual void process();
        protected:
        private:
            /**
             * The scalar input to use.
             */
            SPtr< di::core::Connector< di::core::DataSetScalarRegular3d > > m_dataInput;

            /**
             * The voxel output to use.
             */
            SPtr< di::core::Connector< di::core::DataSetScalarRegular3d > > m_dataOutput;
        };
    }
}

#endif  // DI_DILATATE_H


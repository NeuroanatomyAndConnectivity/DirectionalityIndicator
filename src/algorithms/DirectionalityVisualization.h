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

#ifndef DIRECTIONALITYVISUALIZATION_H
#define DIRECTIONALITYVISUALIZATION_H

#include "core/Algorithm.h"

namespace di
{
    namespace core
    {
        class TriangleDataSet;
    }

    namespace algorithms
    {
        /**
         * Directionality Indicator. This class implements the algorithm and the visualization.
         */
        class DirectionalityVisualization: public di::core::Algorithm
                                           // public di::core::Visualization
        {
        public:
            /**
             * Constructor. Initialize all inputs, outputs and parameters.
             */
            DirectionalityVisualization();

            /**
             * Destructor. Clean up if needed.
             */
            virtual ~DirectionalityVisualization();

            /**
             * Process the data in the inputs and update output data. Keep in mind that this might be called in its own thread thread.
             */
            virtual void process();

        protected:
        private:
            /**
             * The triangle mesh input to use.
             */
            SPtr< di::core::AlgorithmData< di::core::TriangleDataSet > > m_triangleDataInput;
        };
    }
}

#endif  // DIRECTIONALITYVISUALIZATION_H


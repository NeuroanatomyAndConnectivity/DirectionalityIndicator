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

#ifndef EXTRACTREGIONS_H
#define EXTRACTREGIONS_H

#include <mutex>

#include "core/Algorithm.h"

namespace di
{
    namespace core
    {
        class LineDataSet;
        class TriangleDataSet;
    }

    namespace algorithms
    {
        /**
         * Extract regions from a given triangle mesh dataset.
         */
        class ExtractRegions: public di::core::Algorithm
        {
        public:
            /**
             * Constructor. Initialize all inputs, outputs and parameters.
             */
            ExtractRegions();

            /**
             * Destructor. Clean up if needed.
             */
            virtual ~ExtractRegions();

            /**
             * Does nothing in this case, besides setting the injection data.
             */
            virtual void process();
        protected:
        private:
            /**
             * The resulting line data.
             */
            SPtr< di::core::Connector< di::core::LineDataSet > > m_dataOutput;

            /**
             * The triangle mesh input to use.
             */
            SPtr< di::core::Connector< di::core::TriangleDataSet > > m_dataInput;
        };
    }
}

#endif  // EXTRACTREGIONS_H


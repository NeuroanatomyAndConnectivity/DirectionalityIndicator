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

#ifndef DI_EXTRACTREGIONS_H
#define DI_EXTRACTREGIONS_H

#include <mutex>
#include <set>
#include <vector>

#include <di/core/Algorithm.h>
#include <di/core/data/DataSetTypes.h>
#include <di/io/RegionLabelReader.h>
#include <di/core/ParameterTypes.h>

namespace di
{
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

            /**
             * Associate each region with its neighbours. The size_t is the region index.
             */
            typedef std::vector< std::set< size_t > > RegionNeighbourhood;

            /**
             * Edge-list of the connections between regions.
             */
            typedef std::map< size_t, std::set< size_t > > DirectedRegionNeighbourhood;

            /**
             * Associate each region with its connected region. The vector lists the source, the targets are in the set.
             */
            typedef std::vector< std::set< size_t > > RegionConnections;

            /**
             * The type used for storing region information. The point index is the region index.
             */
            typedef di::core::DataSet< core::Points, // set of center points, one per region. Point index is region index.
                                       std::vector< glm::vec3 >,  // set of normals for each region. Index is region index.
                                       di::RGBAArray, // region colors. One per region. Index is region index.
                                       RegionNeighbourhood, // associate each region index with a set of direct neighbours.
                                       RegionConnections  // associate a region index with a set of region indices that are connected.
                                     > RegionDataSet;

        protected:
        private:
            /**
             * True to switch directions
             */
            core::ParamBool m_enableDirectionSwitch;

            /**
             * The region mesh
             */
            SPtr< di::core::Connector< di::core::LineDataSet > > m_regionMeshOutput;


            /**
             * The vectors on the triangle data
             */
            SPtr< di::core::Connector< di::core::TriangleVectorField > > m_vectorOutput;

            /**
             * The triangle mesh input to use.
             */
            SPtr< di::core::Connector< di::core::TriangleDataSet > > m_dataInput;

            /**
             * The triangle label input to use.
             */
            SPtr< di::core::Connector< di::io::RegionLabelReader::DataSetType > > m_dataLabelInput;

            /**
             * The triangle label ordering to use.
             */
            SPtr< di::core::Connector< di::io::RegionLabelReader::DataSetType > > m_dataLabelOrderingInput;

        };
    }
}

#endif  // DI_EXTRACTREGIONS_H


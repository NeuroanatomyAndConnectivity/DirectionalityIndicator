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

#define LogTag "algorithms/DirectionalityVisualization"
#include "core/Logger.h"

#include "core/data/TriangleDataSet.h"

#include "DirectionalityVisualization.h"

namespace di
{
    namespace algorithms
    {
        DirectionalityVisualization::DirectionalityVisualization():
            Algorithm( "Directionality Visualization",
                       "This algorithm takes a triangle mesh and scalar information defined on the mesh. "
                       "It the creates directional information from the scalar data and displays it on "
                       "the mesh itself visually." ),
            Visualization()
        {
            // We require some inputs.

            // 1: the triangle mesh
            m_triangleDataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle mesh on which the directionality information should be shown."
            );
        }

        DirectionalityVisualization::~DirectionalityVisualization()
        {
            // nothing to clean up so far
        }

        void DirectionalityVisualization::process()
        {
            // Get input data
            auto data = m_triangleDataInput->getData();
            LogD << "Got data instance " << static_cast< const void* >( data.get() ) << "." << LogEnd;

            // Provide the needed information to the visualizer itself.
            m_visTriangleData = data;

            // As the rendering system does not render permanently, inform about the update.
            renderRequest();
        }

        void DirectionalityVisualization::prepare()
        {

        }

        void DirectionalityVisualization::finalize()
        {
        }

        void DirectionalityVisualization::render()
        {
        }

        void DirectionalityVisualization::update()
        {
        }
    }
}


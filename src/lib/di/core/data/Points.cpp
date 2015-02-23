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

#include <vector>
#include <map>

#include "Points.h"

namespace di
{
    namespace core
    {
        Points::Points()
        {
            // nothing to do. Vectors are initialized already.
        }

        Points::~Points()
        {
            // nothing to do. Vectors get cleared on destruction.
        }

        size_t Points::addVertex( const glm::vec3& vertex )
        {
            m_boundingBox.include( vertex );
            m_vertices.push_back( vertex );
            return m_vertices.size() - 1;
        }

        size_t Points::addVertex( float x, float y, float z )
        {
            return addVertex(
                {
                    x, y, z
                }
            );
        }

        const Vec3Array& Points::getVertices() const
        {
            return m_vertices;
        }

        size_t Points::getNumVertices() const
        {
            return m_vertices.size();
        }

        const BoundingBox& Points::getBoundingBox() const
        {
            return m_boundingBox;
        }

        const Points::Point& Points::getVertex( size_t pointID ) const
        {
            return m_vertices[ pointID ];
        }

        Points::Point& Points::getVertex( size_t pointID )
        {
            return m_vertices[ pointID ];
        }

        void Points::setVertices( const Vec3Array& vertices )
        {
            m_vertices = vertices;
        }
    }
}

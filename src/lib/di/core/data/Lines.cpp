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

#define LogTag "core/data/Lines"
#include <di/core/Logger.h>

#include "Lines.h"

namespace di
{
    namespace core
    {
        Lines::Lines()
        {
            // nothing to do. Vectors are initialized already.
        }

        Lines::~Lines()
        {
            // nothing to do. Vectors get cleared on destruction.
        }

        size_t Lines::addVertex( const glm::vec3& vertex )
        {
            m_boundingBox.include( vertex );
            m_vertices.push_back( vertex );
            return m_vertices.size() - 1;
        }

        size_t Lines::addVertex( float x, float y, float z )
        {
            return addVertex(
                {
                    x, y, z
                }
            );
        }

        size_t Lines::addLine( glm::ivec2 indices )
        {
            m_lines.push_back( indices );
            return m_lines.size() - 1;
        }

        size_t Lines::addLine( size_t index1, size_t index2 )
        {
            return addLine(
                {
                    index1, index2
                }
            );
        }

        const Vec3Array& Lines::getVertices() const
        {
            return m_vertices;
        }

        const IndexVec2Array& Lines::getLines() const
        {
            return m_lines;
        }

        size_t Lines::getNumLines() const
        {
            return m_lines.size();
        }

        size_t Lines::getNumVertices() const
        {
            return m_vertices.size();
        }

        bool Lines::sanityCheck() const
        {
            bool enoughLines = ( getNumLines() >= 1 );
            return enoughLines;
        }

        const BoundingBox& Lines::getBoundingBox() const
        {
            return m_boundingBox;
        }

        Lines::Line Lines::getVertices( size_t lineID ) const
        {
            auto vertexIDs = m_lines[ lineID ];
            return std::make_tuple( m_vertices[ vertexIDs.x ],
                                    m_vertices[ vertexIDs.y ] );
        }

        void Lines::setVertices( const Vec3Array& vertices )
        {
            m_vertices = vertices;
        }

        void Lines::setLines( const IndexVec2Array& lines )
        {
            m_lines = lines;
        }
    }
}

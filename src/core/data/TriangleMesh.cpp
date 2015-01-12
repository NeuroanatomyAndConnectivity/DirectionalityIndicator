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

#include "TriangleMesh.h"

namespace di
{
    namespace core
    {
        TriangleMesh::TriangleMesh()
        {
            // nothing to do. Vectors are initialized already.
        }

        TriangleMesh::~TriangleMesh()
        {
            // nothing to do. Vectors get cleared on destruction.
        }

        size_t TriangleMesh::addVertex( const glm::vec3& vertex )
        {
            m_vertices.push_back( vertex );
            return m_triangles.size() - 1;
        }

        size_t TriangleMesh::addVertex( double x, double y, double z )
        {
            return addVertex(
                {
                    x, y, z
                }
            );
        }

        size_t TriangleMesh::addTriangle( glm::ivec3 indices )
        {
            m_triangles.push_back( indices );
            return m_triangles.size() - 1;
        }

        size_t TriangleMesh::addTriangle( size_t index1, size_t index2, size_t index3 )
        {
            return addTriangle(
                {
                    index1, index2, index3
                }
            );
        }

        const Vec3Array& TriangleMesh::getVertices() const
        {
            return m_vertices;
        }

        const IndexVec3Array& TriangleMesh::getTriangles() const
        {
            return m_triangles;
        }

        size_t TriangleMesh::getNumTriangles() const
        {
            return m_triangles.size();
        }

        size_t TriangleMesh::getNumVertices() const
        {
            return m_vertices.size();
        }
    }
}

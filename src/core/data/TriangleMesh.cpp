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

#define LogTag "core/data/TriangleMesh"
#include "core/Logger.h"

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
            m_boundingBox.include( vertex );
            m_vertices.push_back( vertex );
            return m_triangles.size() - 1;
        }

        size_t TriangleMesh::addVertex( float x, float y, float z )
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

        size_t TriangleMesh::getNumNormals() const
        {
            return m_normals.size();
        }

        bool TriangleMesh::sanityCheck() const
        {
            bool enoughTris = ( getNumTriangles() >= 1 );
            bool enoughNormals = ( getNumNormals() == 0 ) || ( getNumNormals() == getNumTriangles() );  // either a normal for every vertex or none.
            return enoughTris && enoughNormals;
        }

        const BoundingBox& TriangleMesh::getBoundingBox() const
        {
            return m_boundingBox;
        }

        const NormalArray& TriangleMesh::getNormals() const
        {
            return m_normals;
        }

        TriangleMesh::Triangle TriangleMesh::getVertices( size_t triangleID ) const
        {
            auto vertexIDs = m_triangles[ triangleID ];
            return std::make_tuple( m_vertices[ vertexIDs.x ],
                                    m_vertices[ vertexIDs.y ],
                                    m_vertices[ vertexIDs.z ] );
        }

        void TriangleMesh::calculateNormals()
        {
            m_normals.clear();

            // we want to keep track of the triangles that share a vertex
            std::map< IndexVec3Array::value_type::value_type, std::vector< size_t > > vertexTriMap;

            // iterate all triangles and map between vertex and triangle
            for( size_t triID = 0; triID < m_triangles.size(); ++triID )
            {
                auto tri = m_triangles[ triID ];

                // temporarily store a map between the vertex id and the corresponding triangle index. A reverse index so to say.
                vertexTriMap[ tri.x ].push_back( triID );
                vertexTriMap[ tri.y ].push_back( triID );
                vertexTriMap[ tri.z ].push_back( triID );
            }

            // we can now go through each vertex and
            for( size_t vertID = 0; vertID < m_vertices.size(); ++vertID )
            {
                // store all triangle normals of all triangles associated with this vertex in here:
                std::vector< glm::vec3 > neighbourNormals;

                // and iterate each triangle it belongs:
                for( auto triID : vertexTriMap[ vertID ] )
                {
                    Triangle vertices = getVertices( triID );

                    // do the typical cross-product style normal calculation:
                    auto v1 = std::get< 1 >( vertices ) - std::get< 0 >( vertices );
                    auto v2 = std::get< 2 >( vertices ) - std::get< 1 >( vertices );

                    // cross product
                    neighbourNormals.push_back( glm::normalize( glm::cross( v1, v2 ) ) );
                }

                // we now have all triangle normals of the triangles that use this vertex. Merge the normals to get a smooth vertex normal:
                glm::vec3 smoothNormal( 0.0, 0.0, 0.0 );
                for( auto normal : neighbourNormals )
                {
                    smoothNormal += normal;
                }

                // store the smooth normal for this vertex:
                m_normals.push_back( glm::normalize( smoothNormal ) );
            }
        }
    }
}

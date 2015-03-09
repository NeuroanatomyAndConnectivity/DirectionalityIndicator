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

#include <algorithm>
#include <vector>
#include <map>

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
            return m_vertices.size() - 1;
        }

        size_t TriangleMesh::addVertex( float x, float y, float z )
        {
            return addVertex(
                {
                    x, y, z
                }
            );
        }

        size_t TriangleMesh::addNormal( float x, float y, float z )
        {
            return addNormal(
                {
                    x, y, z
                }
            );
        }

        size_t TriangleMesh::addNormal( const glm::vec3& normal )
        {
            m_normals.push_back( normal );
            return m_normals.size() - 1;
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
            // Range check is done by std::vector.
            auto vertexIDs = m_triangles[ triangleID ];
            return std::make_tuple( m_vertices[ vertexIDs.x ],
                                    m_vertices[ vertexIDs.y ],
                                    m_vertices[ vertexIDs.z ] );
        }

        glm::vec3 TriangleMesh::getVertex( size_t vertexID ) const
        {
            // Range check is done by std::vector.
            return m_vertices[ vertexID ];
        }

        void TriangleMesh::setTriangles( const IndexVec3Array& triangles )
        {
            m_triangles = triangles;
        }

        void TriangleMesh::setVertices( const Vec3Array& vertices )
        {
            m_vertices = vertices;
        }

        void TriangleMesh::setNormals( const NormalArray& normals )
        {
            m_normals = normals;
        }

        glm::vec3 TriangleMesh::getNormal( size_t vertexID ) const
        {
            return m_normals[ vertexID ];
        }

        void TriangleMesh::calculateInverseIndex() const
        {
            // Clean up and create empty vector with as much empty elements as vertices.
            m_inverseIndex.clear();
            m_inverseIndex.resize( getNumVertices() );

            // iterate all triangles and map between vertex and triangle
            // NOTE: as the triangle Index is increasing, the inverse index is sorted automatically.
            for( size_t triID = 0; triID < m_triangles.size(); ++triID )
            {
                // get verts of this triangle
                auto vertexIDs = m_triangles[ triID ];
                m_inverseIndex[ vertexIDs.x ].push_back( triID );
                m_inverseIndex[ vertexIDs.y ].push_back( triID );
                m_inverseIndex[ vertexIDs.z ].push_back( triID );
            }
        }

        std::vector< size_t > TriangleMesh::getNeighbours( size_t triID ) const
        {
            if( m_inverseIndex.empty() )
            {
                calculateInverseIndex();
            }

            // Get triangles of each vertex
            auto tris1 = getTrianglesForVertex( m_triangles[ triID ].x );
            auto tris2 = getTrianglesForVertex( m_triangles[ triID ].y );
            auto tris3 = getTrianglesForVertex( m_triangles[ triID ].z );

            // Reserve enough space
            tris3.reserve( tris1.size() + tris2.size() + tris3.size() );
            tris3.insert( tris3.end(), tris2.begin(), tris2.end() );
            tris3.insert( tris3.end(), tris1.begin(), tris3.end() );

            // sort and make unique.
            std::sort( tris3.begin(), tris3.end() );
            auto last = std::unique( tris3.begin(), tris3.end() );
            tris3.erase( last, tris3.end() );

            return tris3;
        }

        std::vector< size_t > TriangleMesh::getNeighbourVertices( size_t vertexID ) const
        {
            if( m_inverseIndex.empty() )
            {
                calculateInverseIndex();
            }

            std::vector< size_t > result;

            // Get triangles of each vertex
            auto tris = getTrianglesForVertex( vertexID );
            result.reserve( tris.size() * 3 );
            for( auto triID : tris )
            {
                auto vertexIDs = m_triangles[ triID ];
                // NOTE: one of them is == vertexID
                result.push_back( vertexIDs.x );
                result.push_back( vertexIDs.y );
                result.push_back( vertexIDs.z );
            }

            // sort and make unique.
            std::sort( result.begin(), result.end() );
            auto last = std::unique( result.begin(), result.end() );
            result.erase( last, result.end() );

            return result;
        }

        const std::vector< size_t >& TriangleMesh::getTrianglesForVertex( size_t vertexID ) const
        {
            if( m_inverseIndex.empty() )
            {
                calculateInverseIndex();
            }

            // we already have this information:
            return m_inverseIndex[ vertexID ];
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

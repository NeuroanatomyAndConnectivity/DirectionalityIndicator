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

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include <tuple>

#include <di/core/BoundingBox.h>

#include <di/MathTypes.h>
#include <di/GfxTypes.h>

namespace di
{
    namespace core
    {
        /**
         * This is a basic, indexed triangle mesh class for three-dimensional meshes.
         *
         * It only defines the grid properties of the mesh. No attributes are stored (attributes like normals,
         * colors). The purpose of not storing attributes is to be able to use a mesh as grid (2-simplex) for calculations.
         */
        class TriangleMesh
        {
        public:
            /**
             * Constructor. Creates an empty triangle mesh.
             */
            TriangleMesh();

            /**
             * Destructor to clean up the contents.
             */
            virtual ~TriangleMesh();

            /**
             * Add a vertex to the vertex list.
             *
             * \param vertex the vertex to add.
             *
             * \return the index of the vertex.
             */
            size_t addVertex( const glm::vec3& vertex );

            /**
             * Add a vertex to the vertex list.
             *
             * \param x x component
             * \param y y component
             * \param z z component
             *
             * \return the index of the vertex.
             */
            size_t addVertex( float x, float y, float z );

            /**
             * Add the given indices to the index list to define a new triangle.
             *
             * \param index1 index of first vertex
             * \param index2 index of second vertex
             * \param index3 index of third vertex
             *
             * \return the index of the triangle.
             */
            size_t addTriangle( size_t index1, size_t index2, size_t index3 );

            /**
             * Add the given indices to the index list to define a new triangle. There are no sanity checks for index validity. This allows you to
             * define triangle indices before adding vertices. Useful for file loading.
             *
             * \param indices the index vector.
             *
             * \return the index of the triangle.
             */
            size_t addTriangle( glm::ivec3 indices );

            /**
             * Get the vertex array.
             *
             * \return the vertex array.
             */
            const Vec3Array& getVertices() const;

            /**
             * Set the vertex array. Previously added vertices will be overwritten.
             *
             * \param vertices the vertex array.
             */
            void setVertices( const Vec3Array& vertices );

            /**
             * Abbreviation for 3 vertices of a triangle.
             */
            typedef std::tuple< glm::vec3, glm::vec3, glm::vec3 > Triangle;

            /**
             * Get the vertices for the specified triangle id.
             *
             * \throw std::out_of_range if the index is invalid.
             *
             * \param triangleID the id of the triangle to retrieve the vertices for.
             *
             * \return the vertices
             */
            Triangle getVertices( size_t triangleID ) const;

            /**
             * Get the vertex associated with the given vertex ID.
             *
             * \param vertexID the vertex ID
             * \throw std::out_of_range if the index is invalid.
             *
             * \return the vertex
             */
            glm::vec3 getVertex( size_t vertexID ) const;

            /**
             * Get the triangle normal array. There is a normal for each vertex. Per-triangle normals are not supported. To achieve this, ensure that
             * no vertex is used by multiple triangles. Than store the same normal for each of the three vertices of a single triangle.
             *
             * \return the normal array.
             */
            const NormalArray& getNormals() const;

            /**
             * Query the normal at a given vertex.
             *
             * \param vertexID the ID of the vertex
             *
             * \return the normal
             */
            glm::vec3 getNormal( size_t vertexID ) const;

            /**
             * Set the given normals. Overwrites previously defined normals
             *
             * \param normals the normals to set
             */
            void setNormals( const NormalArray& normals );

            /**
             * Get the amount of normals for this mesh. A valid mesh will contain either 0 or exactly 1 for each vertex.
             *
             * \return the number of normals.
             */
            size_t getNumNormals() const;

            /**
             * Get the triangle index array.
             *
             * \return the index array.
             */
            const IndexVec3Array& getTriangles() const;

            /**
             * Set the triangle index array. Overwrites previously set triangles.
             *
             * \param triangles the index array.
             */
            void setTriangles( const IndexVec3Array& triangles );

            /**
             * The number of triangles currently defined.
             *
             * \return the number of triangles.
             */
            size_t getNumTriangles() const;

            /**
             * The number of vertices currently defined. Please keep in mind that the number of triangles may not be 3 * \ref getNumTriangles().
             *
             * \return the number of vertices.
             */
            size_t getNumVertices() const;

            /**
             * Checks if the data is reasonable. Basically, this ensures that there is at least one triangle and that there are enough vertices. It
             * also checks that there are either no normals defined or exactly one per vertex.
             *
             * \return true if reasonable data is present.
             */
            bool sanityCheck() const;

            /**
             * Get the bounding volume of this triangle mesh.
             *
             * \return the bounding box.
             */
            const BoundingBox& getBoundingBox() const;

            /**
             * This is a useful function to calculate smooth normals. To have it create semi-per-triangle-normals, do not share vertices for the
             * triangles. The normals are only smooth for triangles with shared vertices.
             */
            void calculateNormals();

        protected:
        private:
            /**
             * Vertex array.
             */
            Vec3Array m_vertices = {};

            /**
             * Triangle index list. Index the triangle to query its 3 vertices.
             */
            IndexVec3Array m_triangles = {};

            /**
             * Normals.
             */
            NormalArray m_normals = {};

            /**
             * The bounding box.
             */
            BoundingBox m_boundingBox;
        };
    }
}

#endif  // TRIANGLEMESH_H


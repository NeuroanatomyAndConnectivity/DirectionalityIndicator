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

#include "MathTypes.h"
#include "GfxTypes.h"

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
             * Get the triangle index array.
             *
             * \return the index array.
             */
            const IndexVec3Array& getTriangles() const;

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
             * Checks if the data is reasonable. Basically, this ensures that there is at least one triangle and that there are enough vertices.
             *
             * \return true if reasonable data is present.
             */
            bool sanityCheck() const;
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
        };
    }
}

#endif  // TRIANGLEMESH_H


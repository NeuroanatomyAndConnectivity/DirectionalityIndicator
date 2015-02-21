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

#ifndef DI_POINTS_H
#define DI_POINTS_H

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
         * This is a basic, indexed point class for three-dimensional piles of points.
         *
         * It only defines the grid properties of the mesh. No attributes are stored (attributes like colors).
         */
        class Points
        {
        public:
            /**
             * Constructor. Creates an empty point set.
             */
            Points();

            /**
             * Destructor to clean up the contents.
             */
            virtual ~Points();

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
             * Abbreviation for 2 vertices of a point.
             */
            typedef glm::vec3 Point;

            /**
             * Get the vertices for the specified point id.
             *
             * \throw std::out_of_range if the index is invalid.
             *
             * \param pointID the id of the point to retrieve the vertices for.
             *
             * \return the vertices
             */
            const Point& getVertex( size_t pointID ) const;

            /**
             * Get the vertices for the specified point id.
             *
             * \throw std::out_of_range if the index is invalid.
             *
             * \param pointID the id of the point to retrieve the vertices for.
             *
             * \return the vertices
             */
            Point& getVertex( size_t pointID );

            /**
             * The number of vertices currently defined.
             *
             * \return the number of vertices.
             */
            size_t getNumVertices() const;

            /**
             * Get the bounding volume of these points.
             *
             * \return the bounding box.
             */
            const BoundingBox& getBoundingBox() const;

        protected:
        private:
            /**
             * Vertex array.
             */
            Vec3Array m_vertices = {};

            /**
             * The bounding box.
             */
            BoundingBox m_boundingBox;
        };
    }
}

#endif  // DI_POINTS_H


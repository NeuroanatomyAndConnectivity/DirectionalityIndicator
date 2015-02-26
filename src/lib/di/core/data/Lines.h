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

#ifndef DI_LINES_H
#define DI_LINES_H

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
         * This is a basic, indexed line  class for three-dimensional piles of lines.
         *
         * It only defines the grid properties of the mesh. No attributes are stored (attributes like colors).
         */
        class Lines
        {
        public:
            /**
             * Constructor. Creates an empty line set.
             */
            Lines();

            /**
             * Destructor to clean up the contents.
             */
            virtual ~Lines();

            /**
             * Add a vertex to the vertex list.
             *
             * \param vertex the vertex to add.
             * \param ifUnique add vertex if not yet present. If already present, return its index.
             *
             * \return the index of the vertex.
             */
            size_t addVertex( const glm::vec3& vertex, bool ifUnique = false );

            /**
             * Add a vertex to the vertex list.
             *
             * \param x x component
             * \param y y component
             * \param z z component
             * \param ifUnique add vertex if not yet present. If already present, return its index.
             *
             * \return the index of the vertex.
             */
            size_t addVertex( float x, float y, float z, bool ifUnique = false );

            /**
             * Add the given indices to the index list to define a new line.
             *
             * \param index1 index of first vertex
             * \param index2 index of second vertex
             *
             * \return the index of the line.
             */
            size_t addLine( size_t index1, size_t index2 );

            /**
             * Add the given indices to the index list to define a new line. There are no sanity checks for index validity. This allows you to
             * define line indices before adding vertices. Useful for file loading.
             *
             * \param indices the index vector.
             *
             * \return the index of the line.
             */
            size_t addLine( glm::ivec2 indices );

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
             * Abbreviation for 2 vertices of a line.
             */
            typedef std::tuple< glm::vec3, glm::vec3 > Line;

            /**
             * Get the vertices for the specified line id.
             *
             * \throw std::out_of_range if the index is invalid.
             *
             * \param lineID the id of the line to retrieve the vertices for.
             *
             * \return the vertices
             */
            Line getVertices( size_t lineID ) const;

            /**
             * Get vertex for the given ID.
             *
             * \param vertexID the ID
             *
             * \return the vertex.
             */
            glm::vec3 getVertex( size_t vertexID ) const;

            /**
             * Get the lines index array.
             *
             * \return the index array.
             */
            const IndexVec2Array& getLines() const;

            /**
             * Get the lines index array.
             *
             * \param lineID the line to get
             *
             * \return the indices of start and end of the line.
             */
            glm::ivec2 getLine( size_t lineID ) const;

            /**
             * Set the lines index array. Overwrites previously set lines.
             *
             * \param lines the index array.
             */
            void setLines( const IndexVec2Array& lines );

            /**
             * The number of lines currently defined.
             *
             * \return the number of lines.
             */
            size_t getNumLines() const;

            /**
             * The number of vertices currently defined. Please keep in mind that the number of vertices might not be 2 * \ref getNumLines().
             *
             * \return the number of vertices.
             */
            size_t getNumVertices() const;

            /**
             * Checks if the data is reasonable. Basically, this ensures that there is at least one line and that there are enough vertices.
             *
             * \return true if reasonable data is present.
             */
            bool sanityCheck() const;

            /**
             * Get the bounding volume of these lines.
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
             * Line index list. Index the line to query its 2 vertices.
             */
            IndexVec2Array m_lines = {};

            /**
             * The bounding box.
             */
            BoundingBox m_boundingBox;
        };
    }
}

#endif  // DI_LINES_H


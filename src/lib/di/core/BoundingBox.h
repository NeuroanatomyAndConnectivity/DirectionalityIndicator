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

#ifndef DI_BOUNDINGBOX_H
#define DI_BOUNDINGBOX_H

#include <di/MathTypes.h>

namespace di
{
    namespace core
    {
        /**
         * This is a basic bounding box implementation.
         */
        class BoundingBox
        {
        public:
            /**
             * Constructor. Creates an empty bounding box with a 0 volume.
             */
            BoundingBox();

            /**
             * Destructor to clean up the contents.
             */
            virtual ~BoundingBox();

            /**
             * Get the bounding box minimum values.
             *
             * \return the minimum values
             */
            const glm::dvec3& getMin() const;

            /**
             * Get the bounding box maximum values.
             *
             * \return the maximum values
             */
            const glm::dvec3& getMax() const;

            /**
             * Get the bounding box center.
             *
             * \return the center point
             */
            glm::dvec3 getCenter() const;

            /**
             * Get the size of the bounding box.
             *
             * \return size.
             */
            glm::dvec3 getSize() const;

            /**
             * Include the given vertex.
             *
             * \param x x coord
             * \param y y coord
             * \param z z coord
             */
            void include( double x, double y, double z = 0.0 );

            /**
             * Include the given vertex.
             *
             * \param point the point to include.
             */
            void include( const glm::vec3& point );

            /**
             * Include the given vertex. If specifying a 2D point, z is assumed to be 0.
             *
             * \param point the point to include.
             */
            void include( const glm::vec2& point );

            /**
             * Include the given bounding box.
             *
             * \param bb the bounding box to include.
             */
            void include( const BoundingBox& bb );

        protected:
        private:
            /**
             * Min values in each direction.
             */
            glm::dvec3 m_bbMin;

            /**
             * Max values in each direction.
             */
            glm::dvec3 m_bbMax;
        };
    }
}

#endif  // DI_BOUNDINGBOX_H


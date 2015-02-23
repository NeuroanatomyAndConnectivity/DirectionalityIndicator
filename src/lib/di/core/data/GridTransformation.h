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

#ifndef DI_GRIDTRANSFORMATION_H
#define DI_GRIDTRANSFORMATION_H

#include <algorithm>

#include <di/core/BoundingBox.h>
#include <di/MathTypes.h>

#include <di/core/Logger.h>
#define LogTag "core/GridTransformation"

namespace di
{
    namespace core
    {
        class BoundingBox;

        /**
         * Represent the transformation from world-space to grid-space and vice-versa.
         *
         * \note: specialize for other dimensions!
         */
        template< size_t Dimensions = 3 >
        class GridTransformation
        {
        public:
            /**
             * Constructor. Default is to create a non-transform (identity).
             */
            GridTransformation() = default;

            /**
             * Copy constructor.
             *
             * \param other the instance to copy
             */
            GridTransformation( const GridTransformation& other ) = default;

            /**
             * Assign operation.
             *
             * \param other the instance to assign to this one.
             *
             * \return  this.
             */
            GridTransformation& operator=( const GridTransformation& other ) = default;

            /**
             * Destructor. Clean up if needed.
             */
            virtual ~GridTransformation() = default;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Convenience construction
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Specify a matrix directly.
             *
             * \param matrix the matrix.
             */
            GridTransformation( const glm::mat4& matrix ):
                m_matrix( matrix )
            {
            }

            /**
             * Transform the specified world-space vector to grid space.
             *
             * \tparam VectorType the type of vector.
             * \param v the vector
             *
             * \return the transformed vector
             */
            template< typename VectorType >
            VectorType operator*( const VectorType& v ) const
            {
                return m_matrix * v;
            }

            /**
             * Transform the specified world-space vector to grid space.
             *
             * \param v the vector
             *
             * \return the transformed vector
             */
            glm::vec3 operator*( const glm::vec3& v ) const
            {
                auto result = m_matrix * glm::vec4( v, 1.0 );
                return glm::vec3( result.x, result.y, result.z );
            }

        protected:
        private:
            /**
             * The matrix representing the transformation. FROM world-space TO grid-space
             */
            glm::dmat4 m_matrix;
        };
    }
}

#endif  // DI_GRIDTRANSFORMATION_H


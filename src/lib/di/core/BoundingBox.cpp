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
#include <limits>

#include <di/MathTypes.h>

#include "BoundingBox.h"

namespace di
{
    namespace core
    {
        BoundingBox::BoundingBox():
            m_bbMin( std::numeric_limits< double >::max() ),
            m_bbMax( std::numeric_limits< double >::min() )
        {
        }

        BoundingBox::~BoundingBox()
        {
        }

        const glm::dvec3& BoundingBox::getMin() const
        {
            return m_bbMin;
        }

        const glm::dvec3& BoundingBox::getMax() const
        {
            return m_bbMax;
        }

        glm::dvec3 BoundingBox::getCenter() const
        {
            return m_bbMin + ( 0.5 * ( m_bbMax - m_bbMin ) );
        }

        glm::dvec3 BoundingBox::getSize() const
        {
            return m_bbMax - m_bbMin;
        }

        void BoundingBox::include( double x, double y, double z )
        {
            m_bbMin.x = std::min( m_bbMin.x, x );
            m_bbMin.y = std::min( m_bbMin.y, y );
            m_bbMin.z = std::min( m_bbMin.z, z );

            m_bbMax.x = std::max( m_bbMax.x, x );
            m_bbMax.y = std::max( m_bbMax.y, y );
            m_bbMax.z = std::max( m_bbMax.z, z );
        }

        void BoundingBox::include( const glm::vec3& point )
        {
            include( point.x, point.y, point.z );
        }

        void BoundingBox::include( const glm::vec2& point )
        {
            include( point.x, point.y, 0.0 );
        }

        void BoundingBox::include( const BoundingBox& bb )
        {
            // ignore BB without a size
            if( bb.getMin().x > bb.getMax().x )
            {
                return;
            }
            include( bb.getMin().x, bb.getMin().y, bb.getMin().z );
            include( bb.getMax().x, bb.getMax().y, bb.getMax().z );
        }
    }
}


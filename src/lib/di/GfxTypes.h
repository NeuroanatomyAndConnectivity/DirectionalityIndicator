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

#ifndef DI_GFXTYPES_H
#define DI_GFXTYPES_H

// NOTE: This file is mostly used to include some standard graphics types, like vector arrays and similar.

#include <vector>
#include <ostream>

// we need the math types
#include <di/MathTypes.h>

namespace di
{
    typedef glm::vec4 Color;

    typedef std::vector< glm::vec2 > Vec2Array;
    typedef std::vector< glm::vec3 > Vec3Array;
    typedef std::vector< glm::vec4 > Vec4Array;

    typedef std::vector< glm::vec3 > NormalArray;

    typedef std::vector< glm::vec4 > RGBAArray;

    typedef std::vector< glm::ivec2 > IndexVec2Array;
    typedef std::vector< glm::ivec3 > IndexVec3Array;
    typedef std::vector< glm::ivec4 > IndexVec4Array;


    /**
     * Output to stream.
     *
     * \param os the stream to write to
     * \param obj the obj to write
     *
     * \return the stream
     */
    inline std::ostream& operator<<( std::ostream& os, const glm::vec2& obj )
    {
        os << obj[ 0 ] << "," << obj[ 1 ];
        return os;
    }

    /**
     * Output to stream.
     *
     * \param os the stream to write to
     * \param obj the obj to write
     *
     * \return the stream
     */
    inline std::ostream& operator<<( std::ostream& os, const glm::vec3& obj )
    {
        os << obj[ 0 ] << "," << obj[ 1 ] << "," << obj[ 2 ];
        return os;
    }

    /**
     * Output to stream.
     *
     * \param os the stream to write to
     * \param obj the obj to write
     *
     * \return the stream
     */
    inline std::ostream& operator<<( std::ostream& os, const glm::vec4& obj )
    {
        for( int c = 0; c < 3; ++c )
        {
            os << obj[ c ] << ",";
        }
        os << obj[ 3 ];
        return os;
    }

    /**
     * Output to stream.
     *
     * \param os the stream to write to
     * \param obj the obj to write
     *
     * \return the stream
     */
    inline std::ostream& operator<<( std::ostream& os, const glm::mat4& obj )
    {
        for( int r = 0; r < 3; ++r )
        {
            os << obj[ r ] << ",";
        }
        os << obj[ 3 ];
        return os;
    }

    /**
     * Output to stream.
     *
     * \param os the stream to write to
     * \param obj the obj to write
     *
     * \return the stream
     */
    inline std::ostream& operator<<( std::ostream& os, const glm::mat3& obj )
    {
        for( int r = 0; r < 2; ++r )
        {
            os << obj[ r ] << ",";
        }
        os << obj[ 2 ];
        return os;
    }
}

#endif  // DI_GFXTYPES_H


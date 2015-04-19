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

#ifndef DI_CONVERSION_H
#define DI_CONVERSION_H

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <type_traits>
#include <stdexcept>

#include <di/core/StringUtils.h>

#include <di/GfxTypes.h>

namespace di
{
    namespace core
    {

        /**
         * Convert a given string to the desired type. This function needs to be specialized for more complex types. This specific version
         * handles,
         * floats,doubles (up to long double), ints (up to long long), unsigned ints  (up to long long), bools, chars ...
         *
         * \tparam ValueType the desired target type
         * \param source the source string
         *
         * \throw std::invalid_argument if no conversion could be performed
         * \throw std::out_of_range if the string describes a value too large to fit into the target type.
         *
         * \return the value
         */
        template< typename ValueType >
        inline ValueType fromString( const std::string& source )
        {
            static_assert( std::is_arithmetic< ValueType >::value, "Non arithmetic types not supported by this function." );

            if( std::is_integral< ValueType >::value )
            {
                if( std::is_unsigned< ValueType >::value )
                {
                    return static_cast< ValueType >( stoull( source ) );
                }
                else
                {
                    return static_cast< ValueType >( stoll( source ) );
                }
            }

            if( std::is_floating_point< ValueType >::value )
            {
                return static_cast< ValueType >( std::stold( source ) );
            }

            return ValueType();
        }

        /**
         * \copydoc fromString
         *
         * \note specialize to lists of numbers
         */
        template< typename ValueType >
        inline std::vector< ValueType > fromStringAsVector( const std::string& source )
        {
            auto values = di::core::split( source, ',' );
            std::vector< ValueType > result;

            // Iterate
            for( auto number : values )
            {
                result.push_back( fromString< ValueType >( di::core::trim( number ) ) );
            }

            return result;
        }

        /**
         * \copydoc fromString
         *
         * \note specialize to glm::vec types
         */
        template<>
        inline glm::vec2 fromString< glm::vec2 >( const std::string& source )
        {
            auto values = fromStringAsVector< float >( source );
            if( values.size() != 2 )
            {
                throw std::invalid_argument( "String is not a vec2. Requires exactly 2 arguments." );
            }

            return glm::vec2( values[ 0 ], values[ 1 ] );
        }

        /**
         * \copydoc fromString
         *
         * \note specialize to glm::vec types
         */
        template<>
        inline glm::vec3 fromString< glm::vec3 >( const std::string& source )
        {
            auto values = fromStringAsVector< float >( source );
            if( values.size() != 3 )
            {
                throw std::invalid_argument( "String is not a vec3. Requires exactly 3 arguments." );
            }

            return glm::vec3( values[ 0 ], values[ 1 ], values[ 2 ] );
        }

        /**
         * \copydoc fromString
         *
         * \note specialize to glm::vec types
         */
        template<>
        inline glm::vec4 fromString< glm::vec4 >( const std::string& source )
        {
            auto values = fromStringAsVector< float >( source );
            if( values.size() != 4 )
            {
                throw std::invalid_argument( "String is not a vec4. Requires exactly 4 arguments." );
            }

            return glm::vec4( values[ 0 ], values[ 1 ], values[ 2 ], values[ 3 ] );
        }

        /**
         * \copydoc fromString
         *
         * \note specialize to glm::mat types
         */
        template<>
        inline glm::mat2 fromString< glm::mat2 >( const std::string& source )
        {
            auto values = fromStringAsVector< float >( source );
            if( values.size() != 2 * 2 )
            {
                throw std::invalid_argument( "String is not a mat2. Requires exactly 2*2 arguments." );
            }

            return glm::mat2( glm::vec2( values[ 0 ], values[ 1 ] ), glm::vec2( values[ 2 ], values[ 3 ] ) );
        }

        /**
         * \copydoc fromString
         *
         * \note specialize to glm::mat types
         */
        template<>
        inline glm::mat3 fromString< glm::mat3 >( const std::string& source )
        {
            auto values = fromStringAsVector< float >( source );
            if( values.size() != 3 * 3 )
            {
                throw std::invalid_argument( "String is not a mat3. Requires exactly 3*3 arguments." );
            }

            return glm::mat3( glm::vec3( values[0], values[1], values[2] ), glm::vec3( values[3], values[4], values[5] ),
                              glm::vec3( values[6], values[7], values[8] ) );
        }

        /**
         * \copydoc fromString
         *
         * \note specialize to glm::mat types
         */
        template<>
        inline glm::mat4 fromString< glm::mat4 >( const std::string& source )
        {
            auto values = fromStringAsVector< float >( source );
            if( values.size() != 4 * 4 )
            {
                throw std::invalid_argument( "String is not a mat4. Requires exactly 4*4 arguments." );
            }

            return glm::mat4( glm::vec4( values[ 0 ], values[ 1 ], values[ 2 ], values[ 3 ] ),
                              glm::vec4( values[ 4 ], values[ 5 ], values[ 6 ], values[ 7 ] ),
                              glm::vec4( values[ 8 ], values[ 9 ], values[ 10 ], values[ 11 ] ),
                              glm::vec4( values[ 12 ], values[ 13 ], values[ 14 ], values[ 15 ] ) );
        }

        /**
         * Convert to string using the << operator.
         *
         * \param source the value to convert
         *
         * \return the resulting string
         */
        template< typename ValueType >
        inline std::string toString( const ValueType& source )
        {
           std::stringstream ss;
           ss << std::setprecision( 15 ) << source;
           return ss.str();
        }

        /**
         * \copydoc toString
         *
         * \note specialize to lists of numbers
         */
        template< typename ValueType >
        inline std::string toStringFromVector( const std::vector< ValueType >& source )
        {
            std::stringstream ss;
            for( auto v : source )
            {
                ss << toString( v ) << ",";
            }
            return ss.str();
        }

        /**
         * \copydoc toString
         *
         * \note glm::vec type specialization
         */
        template<>
        inline std::string toString< glm::vec2 >( const glm::vec2& source )
        {
            std::stringstream ss;
            ss << std::setprecision( 15 ) << source[ 0 ] << "," << source[ 1 ];
            return ss.str();
        }

        /**
         * \copydoc toString
         *
         * \note glm::vec type specialization
         */
        template<>
        inline std::string toString< glm::vec3 >( const glm::vec3& source )
        {
            std::stringstream ss;
            ss << std::setprecision( 15 ) << source[ 0 ] << "," << source[ 1 ] << "," << source[ 2 ];
            return ss.str();
        }

        /**
         * \copydoc toString
         *
         * \note glm::vec type specialization
         */
        template<>
        inline std::string toString< glm::vec4 >( const glm::vec4& source )
        {
            std::stringstream ss;
            ss << std::setprecision( 15 ) << source[ 0 ] << "," << source[ 1 ] << "," << source[ 2 ] << "," << source[ 3 ];
            return ss.str();
        }

        /**
         * \copydoc toString
         *
         * \note glm::mat type specialization
         */
        template<>
        inline std::string toString< glm::mat4 >( const glm::mat4& source )
        {
            std::stringstream ss;
            for( int r = 0; r < 3; ++r )
            {
                ss << toString( source[ r ] ) << ",";
            }
            ss << toString( source[ 3 ] );
            return ss.str();
        }

        /**
         * \copydoc toString
         *
         * \note glm::mat type specialization
         */
        template<>
        inline std::string toString< glm::mat3 >( const glm::mat3& source )
        {
            std::stringstream ss;
            for( int r = 0; r < 2; ++r )
            {
                ss << toString( source[ r ] ) << ",";
            }
            ss << toString( source[ 2 ] );
            return ss.str();
        }

        /**
         * \copydoc toString
         *
         * \note glm::mat type specialization
         */
        template<>
        inline std::string toString< glm::mat2 >( const glm::mat2& source )
        {
            std::stringstream ss;
            for( int r = 0; r < 1; ++r )
            {
                ss << toString( source[ r ] ) << ",";
            }
            ss << toString( source[ 1 ] );
            return ss.str();
        }
    }
}

#endif  // DI_CONVERSION_H



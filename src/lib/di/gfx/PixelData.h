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

#ifndef DI_PIXELDATA_H
#define DI_PIXELDATA_H

#include <string>
#include <vector>

#include <di/Types.h>
#include <di/MathTypes.h>

#include <di/gfx/OpenGL.h>
#include <di/gfx/GLBindable.h>

namespace di
{
    namespace core
    {
        /**
         * Class that represents plain OpenGL pixel data. For the sake of simplicity, this is always 3D. A 1D image has only a width for example.
         *
         * \ţparam ValueType the type used for each each pixel. Default is a unsigned int, 8bit, RGBA pixel. Type needs to be POD.
         */
        template< typename ValueType = glm::u8vec4 >
        class PixelData
        {
        public:
            /**
             * Construct a pixel data storage object. Define a depth of 1 for 2d images.
             *
             * \param width width of the data
             * \param height height of the data, can be 1 to denote a 1D image
             * \param depth depth of the data, can be 1 to denote a 1D or 2D image
             */
            PixelData( size_t width, size_t height = 1, size_t depth = 1 ):
                m_width( width ),
                m_height( height ),
                m_depth( depth ),
                m_pixelData( width * height * depth, ValueType() )
            {
            }

            /**
             * Destructor cleans up the data.
             */
            ~PixelData() = default;

            /**
             * Query width of the data.
             *
             * \return width
             */
            size_t getWidth() const
            {
                return m_width;
            }

            /**
             * Query height of the data.
             *
             * \return height
             */
            size_t getHeight() const
            {
                return m_height;
            }

            /**
             * Query depth of the data.
             *
             * \return depth
             */
            size_t getDepth() const
            {
                return m_depth;
            }

            /**
             * Return the plain data pointer. Useful in combination with OpenGL commands.
             *
             * \return the data pointer
             */
            void* data()
            {
                return m_pixelData.data();
            }

            /**
             * Return the plain data pointer. Useful in combination with OpenGL commands.
             *
             * \return the data pointer
             */
            const void* data() const
            {
                return m_pixelData.data();
            }

            /**
             * Get the value at the given pixel coordinate.
             *
             * \param x the x coordinate
             * \param y the y coordinate, default is 0
             * \param z the z coordinate, default is 0
             *
             * \throw range_error if the coordinates are outside the image.
             *
             * \return the pixel
             */
            const ValueType& operator()( size_t x, size_t y = 0, size_t z = 0 ) const
            {
                if( ( x >= m_width ) || ( y >= m_height ) || ( z >= m_depth ) )
                {
                    throw std::range_error( std::string( "Coordinates (" )
                                                            + std::to_string( x ) + ", "
                                                            + std::to_string( y ) + ", "
                                                            + std::to_string( z ) + ")"
                                            + std::string( " out of range [" ) + std::to_string( m_width ) + ", "
                                                              + std::to_string( m_height ) + ", "
                                                              + std::to_string( m_depth ) + "]." );
                }

                return m_pixelData[ x + y * m_width + z * m_width * m_height ];
            }

            /**
             * Get the value at the given pixel coordinate.
             *
             * \param x the x coordinate
             * \param y the y coordinate, default is 0
             * \param z the z coordinate, default is 0
             *
             * \throw range_error if the coordinates are outside the image.
             *
             * \return the pixel
             */
            ValueType& operator()( size_t x, size_t y = 0, size_t z = 0 )
            {
                if( ( x >= m_width ) || ( y >= m_height ) || ( z >= m_depth ) )
                {
                    throw std::range_error( std::string( "Coordinates (" )
                                                            + std::to_string( x ) + ", "
                                                            + std::to_string( y ) + ", "
                                                            + std::to_string( z ) + ")"
                                            + std::string( " out of range [" ) + std::to_string( m_width ) + ", "
                                                              + std::to_string( m_height ) + ", "
                                                              + std::to_string( m_depth ) + "]." );
                }

                return m_pixelData[ x + y * m_width + z * m_width * m_height ];
            }
        protected:
        private:
            /**
             * Width of the data.
             */
            size_t m_width = 0;

            /**
             * Height of the data, can be 1 for 1d images.
             */
            size_t m_height = 0;

            /**
             * Depth of the data, can be 1 for 2d and 1d images.
             */
            size_t m_depth = 0;

            /**
             * The data itself
             */
            std::vector< ValueType > m_pixelData;
        };


        /**
         * RGBA image, 8Bit per channel.
         */
        typedef core::PixelData< glm::u8vec4 > RGBA8Image;

        /**
         * RGB image, 8Bit per channel.
         */
        typedef core::PixelData< glm::u8vec3 > RGB8Image;
    }
}

#endif  // DI_PIXELDATA_H


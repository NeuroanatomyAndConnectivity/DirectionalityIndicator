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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <di/Types.h>

#include <di/gfx/OpenGL.h>
#include <di/gfx/GLBindable.h>

namespace di
{
    namespace core
    {
        /**
         * Basic class to represent a texture. It is nothing more than a representation for a piece of memory.
         */
        class Texture: public GLBindable
        {
        public:
            /**
             * Support several texture types. Find a complete list here: https://www.opengl.org/wiki/Texture
             * For now, only 1-3D textures are implemented.
             */
            enum class TextureType
            {
                Tex1D, // GL_TEXTURE_1D
                Tex2D, // GL_TEXTURE_2D
                Tex3D  // GL_TEXTURE_3D
            };

            /**
             * Create texture. Not yet filled with anything.
             *
             * \param textureType The texture type to use. Default is TextureType::Array.
             */
            explicit Texture( TextureType textureType = TextureType::Tex2D );

            /**
             * Destructor.
             */
            virtual ~Texture();

            /**
             * Create texture name. It does not bind the texture or upload any data. Specify data using \ref data.
             *
             * \return true if successful.
             */
            bool realize() override;

            /**
             * Texture wrapping modes.
             */
            enum class TextureWrap
            {
                ClampToEdge,    // GL_CLAMP_TO_EDGE
                ClampToBorder,  // GL_CLAMP_TO_BORDER
                MirroredRepeat, // GL_MIRRORED_REPEAT
                Repeat          // GL_REPEAT
            };

            /**
             * Set the wrap mode to r,s,t coordinates. Bind before use.
             *
             * \param wrap the wrap mode. Default is TextureWrap::Repeat.
             */
            void setTextureWrap( TextureWrap wrap = TextureWrap::Repeat );

            /**
             * Texture wrapping modes.
             */
            enum class TextureFilter
            {
                Nearest,              // GL_NEAREST
                Linear,               // GL_LINEAR
                NearestMipmapNearest, // GL_NEAREST_MIPMAP_NEAREST
                LinearMipmapNearest,  // GL_LINEAR_MIPMAP_NEAREST
                NearestMipmapLinear,  // GL_NEAREST_MIPMAP_LINEAR
                LinearMipmapLinear,   // GL_LINEAR_MIPMAP_LINEAR
            };

            /**
             * Set the texture filter modes for reduced sizes (min) and increased sizes (mag). Bind before use.
             *
             * \param min minimizing filter. Default is TextureFilter::Linear.
             * \param mag magnification filter. Default is TextureFilter::Linear.
             */
            void setTextureFilter( TextureFilter min = TextureFilter::Linear, TextureFilter mag = TextureFilter::Linear );

            /**
             * Bind the object.
             */
            void bind() override;

            /**
             * Commit data to this texture. If you need something special, derive and extend this. The method itself works for 1D,2D,3D textures.
             *
             * \param width width of the texture.
             * \param height height of the texture. Ignored for 1D textures.
             * \param depth depth of the texture. Ignored for non 3D textures.
             * \param internalformat defines the format internally used by GL. Default is GL_RGB.
             * \param format format of the pixel data. Default is GL_RGB.
             * \param type type of the pixel data. Default is GL_UNSIGNED_BYTE.
             * \param pixels the raw data
             */
            virtual void data( const void* pixels, size_t width, size_t height = 1, size_t depth = 1,
                               GLint internalformat = GL_RGB,
                               GLint format = GL_RGB,
                               GLenum type = GL_UNSIGNED_BYTE );
        protected:

            /**
             * Convert the given internal type to a GL enum.
             *
             * \param type the type to convert
             *
             * \return the GL enum
             */
            GLenum toGLType( const TextureType& type );

            /**
             * Map texture wrap to GL values.
             *
             * \param type the type to convert
             *
             * \return the GL enum
             */
            GLint toGLType( const TextureWrap& type );

            /**
             * Map texture filter to GL values.
             *
             * \param type the type to convert
             *
             * \return the GL enum
             */
            GLint toGLType( const TextureFilter& type );
        private:
            /**
             * The type of this texture.
             */
            TextureType m_textureType;
        };
    }
}

#endif  // TEXTURE_H


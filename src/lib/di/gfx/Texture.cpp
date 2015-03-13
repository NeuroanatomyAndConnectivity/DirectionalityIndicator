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

#include "Texture.h"

#include <di/core/Logger.h>
#define LogTag "gfx/Texture"
#include <di/gfx/GLError.h>

namespace di
{
    namespace core
    {
        Texture::Texture( Texture::TextureType textureType ):
            GLBindable(),
            m_textureType( textureType )
        {
            // Init
        }

        Texture::~Texture()
        {
            // Clean up.
            finalize();
        }

        bool Texture::realize()
        {
            if( isRealized() )
            {
                return true;
            }

            glGenTextures( 1, &m_object );
            logGLError();
            bind();

            // on multi-sample textures, this causes errors.
            if( TextureType::Tex2D_MultiSample != m_textureType )
            {
                setTextureWrap();
                setTextureFilter();
            }

            // according to the GL doc, this will never fail (when valid parameters are given, which is the case here).
            return true;
        }

        void Texture::finalize()
        {
            if( isRealized() )
            {
                glDeleteTextures( 1, &m_object );
                m_object = 0;
            }
        }

        void Texture::bind()
        {
            glBindTexture( toGLType( m_textureType ), m_object );
            logGLError();
        }

        void Texture::setTextureWrap( TextureWrap wrap )
        {
            glTexParameteri( toGLType( m_textureType ), GL_TEXTURE_WRAP_R, toGLType( wrap ) );
            glTexParameteri( toGLType( m_textureType ), GL_TEXTURE_WRAP_S, toGLType( wrap ) );
            glTexParameteri( toGLType( m_textureType ), GL_TEXTURE_WRAP_T, toGLType( wrap ) );
            logGLError();
        }

        void Texture::setTextureFilter( TextureFilter min, TextureFilter mag )
        {
            glTexParameteri( toGLType( m_textureType ), GL_TEXTURE_MAG_FILTER, toGLType( mag ) );
            glTexParameteri( toGLType( m_textureType ), GL_TEXTURE_MIN_FILTER, toGLType( min ) );
            logGLError();
        }

        void Texture::data( const void* pixels, size_t width, size_t height, size_t depth,
                               GLint internalformat,
                               GLint format,
                               GLenum type )
        {
            switch( m_textureType )
            {
                case TextureType::Tex1D:
                    glTexImage1D( GL_TEXTURE_1D, 0, internalformat, width, 0, format, type, pixels );
                    break;
                case TextureType::Tex2D:
                    glTexImage2D( GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, pixels );
                    break;
                case TextureType::Tex3D:
                    glTexImage3D( GL_TEXTURE_3D, 0, internalformat, width, height, depth, 0, format, type, pixels );
                    break;
                case TextureType::Tex2D_MultiSample:
                    LogW << "Do not use Texture::data for multi-sample textures." << LogEnd;
                    glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 1, internalformat, width, height, false );
                    break;
                default:
                    LogE << "Unknown texture type for data call." << LogEnd;
                    break;
            }
        }

        void Texture::data( size_t width, size_t height, size_t depth, size_t samples,
                               GLint internalformat,
                               GLint format,
                               GLenum type )
        {
            switch( m_textureType )
            {
                case TextureType::Tex1D:
                    glTexImage1D( GL_TEXTURE_1D, 0, internalformat, width, 0, format, type, nullptr );
                    break;
                case TextureType::Tex2D:
                    glTexImage2D( GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, nullptr );
                    break;
                case TextureType::Tex3D:
                    glTexImage3D( GL_TEXTURE_3D, 0, internalformat, width, height, depth, 0, format, type, nullptr );
                    break;
                case TextureType::Tex2D_MultiSample:
                    glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, samples, internalformat, width, height, false );
                    break;
                default:
                    LogE << "Unknown texture type for data call." << LogEnd;
                    break;
            }
            logGLError();
        }

        void Texture::read( void* pixels, GLint format, GLenum type ) const
        {
            glGetTexImage( toGLType( m_textureType ), 0, format, type, pixels );
            logGLError();
        }

        GLenum Texture::toGLType( const TextureType& type ) const
        {
            switch( type )
            {
                case TextureType::Tex1D:
                    return GL_TEXTURE_1D;
                case TextureType::Tex2D:
                    return GL_TEXTURE_2D;
                case TextureType::Tex3D:
                    return GL_TEXTURE_3D;
                case TextureType::Tex2D_MultiSample:
                    return GL_TEXTURE_2D_MULTISAMPLE;
                default:
                    return -1;
            }
        }

        GLint Texture::toGLType( const TextureFilter& type ) const
        {
            switch( type )
            {
                case TextureFilter::Nearest:
                    return GL_NEAREST;
                case TextureFilter::Linear:
                    return GL_LINEAR;
                case TextureFilter::NearestMipmapNearest:
                    return GL_NEAREST_MIPMAP_NEAREST;
                case TextureFilter::LinearMipmapNearest:
                    return GL_LINEAR_MIPMAP_NEAREST;
                case TextureFilter::NearestMipmapLinear:
                    return GL_NEAREST_MIPMAP_LINEAR;
                case TextureFilter::LinearMipmapLinear:
                    return GL_LINEAR_MIPMAP_LINEAR;
                default:
                    return -1;
            }
        }

        GLint Texture::toGLType( const TextureWrap& type ) const
        {
            switch( type )
            {
                case TextureWrap::ClampToEdge:
                    return GL_CLAMP_TO_EDGE;
                case TextureWrap::ClampToBorder:
                    return GL_CLAMP_TO_BORDER;
                case TextureWrap::MirroredRepeat:
                    return GL_MIRRORED_REPEAT;
                case TextureWrap::Repeat:
                    return GL_REPEAT;
                default:
                    return -1;
            }
        }

    }
}



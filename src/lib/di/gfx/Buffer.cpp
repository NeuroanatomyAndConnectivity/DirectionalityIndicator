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

#include "Buffer.h"

#include <di/core/Logger.h>
#define LogTag "gfx/Buffer"
#include <di/gfx/GLError.h>

namespace di
{
    namespace core
    {
        Buffer::Buffer( Buffer::BufferType bufferType ):
            GLBindable(),
            m_bufferType( bufferType )
        {
            // Init
        }

        Buffer::~Buffer()
        {
            // Clean up.
            finalize();
        }

        bool Buffer::realize()
        {
            if( isRealized() )
            {
                return true;
            }

            // Create the buffer
            logGLError();
            glGenBuffers( 1, &m_object );
            logGLError();

            // according to the GL doc, this will never fail (when valid parameters are given, which is the case here).
            return true;
        }


        void Buffer::finalize()
        {
            if( isRealized() )
            {
                glDeleteBuffers( 1, &m_object );
                m_object = 0;
            }
        }

        void Buffer::bind()
        {
            glBindBuffer( toGLType( m_bufferType ), m_object );
            logGLError();
        }

        void Buffer::data( size_t size, const void* ptr )
        {
            // feed the buffer, and let OpenGL know that we don't plan to
            // change it (STATIC) and that it will be used for drawing (DRAW)
            glBufferData( toGLType( m_bufferType ), size, ptr, GL_STATIC_DRAW );
            logGLError();
        }

        GLenum Buffer::toGLType( const BufferType& type )
        {
            switch( type )
            {
                case BufferType::Array:
                    return GL_ARRAY_BUFFER;
                case BufferType::ElementArray:
                    return GL_ELEMENT_ARRAY_BUFFER;
                default:
                    return -1;
            }
        }
    }
}



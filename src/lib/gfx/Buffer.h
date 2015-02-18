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

#ifndef BUFFER_H
#define BUFFER_H
#include <iostream>

#include "Types.h"

#include "OpenGL.h"
#include "GLBindable.h"

namespace di
{
    namespace core
    {
        /**
         * Basic class to represent a buffer. It is nothing more than a representation for a piece of memory.
         */
        class Buffer: public GLBindable
        {
        public:
            /**
             * Support several buffer types. Find a complete list here:  https://www.opengl.org/sdk/docs/man/html/glBindBuffer.xhtml
             * For now, only array buffers are implemented.
             */
            enum class BufferType
            {
                Array,       // OpenGL: GL_ARRAY_BUFFER
                ElementArray // OpenGL: GL_ELEMENT_ARRAY_BUFFER
            };

            /**
             * Create buffer. Not yet filled with anything.
             *
             * \param bufferType The buffer type to use. Default is BufferType::Array.
             */
            explicit Buffer( BufferType bufferType = BufferType::Array );

            /**
             * Destructor.
             */
            virtual ~Buffer();

            /**
             * Create buffer name. It does not bind the buffer or upload any data. Specify data using \ref data.
             *
             * \return true if successful.
             */
            bool realize() override;

            /**
             * Bind the object.
             */
            void bind() override;

            /**
             * Commit data to this buffer. The source needs to be something that provides the following members "size", "data", and the typedef
             * "value_type". This is true for the widely used std::vector.
             *
             * \tparam Container
             * \param container
             */
            template< typename Container >
            void data( const Container& container )
            {
                data( sizeof( typename Container::value_type ) * container.size(), container.data() );
            }

            /**
             * Commit data to this buffer. The source needs to be something that provides the following members "size", "data", and the typedef
             * "value_type". This is true for the widely used std::vector.
             *
             * \tparam Container
             * \param container
             */
            template< typename Container >
            void data( ConstSPtr< Container > container )
            {
                data( sizeof( typename Container::value_type ) * container->size(), container->data() );
            }

            /**
             * Commit data to this buffer. The source needs to be something that provides the following members "size", "data", and the typedef
             * "value_type". This is true for the widely used std::vector.
             *
             * \tparam Container
             * \param container
             */
            template< typename Container >
            void data( Container* container )
            {
                data( sizeof( typename Container::value_type ) * container->size(), container->data() );
            }

            /**
             * Commit data to this buffer.
             *
             * \param size the size of the buffer
             * \param ptr the data pointer.
             */
            virtual void data( size_t size, const void* ptr );
        protected:
            /**
             * Convert the given internal type to a GL enum.
             *
             * \param type the type to convert
             *
             * \return the GL enum
             */
            GLenum toGLType( const BufferType& type );

        private:
            /**
             * The type of this buffer.
             */
            BufferType m_bufferType;
        };
    }
}

#endif  // BUFFER_H


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

#include <string>
#include <vector>

#define LogTag "algorithms/DirectionalityVisualization"
#include "core/Logger.h"

#include "core/data/TriangleDataSet.h"
#include "core/Filesystem.h"

#include "gfx/GL.h"
#include "gfx/GLError.h"

#include "DirectionalityVisualization.h"

namespace di
{
    namespace algorithms
    {
        DirectionalityVisualization::DirectionalityVisualization():
            Algorithm( "Directionality Visualization",
                       "This algorithm takes a triangle mesh and scalar information defined on the mesh. "
                       "It the creates directional information from the scalar data and displays it on "
                       "the mesh itself visually." ),
            Visualization()
        {
            // We require some inputs.

            // 1: the triangle mesh
            m_triangleDataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle mesh on which the directionality information should be shown."
            );
        }

        DirectionalityVisualization::~DirectionalityVisualization()
        {
            // nothing to clean up so far
        }

        void DirectionalityVisualization::process()
        {
            // Get input data
            auto data = m_triangleDataInput->getData();
            LogD << "Got data instance " << static_cast< const void* >( data.get() ) << "." << LogEnd;

            // Provide the needed information to the visualizer itself.
            bool changeVis = ( m_visTriangleData != data );
            m_visTriangleData = data;

            // As the rendering system does not render permanently, inform about the update.
            if( changeVis )
            {
                renderRequest();
            }
        }

        core::BoundingBox DirectionalityVisualization::getBoundingBox() const
        {
            if( m_visTriangleData )
            {
                return m_visTriangleData->getGrid()->getBoundingBox();
            }
            else
            {
                return core::BoundingBox();
            }
        }

        void DirectionalityVisualization::prepare()
        {
            LogD << "Vis Prepare" << LogEnd;

            std::string localShaderPath = core::getRuntimePath() + "/algorithms/shaders/";
            m_vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "MeshRender-vertex.glsl" ) );
            m_fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "MeshRender-fragment.glsl" ) );

            // Link them to build the program itself
            // m_shaderProgram = std::make_shared< di::core::Program >( { m_vertexShader, m_fragmentShader } );
            // NOTE: the above code does not compile on CLang.
            m_shaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            m_vertexShader,
                            m_fragmentShader
                        }
            ) );
            m_shaderProgram->realize();
       }

        void DirectionalityVisualization::finalize()
        {
            LogD << "Vis Finalize" << LogEnd;
        }

        void DirectionalityVisualization::render( const core::View& view )
        {
            if( !( m_VAO && m_triVBO && m_triIBO && m_shaderProgram ) )
            {
                return;
            }
            // LogD << "Vis Render" << LogEnd;

            m_shaderProgram->bind();
            m_shaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            m_shaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );

            glBindVertexArray( m_VAO );
            glDrawElements( GL_TRIANGLES, m_visTriangleData->getGrid()->getTriangles().size() * 3, GL_UNSIGNED_INT, NULL );
        }

        void DirectionalityVisualization::update()
        {
            // Be warned: this method is huge. I did not yet use a VAO and VBO abstraction. This causes the code to be quite long. But I structured it
            // and many code parts repeat again and again.

            if( !m_visTriangleData )
            {
                return;
            }

            if( !isRenderingRequested() )
            {
                return;
            }
            LogD << "Vis Update" << LogEnd;
            resetRenderingRequest();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create VAO and VBO for the mesh itself
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // get the location of attribute "position" in program
            GLint vertexLoc = m_shaderProgram->getAttribLocation( "position" );
            GLint colorLoc = m_shaderProgram->getAttribLocation( "color" );
            GLint normalLoc = m_shaderProgram->getAttribLocation( "normal" );

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // VBO: Vertices

            // Create the triangle vertex buffer
            glGenBuffers( 1, &m_triVBO );

            // bind buffer for positions and copy data into buffer
            // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
            glBindBuffer( GL_ARRAY_BUFFER, m_triVBO );

            // feed the buffer, and let OpenGL know that we don't plan to
            // change it (STATIC) and that it will be used for drawing (DRAW)
            glBufferData( GL_ARRAY_BUFFER, sizeof( glm::vec3::value_type ) * 3 * m_visTriangleData->getGrid()->getVertices().size(),
                                           m_visTriangleData->getGrid()->getVertices().data(),
                                           GL_STATIC_DRAW );

            // Enable the attribute at that location
            glEnableVertexAttribArray( vertexLoc );

            // Tell OpenGL what the array contains:
            // it is a set of 3 floats for each vertex
            glVertexAttribPointer( vertexLoc, 3, GL_FLOAT, 0, 0, 0 );

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // VBO: Colors

            // Create the triangle color buffer
            glGenBuffers( 1, &m_colorVBO );

            // bind buffer for positions and copy data into buffer
            // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
            glBindBuffer( GL_ARRAY_BUFFER, m_colorVBO );

            // feed the buffer, and let OpenGL know that we don't plan to
            // change it (STATIC) and that it will be used for drawing (DRAW)
            glBufferData( GL_ARRAY_BUFFER, sizeof( RGBAArray::value_type::value_type ) * 4 * m_visTriangleData->getAttributes()->size(),
                                           m_visTriangleData->getAttributes()->data(),
                                           GL_STATIC_DRAW );

            // Enable the attribute at that location
            glEnableVertexAttribArray( colorLoc );

            // Tell OpenGL what the array contains:
            // it is a set of 3 floats for each vertex
            glVertexAttribPointer( colorLoc, 4, GL_FLOAT, 0, 0, 0 );

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // VBO: Normals

            // Create the triangle color buffer
            glGenBuffers( 1, &m_normalVBO );

            // bind buffer for positions and copy data into buffer
            // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
            glBindBuffer( GL_ARRAY_BUFFER, m_normalVBO );

            // feed the buffer, and let OpenGL know that we don't plan to
            // change it (STATIC) and that it will be used for drawing (DRAW)
            glBufferData( GL_ARRAY_BUFFER, sizeof( NormalArray::value_type::value_type ) * 3 * m_visTriangleData->getGrid()->getNormals().size(),
                                           m_visTriangleData->getGrid()->getNormals().data(),
                                           GL_STATIC_DRAW );

            // Enable the attribute at that location
            glEnableVertexAttribArray( normalLoc );

            // Tell OpenGL what the array contains:
            // it is a set of 3 floats for each vertex
            glVertexAttribPointer( normalLoc, 3, GL_FLOAT, 0, 0, 0 );

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // IBO: Indices

            glGenBuffers( 1, &m_triIBO );

            // bind buffer for positions and copy data into buffer
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_triIBO );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( glm::ivec3::value_type ) * 3 * m_visTriangleData->getGrid()->getTriangles().size(),
                                                   m_visTriangleData->getGrid()->getTriangles().data(),
                                                   GL_STATIC_DRAW );


        }
    }
}


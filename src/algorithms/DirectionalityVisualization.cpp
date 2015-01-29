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
            if( !( m_VAO && m_shaderProgram && m_vertexBuffer ) )
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

        void DirectionalityVisualization::update( const core::View& view )
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
            // Create Vertex Array Object VAO and the corresponding Vertex Buffer Objects VBO for the mesh itself
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // get the location of attribute "position" in program
            GLint vertexLoc = m_shaderProgram->getAttribLocation( "position" );
            GLint colorLoc = m_shaderProgram->getAttribLocation( "color" );
            GLint normalLoc = m_shaderProgram->getAttribLocation( "normal" );

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );

            // Create some buffers
            m_vertexBuffer = std::make_shared< core::Buffer >();
            m_normalBuffer = std::make_shared< core::Buffer >();
            m_colorBuffer = std::make_shared< core::Buffer >();
            m_indexBuffer = std::make_shared< core::Buffer >( core::Buffer::BufferType::ElementArray );

            // Set the data using the triangle mesh. Also set the location mapping of the shader using the VAO
            m_vertexBuffer->realize();
            m_vertexBuffer->bind();
            m_vertexBuffer->data( m_visTriangleData->getGrid()->getVertices() );

            glEnableVertexAttribArray( vertexLoc );
            glVertexAttribPointer( vertexLoc, 3, GL_FLOAT, 0, 0, 0 );

            m_colorBuffer->realize();
            m_colorBuffer->bind();
            m_colorBuffer->data( m_visTriangleData->getAttributes() );
            glEnableVertexAttribArray( colorLoc );
            glVertexAttribPointer( colorLoc, 4, GL_FLOAT, 0, 0, 0 );

            m_normalBuffer->realize();
            m_normalBuffer->bind();
            m_normalBuffer->data( m_visTriangleData->getGrid()->getNormals() );
            glEnableVertexAttribArray( normalLoc );
            glVertexAttribPointer( normalLoc, 3, GL_FLOAT, 0, 0, 0 );

            m_indexBuffer->realize();
            m_indexBuffer->bind();
            m_indexBuffer->data( m_visTriangleData->getGrid()->getTriangles() );

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create a Framebuffer Object (FBO)
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // The framebuffer
            // glGenFramebuffers( 1, & m_fbo );

            // Bind it to be able to modify and configure:
            // glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );

            // Create some textures for LIC. 3



        }
    }
}


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

#include <di/core/data/LineDataSet.h>
#include <di/core/Filesystem.h>

#include <di/gfx/GL.h>
#include <di/gfx/GLError.h>

#include "RenderLines.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/RenderLines"

namespace di
{
    namespace algorithms
    {
        RenderLines::RenderLines():
            Algorithm( "Render Lines",
                       "This algorithm takes a bunch of lines and renders it to screen." ),
            Visualization()
        {
            // We require some inputs.

            // 1: the triangle mesh
            m_lineDataInput = addInput< di::core::LineDataSet >(
                    "Lines",
                    "The lines to render."
            );
        }

        RenderLines::~RenderLines()
        {
            // nothing to clean up so far
        }

        void RenderLines::process()
        {
            // Get input data
            auto data = m_lineDataInput->getData();

            LogD << "Got " << data->getGrid()->getNumLines() << " lines with " << data->getGrid()->getNumVertices() << " vertices." << LogEnd;

            // Provide the needed information to the visualizer itself.
            bool changeVis = ( m_visLineData != data );
            m_visLineData = data;

            // As the rendering system does not render permanently, inform about the update.
            if( changeVis )
            {
                renderRequest();
            }
        }

        core::BoundingBox RenderLines::getBoundingBox() const
        {
            if( m_visLineData )
            {
                return m_visLineData->getGrid()->getBoundingBox();
            }
            else
            {
                return core::BoundingBox();
            }
        }

        void RenderLines::prepare()
        {
            LogD << "Vis Prepare" << LogEnd;

            SPtr< di::core::Shader > vertexShader = nullptr;
            SPtr< di::core::Shader > fragmentShader = nullptr;

            std::string localShaderPath = core::getResourcePath() + "/algorithms/shaders/";
            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "RenderLines-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "RenderLines-fragment.glsl" ) );

            // Link them to build the program itself
            // m_shaderProgram = std::make_shared< di::core::Program >( { m_vertexShader, m_fragmentShader } );
            // NOTE: the above code does not compile on CLang.
            m_shaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            vertexShader,
                            fragmentShader
                        }
            ) );
            m_shaderProgram->realize();
        }

        void RenderLines::finalize()
        {
            LogD << "Vis Finalize" << LogEnd;
        }

        void RenderLines::render( const core::View& view )
        {
            if( !( m_VAO && m_shaderProgram && m_vertexBuffer ) )
            {
                return;
            }
            // LogD << "Vis Render" << LogEnd;

            m_shaderProgram->bind();
            m_shaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            m_shaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );
            logGLError();

            glEnable( GL_BLEND );

            glBindVertexArray( m_VAO );
            glDrawElements( GL_LINES, m_visLineData->getGrid()->getLines().size() * 2, GL_UNSIGNED_INT, NULL );

            glDisable(  GL_BLEND );
            logGLError();
        }

        void RenderLines::update( const core::View& /* view */, bool reload )
        {
            // Be warned: this method is huge. I did not yet use a VAO and VBO abstraction. This causes the code to be quite long. But I structured it
            // and many code parts repeat again and again.

            if( !m_visLineData )
            {
                return;
            }

            if( !isRenderingRequested() && !reload )
            {
                return;
            }
            LogD << "Vis Update" << LogEnd;
            resetRenderingRequest();

            prepare();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create Vertex Array Object VAO and the corresponding Vertex Buffer Objects VBO for the mesh itself
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            LogD << "Creating Mesh VAO" << LogEnd;

            m_shaderProgram->bind();
            logGLError();

            // get the location of attribute "position" in program
            GLint vertexLoc = m_shaderProgram->getAttribLocation( "position" );
            GLint colorLoc = m_shaderProgram->getAttribLocation( "color" );
            logGLError();

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );
            logGLError();

            // Create some buffers
            m_vertexBuffer = std::make_shared< core::Buffer >();
            m_colorBuffer = std::make_shared< core::Buffer >();
            m_indexBuffer = std::make_shared< core::Buffer >( core::Buffer::BufferType::ElementArray );
            logGLError();

            // Set the data using the triangle mesh. Also set the location mapping of the shader using the VAO
            m_vertexBuffer->realize();
            m_vertexBuffer->bind();
            m_vertexBuffer->data( m_visLineData->getGrid()->getVertices() );
            logGLError();

            glEnableVertexAttribArray( vertexLoc );
            glVertexAttribPointer( vertexLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_colorBuffer->realize();
            m_colorBuffer->bind();
            m_colorBuffer->data( m_visLineData->getAttributes() );
            glEnableVertexAttribArray( colorLoc );
            glVertexAttribPointer( colorLoc, 4, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_indexBuffer->realize();
            m_indexBuffer->bind();
            m_indexBuffer->data( m_visLineData->getGrid()->getLines() );
            logGLError();
        }
    }
}


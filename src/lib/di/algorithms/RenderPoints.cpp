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

#define LogTag "algorithms/RenderPoints"
#include <di/core/Logger.h>

#include <di/core/data/PointDataSet.h>
#include <di/core/Filesystem.h>

#include <di/gfx/GL.h>
#include <di/gfx/GLError.h>

#include "RenderPoints.h"

namespace di
{
    namespace algorithms
    {
        RenderPoints::RenderPoints():
            Algorithm( "Render Points",
                       "This algorithm takes a bunch of points and renders it to screen." ),
            Visualization()
        {
            // We require some inputs.

            // 1: the triangle mesh
            m_pointDataInput = addInput< di::core::PointDataSet >(
                    "Points",
                    "The points to render."
            );
        }

        RenderPoints::~RenderPoints()
        {
            // nothing to clean up so far
        }

        void RenderPoints::process()
        {
            // Get input data
            auto data = m_pointDataInput->getData();

            LogD << "Got " << data->getGrid()->getNumVertices() << " points." << LogEnd;

            // Provide the needed information to the visualizer itself.
            bool changeVis = ( m_visPointData != data );
            m_visPointData = data;

            // As the rendering system does not render permanently, inform about the update.
            if( changeVis )
            {
                renderRequest();
            }
        }

        core::BoundingBox RenderPoints::getBoundingBox() const
        {
            if( m_visPointData )
            {
                return m_visPointData->getGrid()->getBoundingBox();
            }
            else
            {
                return core::BoundingBox();
            }
        }

        void RenderPoints::prepare()
        {
            LogD << "Vis Prepare" << LogEnd;

            SPtr< di::core::Shader > vertexShader = nullptr;
            SPtr< di::core::Shader > fragmentShader = nullptr;

            std::string localShaderPath = core::getResourcePath() + "/algorithms/shaders/";
            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "RenderPoints-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "RenderPoints-fragment.glsl" ) );

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

        void RenderPoints::finalize()
        {
            LogD << "Vis Finalize" << LogEnd;
        }

        void RenderPoints::render( const core::View& view )
        {
            if( !( m_VAO && m_shaderProgram && m_vertexBuffer ) )
            {
                return;
            }
            // LogD << "Vis Render" << LogEnd;

            m_shaderProgram->bind();
            m_shaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            m_shaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );

            glPointSize( 3.0 );

            logGLError();

            glEnable( GL_BLEND );

            glBindVertexArray( m_VAO );
            glDrawArrays( GL_POINTS, 0, m_visPointData->getGrid()->getVertices().size() );

            glDisable(  GL_BLEND );
            logGLError();
        }

        void RenderPoints::update( const core::View& /* view */, bool reload )
        {
            // Be warned: this method is huge. I did not yet use a VAO and VBO abstraction. This causes the code to be quite long. But I structured it
            // and many code parts repeat again and again.

            if( !m_visPointData )
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
            // m_indexBuffer = std::make_shared< core::Buffer >( core::Buffer::BufferType::ElementArray );
            logGLError();

            // Set the data using the triangle mesh. Also set the location mapping of the shader using the VAO
            m_vertexBuffer->realize();
            m_vertexBuffer->bind();
            m_vertexBuffer->data( m_visPointData->getGrid()->getVertices() );
            logGLError();

            glEnableVertexAttribArray( vertexLoc );
            glVertexAttribPointer( vertexLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_colorBuffer->realize();
            m_colorBuffer->bind();
            m_colorBuffer->data( m_visPointData->getAttributes() );
            glEnableVertexAttribArray( colorLoc );
            glVertexAttribPointer( colorLoc, 4, GL_FLOAT, 0, 0, 0 );
            logGLError();

            /*
            m_indexBuffer->realize();
            m_indexBuffer->bind();
            m_indexBuffer->data( m_visPointData->getGrid()->getLines() );
            logGLError();*/
        }
    }
}


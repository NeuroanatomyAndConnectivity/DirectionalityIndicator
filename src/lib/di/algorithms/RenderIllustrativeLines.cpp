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

#include <di/core/data/TriangleDataSet.h>
#include <di/core/data/Points.h>
#include <di/core/Filesystem.h>

#include <di/gfx/GL.h>
#include <di/gfx/GLError.h>

#include "RenderIllustrativeLines.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/RenderIllustrativeLines"

namespace di
{
    namespace algorithms
    {
        RenderIllustrativeLines::RenderIllustrativeLines():
            Algorithm( "Render Illustrative Lines",
                       "This algorithm takes a bunch of lines and renders it to screen." ),
            Visualization()
        {
            // We require some inputs.

            // 1: the triangle mesh
            m_triangleDataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle mesh on which the directionality information should be shown."
            );
        }

        RenderIllustrativeLines::~RenderIllustrativeLines()
        {
            // nothing to clean up so far
        }

        void RenderIllustrativeLines::process()
        {
            // Get input data
            auto data = m_triangleDataInput->getData();

            // Provide the needed information to the visualizer itself.
            bool changeVis = ( m_visTriangleData != data );
            m_visTriangleData = data;

            // As the rendering system does not render permanently, inform about the update.
            if( changeVis )
            {
                renderRequest();
            }
        }

        core::BoundingBox RenderIllustrativeLines::getBoundingBox() const
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

        void RenderIllustrativeLines::prepare()
        {
            LogD << "Vis Prepare" << LogEnd;

            std::string localShaderPath = core::getResourcePath() + "/algorithms/shaders/";

            // Transformation Stage
            auto transformVertex = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                                     core::readTextFile(
                                                                         localShaderPath + "RenderIllustrativeLines-Transform-vertex.glsl" ) );
            auto transformFragment = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                       core::readTextFile(
                                                                           localShaderPath + "RenderIllustrativeLines-Transform-fragment.glsl" ) );

            // Link them to build the program itself
            m_transformShaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            transformVertex,
                            transformFragment,
                            std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                              core::readTextFile( localShaderPath + "Shading.glsl" ) )
                        }
            ) );
            m_transformShaderProgram->realize();

            SPtr< di::core::Shader > vertexShader = nullptr;
            SPtr< di::core::Shader > fragmentShader = nullptr;
            SPtr< di::core::Shader > geometryShader = nullptr;

            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "RenderIllustrativeLines-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "RenderIllustrativeLines-fragment.glsl" ) );
            geometryShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Geometry,
                                                                 core::readTextFile( localShaderPath + "RenderIllustrativeLines-geometry.glsl" ) );

            // Link them to build the program itself
            // m_shaderProgram = std::make_shared< di::core::Program >( { m_vertexShader, m_fragmentShader } );
            // NOTE: the above code does not compile on CLang.
            m_shaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            vertexShader,
                            fragmentShader,
                            geometryShader,
                            std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                              core::readTextFile( localShaderPath + "Shading.glsl" ) )
                        }
            ) );
            m_shaderProgram->realize();
        }

        void RenderIllustrativeLines::finalize()
        {
            LogD << "Vis Finalize" << LogEnd;
        }

        void RenderIllustrativeLines::render( const core::View& view )
        {
            if( !( m_VAO && m_transformShaderProgram && m_vertexBuffer ) )
            {
                return;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 1 - Draw Color and Noise on geometry to textures:

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboTransform );

            m_transformShaderProgram->bind();
            m_transformShaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            m_transformShaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );
            logGLError();

            // NOTE: keep original Viewport
            // glViewport( 0, 0, view.getViewportSize().x, view.getViewportSize().y );
            logGLError();
            GLenum drawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers( 4, drawBuffers );
            logGLError();

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glClearColor( 1.0f, 0.0f, 0.0f, .0f );

            glBindVertexArray( m_VAO );
            glDrawElements( GL_TRIANGLES, m_visTriangleData->getGrid()->getTriangles().size() * 3, GL_UNSIGNED_INT, NULL );
            logGLError();

           ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Final Step - Merge everything and output to the normal framebuffer:

            // Set the view to be the target.
            view.bind();

            // draw a big quad
            m_shaderProgram->bind();
            m_shaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            m_shaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );
            // m_shaderProgram->setUniform( "u_viewportSize", view.getViewportSize() );
            m_shaderProgram->setUniform( "u_viewportScale", view.getViewportSize() / glm::vec2( 2048, 2048 ) );

            logGLError();

            glActiveTexture( GL_TEXTURE0 );
            m_step1ColorTex->bind();
            glActiveTexture( GL_TEXTURE1 );
            m_step1VecTex->bind();
            glActiveTexture( GL_TEXTURE2 );
            m_step1NormalTex->bind();
            glActiveTexture( GL_TEXTURE3 );
            m_step1PosTex->bind();
            glActiveTexture( GL_TEXTURE4 );
            m_step1DepthTex->bind();

            glBindVertexArray( m_pointVAO );
            glDrawArrays( GL_POINTS, 0, m_points->getVertices().size() );

            logGLError();
        }

        void RenderIllustrativeLines::update( const core::View& /* view */, bool reload )
        {
            // Be warned: this method is huge. I did not yet use a VAO and VBO abstraction. This causes the code to be quite long. But I structured it
            // and many code parts repeat again and again.

            if( !m_visTriangleData )
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

            m_transformShaderProgram->bind();
            logGLError();

            // get the location of attribute "position" in program
            GLint vertexLoc = m_transformShaderProgram->getAttribLocation( "position" );
            GLint colorLoc = m_transformShaderProgram->getAttribLocation( "color" );
            GLint normalLoc = m_transformShaderProgram->getAttribLocation( "normal" );
            logGLError();

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );
            logGLError();

            // Create some buffers
            m_vertexBuffer = std::make_shared< core::Buffer >();
            m_normalBuffer = std::make_shared< core::Buffer >();
            m_colorBuffer = std::make_shared< core::Buffer >();
            m_indexBuffer = std::make_shared< core::Buffer >( core::Buffer::BufferType::ElementArray );
            logGLError();

            // Set the data using the triangle mesh. Also set the location mapping of the shader using the VAO
            m_vertexBuffer->realize();
            m_vertexBuffer->bind();
            m_vertexBuffer->data( m_visTriangleData->getGrid()->getVertices() );
            logGLError();

            glEnableVertexAttribArray( vertexLoc );
            glVertexAttribPointer( vertexLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_colorBuffer->realize();
            m_colorBuffer->bind();
            m_colorBuffer->data( m_visTriangleData->getAttributes() );
            glEnableVertexAttribArray( colorLoc );
            glVertexAttribPointer( colorLoc, 4, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_normalBuffer->realize();
            m_normalBuffer->bind();
            m_normalBuffer->data( m_visTriangleData->getGrid()->getNormals() );
            glEnableVertexAttribArray( normalLoc );
            glVertexAttribPointer( normalLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_indexBuffer->realize();
            m_indexBuffer->bind();
            m_indexBuffer->data( m_visTriangleData->getGrid()->getTriangles() );
            logGLError();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create Vertex Array Object VAO and the corresponding Vertex Buffer Objects VBO for the mesh itself
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            LogD << "Creating Point VAO" << LogEnd;


            // create regular grid of points
            m_points = std::make_shared< di::core::Points >();

            const size_t xSize = 25;
            const size_t ySize = 25;
            for( size_t y = 0; y < ySize; ++y )
            {
                for( size_t x = 0; x < xSize; ++x )
                {
                    m_points->addVertex( static_cast< float >( x ) / static_cast< float >( xSize ),
                                         static_cast< float >( y ) / static_cast< float >( ySize ),
                                         0.0
                                       );
                }
            }

            m_shaderProgram->bind();
            logGLError();

            // get the location of attribute "position" in program
            GLint vertexPointLoc = m_shaderProgram->getAttribLocation( "position" );
            logGLError();

            // Create the VAO
            glGenVertexArrays( 1, &m_pointVAO );
            glBindVertexArray( m_pointVAO );
            logGLError();

            // Create some buffers
            m_vertexBuffer = std::make_shared< core::Buffer >();
            logGLError();

            // Set the data using the triangle mesh. Also set the location mapping of the shader using the VAO
            m_vertexBuffer->realize();
            m_vertexBuffer->bind();
            m_vertexBuffer->data( m_points->getVertices() );
            logGLError();

            glEnableVertexAttribArray( vertexLoc );
            glVertexAttribPointer( vertexPointLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create a Framebuffer Object (FBO) and setup LIC pipeline
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 1: Render and transform to image space
            LogD << "Creating Transform Pass FBO" << LogEnd;

            // The framebuffer
            glGenFramebuffers( 1, & m_fboTransform );

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboTransform );
            logGLError();

            // We need two target texture: color and noise
            m_step1ColorTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1ColorTex->realize();
            m_step1ColorTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            // TODO(sebastian): fixed size textures are a problem ...
            m_step1ColorTex->data( nullptr, 2048, 2048, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
            logGLError();

            m_step1VecTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1VecTex->realize();
            m_step1VecTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1VecTex->data( nullptr, 2048, 2048, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT );
            logGLError();

            m_step1NormalTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1NormalTex->realize();
            m_step1NormalTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1NormalTex->data( nullptr, 2048, 2048, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT );
            logGLError();

            m_step1PosTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1PosTex->realize();
            m_step1PosTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1PosTex->data( nullptr, 2048, 2048, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT );
            logGLError();

            m_step1DepthTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1DepthTex->realize();
            m_step1DepthTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1DepthTex->data( nullptr, 2048, 2048, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT );
            m_step1DepthTex->setTextureFilter( core::Texture::TextureFilter::LinearMipmapLinear, core::Texture::TextureFilter::Linear );
            logGLError();

            // Bind textures to FBO
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_step1ColorTex->getObjectID() , 0 );
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_step1VecTex->getObjectID() , 0 );
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_step1NormalTex->getObjectID() , 0 );
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, m_step1PosTex->getObjectID() , 0 );
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  m_step1DepthTex->getObjectID() , 0 );
            logGLError();

            // Define the out vars to bind to the attachments
            glBindFragDataLocation( m_transformShaderProgram->getObjectID(), 0, "fragColor" );
            logGLError();
            glBindFragDataLocation( m_transformShaderProgram->getObjectID(), 1, "fragVec" );
            logGLError();
            glBindFragDataLocation( m_transformShaderProgram->getObjectID(), 2, "fragNormal" );
            logGLError();
            glBindFragDataLocation( m_transformShaderProgram->getObjectID(), 3, "fragPos" );
            logGLError();

            // Allow the next shader to access them
            m_shaderProgram->setUniform( "u_colorSampler",  0 );
            m_shaderProgram->setUniform( "u_vecSampler",    1 );
            m_shaderProgram->setUniform( "u_normalSampler", 2 );
            m_shaderProgram->setUniform( "u_posSampler",    3 );
            m_shaderProgram->setUniform( "u_depthSampler",  4 );

            // Final Check
            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed for Step 1." << LogEnd;
            }


































































            /*
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create Vertex Array Object VAO and the corresponding Vertex Buffer Objects VBO for the mesh itself
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            LogD << "Creating Mesh VAO" << LogEnd;

            m_shaderProgram->bind();
            logGLError();

            // get the location of attribute "position" in program
            GLint vertexLoc = m_shaderProgram->getAttribLocation( "position" );
            GLint colorLoc = m_shaderProgram->getAttribLocation( "color" );
            GLint normalLoc = m_shaderProgram->getAttribLocation( "normal" );
            logGLError();

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );
            logGLError();

            // Create some buffers
            m_vertexBuffer = std::make_shared< core::Buffer >();
            m_colorBuffer = std::make_shared< core::Buffer >();
            m_normalBuffer = std::make_shared< core::Buffer >();
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

            m_normalBuffer->realize();
            m_normalBuffer->bind();
            m_normalBuffer->data( m_visLineData->getAttributes< 1 >() );
            glEnableVertexAttribArray( normalLoc );
            glVertexAttribPointer( normalLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_indexBuffer->realize();
            m_indexBuffer->bind();
            m_indexBuffer->data( m_visLineData->getGrid()->getLines() );
            logGLError();
            */
        }
    }
}


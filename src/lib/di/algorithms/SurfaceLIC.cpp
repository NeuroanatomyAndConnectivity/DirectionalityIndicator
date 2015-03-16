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
#include <di/core/Filesystem.h>

#include <di/gfx/GL.h>
#include <di/gfx/GLError.h>

#include "SurfaceLIC.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/SurfaceLIC"

namespace di
{
    namespace algorithms
    {
        SurfaceLIC::SurfaceLIC():
            Algorithm( "Surface LIC",
                       "This algorithm takes a triangle mesh and scalar information defined on the mesh. "
                       "It creates a smearing pattern along the gradient information." ),
            Visualization()
        {
            // We require some inputs.

            // 1: the triangle mesh
            m_triangleDataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle mesh on which the directionality information should be shown."
            );

            m_vectorInput = addInput< di::core::TriangleVectorField >(
                    "Directions",
                    "Directional information on the triangle mesh"
            );
        }

        SurfaceLIC::~SurfaceLIC()
        {
            // nothing to clean up so far
        }

        void SurfaceLIC::process()
        {
            // Get input data
            auto data = m_triangleDataInput->getData();
            auto vectors = m_vectorInput->getData();

            // only valid if the grids match
            if( data && vectors )
            {
                if( data->getGrid() != vectors->getGrid() )
                {
                    LogD << "Grids do not match. Ignoring new data." << LogEnd;
                    data = nullptr;
                    vectors = nullptr;
                }
            }
            else
            {
                data = nullptr;
                vectors = nullptr;
            }

            // Provide the needed information to the visualizer itself.
            bool changeVis = ( m_visTriangleData != data ) || ( m_visTriangleVectorData != vectors );
            m_visTriangleData = data;
            m_visTriangleVectorData = vectors;

            // As the rendering system does not render permanently, inform about the update.
            if( changeVis )
            {
                LogD << "LIC got new data. Update Vis." << LogEnd;
                renderRequest();
            }

        }

        core::BoundingBox SurfaceLIC::getBoundingBox() const
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

        void SurfaceLIC::prepare()
        {
            LogD << "Vis Prepare" << LogEnd;

            SPtr< di::core::Shader > vertexShader = nullptr;
            SPtr< di::core::Shader > fragmentShader = nullptr;

            std::string localShaderPath = core::getResourcePath() + "/algorithms/shaders/";
            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "LICMeshTransform-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "LICMeshTransform-fragment.glsl" ) );

            // Link them to build the program itself
            // m_shaderProgram = std::make_shared< di::core::Program >( { m_vertexShader, m_fragmentShader } );
            // NOTE: the above code does not compile on CLang.
            m_shaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            vertexShader,
                            fragmentShader,
                            std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                              core::readTextFile( localShaderPath + "Shading.glsl" ) )
                        }
            ) );
            m_shaderProgram->realize();

            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "LICEdge-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "LICEdge-fragment.glsl" ) );

            // Link them to build the program itself
            m_edgeProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            vertexShader,
                            fragmentShader
                        }
            ) );
            m_edgeProgram->realize();

            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "LICAdvect-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "LICAdvect-fragment.glsl" ) );

            // Link them to build the program itself
            m_advectProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            vertexShader,
                            fragmentShader
                        }
            ) );
            m_advectProgram->realize();

            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "LICCompose-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "LICCompose-fragment.glsl" ) );

            // Link them to build the program itself
            m_composeProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            vertexShader,
                            fragmentShader
                        }
            ) );
            m_composeProgram->realize();
       }

        void SurfaceLIC::finalize()
        {
            LogD << "Vis Finalize" << LogEnd;
        }

        void SurfaceLIC::render( const core::View& view )
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

            glActiveTexture( GL_TEXTURE0 );
            m_whiteNoiseTex->bind();
            logGLError();

            glEnable( GL_BLEND );

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 1 - Draw Color and Noise on geometry to textures:

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboTransform );

            // NOTE: keep original Viewport
            // glViewport( 0, 0, view.getViewportSize().x, view.getViewportSize().y );
            logGLError();
            GLenum drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            glDrawBuffers( 3, drawBuffers );
            logGLError();

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glClearColor( 1.0f, 0.0f, 0.0f, .0f );

            glBindVertexArray( m_VAO );
            glDrawElements( GL_TRIANGLES, m_visTriangleData->getGrid()->getTriangles().size() * 3, GL_UNSIGNED_INT, NULL );
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 2 - Edge detection on depth buffer

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboEdge );
            logGLError();
            GLenum drawBuffersEdge[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers( 1, drawBuffersEdge );
            logGLError();

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glClearColor( 1.0f, 0.0f, 0.0f, .0f );

            // draw a big quad
            m_edgeProgram->bind();
            m_edgeProgram->setUniform( "u_viewportSize", view.getViewportSize() );
            logGLError();

            glActiveTexture( GL_TEXTURE0 );
            m_step1DepthTex->bind();
            glBindVertexArray( m_screenQuadVAO );
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 3 - Advect along the vec field

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboAdvect );
            logGLError();
            GLenum drawBuffersAdvect[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers( 1, drawBuffersAdvect );
            logGLError();

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glClearColor( 1.0f, 0.0f, 0.0f, .0f );

            // draw a big quad
            m_advectProgram->bind();
            m_advectProgram->setUniform( "u_viewportSize", view.getViewportSize() );
            m_advectProgram->setUniform( "u_viewportScale", view.getViewportSize() / glm::vec2( 2048, 2048 ) );
            logGLError();

            glActiveTexture( GL_TEXTURE0 );
            m_step1DepthTex->bind();
            glActiveTexture( GL_TEXTURE1 );
            m_step1NoiseTex->bind();
            glActiveTexture( GL_TEXTURE2 );
            m_step1VecTex->bind();
            glBindVertexArray( m_screenQuadVAO );
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Final Step - Merge everything and output to the normal framebuffer:

            // Set the view to be the target.
            view.bind();

            // draw a big quad
            m_composeProgram->bind();
            m_composeProgram->setUniform( "u_viewportScale", view.getViewportSize() / glm::vec2( 2048, 2048 ) );
            logGLError();

            glActiveTexture( GL_TEXTURE0 );
            m_step1ColorTex->bind();
            glActiveTexture( GL_TEXTURE1 );
            m_step1VecTex->bind();
            glActiveTexture( GL_TEXTURE2 );
            m_step1DepthTex->bind();
            glGenerateMipmap( GL_TEXTURE_2D );
            glActiveTexture( GL_TEXTURE3 );
            m_step2EdgeTex->bind();
            glGenerateMipmap( GL_TEXTURE_2D );
            glActiveTexture( GL_TEXTURE4 );
            m_step1NoiseTex->bind();
            glActiveTexture( GL_TEXTURE5 );
            m_step3AdvectTex->bind();
            glBindVertexArray( m_screenQuadVAO );
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle
            logGLError();
        }

        void SurfaceLIC::update( const core::View& /* view */, bool reload )
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

            // house-keeping
            if( m_fboEdge )
            {
                glDeleteFramebuffers( 1, &m_fboEdge );
            }
            if( m_fboAdvect )
            {
                glDeleteFramebuffers( 1, &m_fboAdvect );
            }
            if( m_fboTransform )
            {
                glDeleteFramebuffers( 1, &m_fboTransform );
            }

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
            GLint vectorsLoc = m_shaderProgram->getAttribLocation( "vectors" );
            logGLError();

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );
            logGLError();

            // Create some buffers
            m_vertexBuffer = std::make_shared< core::Buffer >();
            m_normalBuffer = std::make_shared< core::Buffer >();
            m_colorBuffer = std::make_shared< core::Buffer >();
            m_vectorsBuffer = std::make_shared< core::Buffer >();
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

            m_vectorsBuffer->realize();
            m_vectorsBuffer->bind();
            m_vectorsBuffer->data( m_visTriangleVectorData->getAttributes() );
            glEnableVertexAttribArray( vectorsLoc );
            glVertexAttribPointer( vectorsLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_indexBuffer->realize();
            m_indexBuffer->bind();
            m_indexBuffer->data( m_visTriangleData->getGrid()->getTriangles() );
            logGLError();

            // for texture coordinates, we use the [0,1]-scaled vertex coordinates -> we need the BB to scale.
            core::BoundingBox bb = m_visTriangleData->getGrid()->getBoundingBox();
            m_shaderProgram->setUniform( "u_meshBBMin", bb.getMin() );
            m_shaderProgram->setUniform( "u_meshBBMax", bb.getMax() );
            logGLError();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Texture input data
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // We need 3D noise.
            m_whiteNoiseTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex3D );
            m_whiteNoiseTex->realize();
            //glActiveTexture( GL_TEXTURE0 );
            m_whiteNoiseTex->bind();
            logGLError();

            const size_t noiseWidth = 128;

            // create some noise
            std::srand( time( 0 ) );
            std::vector< unsigned char > randData;
            randData.reserve( noiseWidth * noiseWidth * noiseWidth );
            for( size_t i = 0; i < noiseWidth * noiseWidth * noiseWidth; ++i )
            {
                unsigned char r = static_cast< unsigned char >( std::rand() % 255 );  // NOLINT - no we want std::rand instead of rand_r
                randData.push_back( r );
            }

            // Commit data
            m_whiteNoiseTex->data( randData.data(), noiseWidth, noiseWidth, noiseWidth, GL_R8, GL_RED, GL_UNSIGNED_BYTE );

            // Bind to the shader
            m_shaderProgram->setUniform( "u_noiseSampler", 0 );

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

            m_step1NoiseTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1NoiseTex->realize();
            m_step1NoiseTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1NoiseTex->data( nullptr, 2048, 2048, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE );
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
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_step1NoiseTex->getObjectID() , 0 );
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_step1DepthTex->getObjectID() , 0 );
            logGLError();

            // Define the out vars to bind to the attachments
            glBindFragDataLocation( m_shaderProgram->getObjectID(), 0, "fragColor" );
            glBindFragDataLocation( m_shaderProgram->getObjectID(), 1, "fragVec" );
            glBindFragDataLocation( m_shaderProgram->getObjectID(), 2, "fragNoise" );
            logGLError();

            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed for Step 1." << LogEnd;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 2: Edges in depth buffer
            LogD << "Creating Edge Pass FBO" << LogEnd;

            // The framebuffer
            glGenFramebuffers( 1, & m_fboEdge );

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboEdge );
            logGLError();

            // We need two target texture: color and noise
            m_step2EdgeTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step2EdgeTex->realize();
            m_step2EdgeTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            // TODO(sebastian): fixed size textures are a problem ...
            m_step2EdgeTex->data( nullptr, 2048, 2048, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE );
            m_step2EdgeTex->setTextureFilter( core::Texture::TextureFilter::LinearMipmapLinear, core::Texture::TextureFilter::Linear );
            logGLError();

            // Bind textures to FBO
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_step2EdgeTex->getObjectID() , 0 );
            logGLError();

            // Define the out vars to bind to the attachments
            glBindFragDataLocation( m_edgeProgram->getObjectID(), 0, "fragEdge" );
            logGLError();

            // Samplers
            m_edgeProgram->bind();
            m_edgeProgram->setUniform( "u_depthSampler", 0 );

            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed for Step 2." << LogEnd;
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 3: Advect
            LogD << "Creating Advect Pass FBO" << LogEnd;

            // The framebuffer
            glGenFramebuffers( 1, & m_fboAdvect );

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboAdvect );
            logGLError();

            // We need two target texture: color and noise
            m_step3AdvectTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step3AdvectTex->realize();
            m_step3AdvectTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            // TODO(sebastian): fixed size textures are a problem ...
            m_step3AdvectTex->data( nullptr, 2048, 2048, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE );
            logGLError();

            // Bind textures to FBO
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_step3AdvectTex->getObjectID() , 0 );
            logGLError();

            // Define the out vars to bind to the attachments
            glBindFragDataLocation( m_advectProgram->getObjectID(), 0, "fragAdvect" );
            logGLError();

            // Samplers
            m_advectProgram->bind();
            m_advectProgram->setUniform( "u_depthSampler", 0 );
            m_advectProgram->setUniform( "u_noiseSampler", 1 );
            m_advectProgram->setUniform( "u_vecSampler",   2 );
            m_advectProgram->setUniform( "u_edgeSampler",   3 );

            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed for Step 3." << LogEnd;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 4: Compose
            LogD << "Creating Compose Pass FBO" << LogEnd;

            // Unbind other FBO
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

            // Bind the result textures to the next step
            m_composeProgram->bind();
            m_composeProgram->setUniform( "u_colorSampler", 0 );
            m_composeProgram->setUniform( "u_vecSampler", 1 );
            m_composeProgram->setUniform( "u_depthSampler", 2 );
            m_composeProgram->setUniform( "u_edgeSampler", 3 );
            m_composeProgram->setUniform( "u_noiseSampler", 4 );
            m_composeProgram->setUniform( "u_advectSampler", 5 );

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create an VAO containing the full-screen quad
            LogD << "Creating flat VAO" << LogEnd;

            // Create the full-screen quad.
            float points[] = {
             -1.0f,  1.0f,  0.0f,
              1.0f,  1.0f,  0.0f,
              1.0f, -1.0f,  0.0f,

              1.0f, -1.0f,  0.0f,
             -1.0f, -1.0f,  0.0f,
             -1.0f,  1.0f,  0.0f
            };

            // Create Vertex Array Object
            glGenVertexArrays( 1, &m_screenQuadVAO );
            glBindVertexArray( m_screenQuadVAO );
            logGLError();

            m_screenQuadVertexBuffer = std::make_shared< core::Buffer >();
            m_screenQuadVertexBuffer->realize();
            m_screenQuadVertexBuffer->bind();
            m_screenQuadVertexBuffer->data( 9 * 2 * sizeof( float ), points );
            logGLError();

            glEnableVertexAttribArray( 0 );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
            logGLError();
        }
    }
}


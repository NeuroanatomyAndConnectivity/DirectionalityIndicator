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

            SPtr< di::core::Shader > vertexShader = nullptr;
            SPtr< di::core::Shader > fragmentShader = nullptr;

            std::string localShaderPath = core::getRuntimePath() + "/algorithms/shaders/";
            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "MeshRender-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "MeshRender-fragment.glsl" ) );

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


            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "LICCompose-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "LICCompose-fragment.glsl" ) );

            // Link them to build the program itself
            // m_shaderProgram = std::make_shared< di::core::Program >( { m_vertexShader, m_fragmentShader } );
            // NOTE: the above code does not compile on CLang.
            m_composeProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            vertexShader,
                            fragmentShader
                        }
            ) );
            m_composeProgram->realize();
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
            logGLError();

            glActiveTexture( GL_TEXTURE0 );
            m_whiteNoiseTex->bind();
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 1 - Draw Color and Noise on geometry to textures:

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );
            glBindFragDataLocation( m_shaderProgram->getObjectID(), 0, "fragColor" );
            glBindFragDataLocation( m_shaderProgram->getObjectID(), 1, "fragNoise" );

            glViewport( 0, 0, view.getViewportSize().x, view.getViewportSize().y );
            logGLError();
            GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers( 2, drawBuffers );
            logGLError();

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glClearColor( 1.0f, 0.0f, 0.0f, .0f );

            glBindVertexArray( m_VAO );
            glDrawElements( GL_TRIANGLES, m_visTriangleData->getGrid()->getTriangles().size() * 3, GL_UNSIGNED_INT, NULL );
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Final Step - Merge everything and output to the normal framebuffer:

            // unbind previously bound FBOs
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
            glDrawBuffer( GL_BACK );
            logGLError();

            glEnable( GL_BLEND );
            glDisable( GL_DEPTH_TEST );

            // draw a big quad
            m_composeProgram->bind();
            m_composeProgram->setUniform( "u_viewportScale", view.getViewportSize() / glm::vec2( 2048, 2048 ) );
            logGLError();

            glActiveTexture( GL_TEXTURE0 );
            m_step1ColorTex->bind();
            glActiveTexture( GL_TEXTURE1 );
            m_step1NoiseTex->bind();
            glActiveTexture( GL_TEXTURE2 );
            m_step1DepthTex->bind();
            glBindVertexArray( m_screenQuadVAO );
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle
            logGLError();
            glEnable( GL_DEPTH_TEST );
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

            // The framebuffer
            glGenFramebuffers( 1, & m_fbo );

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );
            logGLError();

            // We need two target texture: color and noise
            m_step1ColorTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1ColorTex->realize();
            m_step1ColorTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            // TODO(sebastian): fixed size textures are a problem ...
            m_step1ColorTex->data( nullptr, 2048, 2048, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
            logGLError();

            m_step1NoiseTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1NoiseTex->realize();
            m_step1NoiseTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1NoiseTex->data( nullptr, 2048, 2048, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
            logGLError();

            m_step1DepthTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1DepthTex->realize();
            m_step1DepthTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1DepthTex->data( nullptr, 2048, 2048, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT );
            logGLError();

            // Bind textures to FBO
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_step1ColorTex->getObjectID() , 0 );
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_step1NoiseTex->getObjectID() , 0 );
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_step1DepthTex->getObjectID() , 0 );
            logGLError();

            // Define the out vars to bind to the attachments
            glBindFragDataLocation( m_shaderProgram->getObjectID(), 0, "fragColor" );
            glBindFragDataLocation( m_shaderProgram->getObjectID(), 1, "fragNoise" );
            logGLError();

            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "FBO issue?" << LogEnd;
            }


            // Bind the result textures to the next step
            m_composeProgram->bind();
            m_composeProgram->setUniform( "u_colorSampler", 0 );
            m_composeProgram->setUniform( "u_noiseSampler", 1 );
            m_composeProgram->setUniform( "u_depthSampler", 2 );

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create an VAO containing the full-screen quad

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


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

            m_vectorInput = addInput< di::core::TriangleVectorField >(
                    "Directions",
                    "Directional information on the triangle mesh"
            );

            m_dataLabelInput = addInput< di::io::RegionLabelReader::DataSetType >(
                    "Labels",
                    "Mesh Labels"
            );

            m_enableSSAO = addParameter< bool >(
                    "Shading: SSAO",
                    "SSAO is a modern rendering approach to get smooth shadows in a scene. This helps to improve spatial perception, at the cost of "
                    "rendering performance",
                    true
            );

            m_specularity = addParameter< double >(
                    "Shading: Specularity",
                    "Change the intensity of the specular highlights on the surface.",
                    0.25
            );
            m_specularity->setRangeHint( 0.0, 1.0 );

            m_interpolateOnSurface = addParameter< bool >(
                    "Interpolate",
                    "Turn on/off interpolation of color on the surface itself. This does not influence the calculation and rendering og arrows.",
                    false
            );

            m_colorArrows = addParameter< di::Color >(
                    "Arrows: Color",
                    "Define the color of the arrows.",
                    di::Color( 1.0, 1.0, 1.0, 1.0 )
            );

            m_numArrows = addParameter< int >(
                    "Arrows: Amount",
                    "Define the amount of arrows.",
                    30
            );
            m_numArrows->setRangeHint( 0, 150 );

            m_widthArrows = addParameter< double >(
                    "Arrows: Width",
                    "Define the width of the arrows.",
                    1.5
            );
            m_widthArrows->setRangeHint( 0.0, 5.0 );

            m_widthArrowTails = addParameter< double >(
                    "Arrows: Tail Width ",
                    "Define the width of the arrow tails as a fraction of the overall width.",
                    0.25
            );
            m_widthArrowTails->setRangeHint( 0.0, 1.0 );

            m_lengthArrows = addParameter< double >(
                    "Arrows: Length",
                    "Define the length of the arrows.",
                    5.0
            );
            m_lengthArrows->setRangeHint( 0.0, 15.0 );

            m_distArrows = addParameter< double >(
                    "Arrows: Distance",
                    "Define the distance between the arrows and the surface.",
                    2.0
            );
            m_distArrows->setRangeHint( 0.0, 10.0 );

            m_jitterArrows = addParameter< bool >(
                    "Arrows: Jitter",
                    "Activate to move the center of each arrow around on a random basis. This can help to avoid grid-like arrows artifacts.",
                    false
            );

            m_curvatureArrows = addParameter< bool >(
                    "Arrows: Curved",
                    "Activate to have the arrows follow the surface curvature.",
                    false
            );

            m_curvatureArrowsSampleDensity = addParameter< int >(
                    "Arrows: Curvature Sampling",
                    "Change to increase or decrease the amount of samples on the surface. More means less performance but improved visuals.",
                    16
            );
            m_curvatureArrowsSampleDensity->setRangeHint( 4, 32 );

            m_maskLabelEnable = addParameter< bool >(
                    "Labels: Emphasize Label",
                    "Enable to emphasize the regions with the defined label and to gray-out others.",
                    false
            );

            m_maskLabel = addParameter< std::vector< int > >(
                    "Labels: Emphasize IDs",
                    "Define the labels to emphasize as comma separated list.",
                    { 1, 3, 5 }
            );

            m_desaturationIntensity = addParameter< double >(
                    "Labels: Desaturation Intensity",
                    "Define the intensity of desaturation.",
                    0.125
            );
            m_desaturationIntensity->setRangeHint( 0.0, 1.0 );
        }

        RenderIllustrativeLines::~RenderIllustrativeLines()
        {
            // nothing to clean up so far
        }

        void RenderIllustrativeLines::onParameterChange( SPtr< core::ParameterBase > /* parameter */ )
        {
            // The VIS parameters do not need a complete update.
            return;
        }

        void RenderIllustrativeLines::process()
        {
            // Get input data
            auto data = m_triangleDataInput->getData();
            auto vectors = m_vectorInput->getData();
            auto labels = m_dataLabelInput->getData();

            // only valid if the grids match
            if( data && vectors && labels )
            {
                if( data->getGrid() != vectors->getGrid() )
                {
                    LogD << "Grids do not match. Ignoring new data." << LogEnd;
                    data = nullptr;
                    vectors = nullptr;
                    labels = nullptr;
                }

                auto lmax = *std::max_element( labels->getAttributes()->begin(),
                                               labels->getAttributes()->end() );
                // m_maskLabel->setRangeHint( 0, lmax );

                LogD << "New Label Data in [0, " << lmax << "]." << LogEnd;
            }
            else
            {
                data = nullptr;
                vectors = nullptr;
                labels = nullptr;
            }

            // Provide the needed information to the visualizer itself.
            bool changeVis = ( m_visTriangleData != data ) ||
                             ( m_visTriangleLabelData != labels ) ||
                             ( m_visTriangleVectorData != vectors );
            m_visTriangleData = data;
            m_visTriangleVectorData = vectors;
            m_visTriangleLabelData = labels;

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

            auto arrowVertex = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                                 core::readTextFile(
                                                                     localShaderPath + "RenderIllustrativeLines-Arrows-vertex.glsl" ) );
            auto arrowsFragment = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                    core::readTextFile(
                                                                        localShaderPath + "RenderIllustrativeLines-Arrows-fragment.glsl" ) );
            auto arrowGeometry = std::make_shared< core::Shader >( core::Shader::ShaderType::Geometry,
                                                                   core::readTextFile(
                                                                       localShaderPath + "RenderIllustrativeLines-Arrows-geometry.glsl" ) );

            // Link them to build the program itself
            m_arrowShaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            arrowVertex,
                            arrowsFragment,
                            arrowGeometry,
                            std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                              core::readTextFile( localShaderPath + "Shading.glsl" ) )                        }
            ) );
            m_arrowShaderProgram->realize();

            auto composeVertex = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                                   core::readTextFile(
                                                                       localShaderPath + "RenderIllustrativeLines-Compose-vertex.glsl" ) );
            auto composeFragment = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                     core::readTextFile(
                                                                        localShaderPath + "RenderIllustrativeLines-Compose-fragment.glsl" ) );

            // Link them to build the program itself
            m_composeShaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            composeVertex,
                            composeFragment,
                            std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                              core::readTextFile( localShaderPath + "Shading.glsl" ) ),
                            std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                              core::readTextFile( localShaderPath + "LineAO.glsl" ) )
                        }
            ) );
            m_composeShaderProgram->realize();

            auto finalVertex = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                                   core::readTextFile(
                                                                       localShaderPath + "RenderIllustrativeLines-Final-vertex.glsl" ) );
            auto finalFragment = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                     core::readTextFile(
                                                                        localShaderPath + "RenderIllustrativeLines-Final-fragment.glsl" ) );

            // Link them to build the program itself
            m_finalShaderProgram = SPtr< di::core::Program >( new di::core::Program(
                        {
                            finalVertex,
                            finalFragment
                        }
            ) );
            m_finalShaderProgram->realize();

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

            glDisable( GL_BLEND );
            m_transformShaderProgram->setDefine( "d_enableInterpolation", m_interpolateOnSurface->get() );
            m_transformShaderProgram->bind();
            m_transformShaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            m_transformShaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );
            m_transformShaderProgram->setUniform( "u_specularity",      m_specularity->get() );

            m_transformShaderProgram->setUniform( "u_maskLabel",        m_maskLabel->get(), 20, -1 );
            m_transformShaderProgram->setUniform( "u_maskLabelEnable",  m_maskLabelEnable->get() );
            m_transformShaderProgram->setUniform( "u_desaturationIntensity", m_desaturationIntensity->get() );
            logGLError();

            // NOTE: keep original Viewport
            // glViewport( 0, 0, view.getViewportSize().x, view.getViewportSize().y );
            logGLError();
            GLenum drawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers( 4, drawBuffers );
            logGLError();

            glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            glBindVertexArray( m_VAO );

            // We need random numbers for the arrows

            auto widthOfArrowArea = 1.0f / static_cast< float >( m_numArrows->get() );

            std::default_random_engine generator;
            std::uniform_real_distribution< float > distribution( -1.0f * widthOfArrowArea, 1.0f * widthOfArrowArea  );
            auto dice = std::bind( distribution, generator );

            // Need to update points?
            unsigned int desiredArrows = m_numArrows->get() * m_numArrows->get();
            if( !m_points || ( desiredArrows != m_points->getNumVertices() ) )
            {
                // create regular grid of points
                m_points = std::make_shared< di::core::Points >();

                const size_t xSize = m_numArrows->get();
                const size_t ySize = m_numArrows->get();
                for( size_t y = 0; y <= ySize; ++y )
                {
                    for( size_t x = 0; x <= xSize; ++x )
                    {
                        auto jitterX = 0.0f;
                        auto jitterY = 0.0f;
                        if( m_jitterArrows->get() )
                        {
                            jitterX = 0.25f * dice();
                            jitterY = 0.25f * dice();
                        }

                        m_points->addVertex( jitterX + static_cast< float >( x ) / static_cast< float >( xSize ),
                                             jitterY + static_cast< float >( y ) / static_cast< float >( ySize ),
                                             0.0
                                           );
                    }
                }
                m_vertexBuffer->bind();
                m_vertexBuffer->data( m_points->getVertices() );
                logGLError();
            }

            glDrawElements( GL_TRIANGLES, m_visTriangleData->getGrid()->getTriangles().size() * 3, GL_UNSIGNED_INT, NULL );
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 2 - Draw Arrows:

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboArrow );

            // draw a big grid of points as arrows
            m_arrowShaderProgram->setDefine( "d_curvatureEnable", m_curvatureArrows->get() );
            m_arrowShaderProgram->setDefine( "d_curvatureNumSegments", m_curvatureArrowsSampleDensity->get() );
            m_arrowShaderProgram->setDefine( "d_curvatureNumVerts", 2 * m_curvatureArrowsSampleDensity->get() );

            m_arrowShaderProgram->bind();
            m_arrowShaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            // m_arrowShaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );
            // m_arrowShaderProgram->setUniform( "u_viewportSize", view.getViewportSize() );
            m_arrowShaderProgram->setUniform( "u_viewportScale", ( view.getViewportSize() - glm::vec2( 1.0 ) ) / glm::vec2( m_fboResolution.x,
                                                                                                                            m_fboResolution.y ) );
            m_arrowShaderProgram->setUniform( "u_width", m_widthArrows->get() );
            m_arrowShaderProgram->setUniform( "u_widthTails", m_widthArrowTails->get() );
            m_arrowShaderProgram->setUniform( "u_height", m_lengthArrows->get() );
            m_arrowShaderProgram->setUniform( "u_dist", m_distArrows->get() );
            m_arrowShaderProgram->setUniform( "u_arrowColor", m_colorArrows->get() );

            // Allow the next shader to access step 1 textures
            m_arrowShaderProgram->setUniform( "u_colorSampler",  0 );
            m_arrowShaderProgram->setUniform( "u_vecSampler",    1 );
            m_arrowShaderProgram->setUniform( "u_normalSampler", 2 );
            m_arrowShaderProgram->setUniform( "u_posSampler",    3 );

            logGLError();

            glActiveTexture( GL_TEXTURE0 );
            m_step1ColorTex->bind();
            m_step1ColorTex->setTextureFilter( di::core::Texture::TextureFilter::Nearest, di::core::Texture::TextureFilter::Nearest );
            glActiveTexture( GL_TEXTURE1 );
            m_step1VecTex->bind();
            m_step1VecTex->setTextureFilter( di::core::Texture::TextureFilter::Nearest, di::core::Texture::TextureFilter::Nearest );
            glActiveTexture( GL_TEXTURE2 );
            m_step1NormalTex->bind();
            m_step1NormalTex->setTextureFilter( di::core::Texture::TextureFilter::Nearest, di::core::Texture::TextureFilter::Nearest );
            glActiveTexture( GL_TEXTURE3 );
            m_step1PosTex->bind();
            m_step1PosTex->setTextureFilter( di::core::Texture::TextureFilter::Nearest, di::core::Texture::TextureFilter::Nearest );

            // NOTE: keep original Viewport
            // glViewport( 0, 0, view.getViewportSize().x, view.getViewportSize().y );
            logGLError();
            GLenum drawBuffersStep2[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers( 1, drawBuffersStep2 );
            logGLError();

            glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            glBindVertexArray( m_pointVAO );
            glDrawArrays( GL_POINTS, 0, m_points->getVertices().size() );
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 3 - Merge everything and output to the normal framebuffer:
            // Set the view to be the target.

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboCompose );
            logGLError();

            // draw a big quad and compose
            m_composeShaderProgram->bind();
            // m_composeShaderProgram->setUniform( "u_ProjectionMatrix", view.getCamera().getProjectionMatrix() );
            m_composeShaderProgram->setUniform( "u_ViewMatrix",       view.getCamera().getViewMatrix() );
            // m_composeShaderProgram->setUniform( "u_viewportSize", view.getViewportSize() );
            m_composeShaderProgram->setUniform( "u_viewportScale", view.getViewportSize() / glm::vec2( m_fboResolution.x, m_fboResolution.y ) );
            m_composeShaderProgram->setUniform( "u_bbSize", getBoundingBox().getSize() );
            m_composeShaderProgram->setUniform( "u_enableSSAO", m_enableSSAO->get() );

            // Textures
            glActiveTexture( GL_TEXTURE0 );
            m_step1ColorTex->bind();
            m_step1ColorTex->setTextureFilter( di::core::Texture::TextureFilter::Linear, di::core::Texture::TextureFilter::Linear );
            glActiveTexture( GL_TEXTURE1 );
            m_step2ColorTex->bind();
            m_step2ColorTex->setTextureFilter( di::core::Texture::TextureFilter::Linear, di::core::Texture::TextureFilter::Linear );
            glActiveTexture( GL_TEXTURE2 );
            m_step1DepthTex->bind();
            glGenerateMipmap( GL_TEXTURE_2D );
            glActiveTexture( GL_TEXTURE3 );
            m_step2DepthTex->bind();
            glGenerateMipmap( GL_TEXTURE_2D );
            glActiveTexture( GL_TEXTURE4 );
            m_step1NormalTex->bind();
            m_step1NormalTex->setTextureFilter( di::core::Texture::TextureFilter::Linear, di::core::Texture::TextureFilter::Linear );
            glActiveTexture( GL_TEXTURE5 );
            m_whiteNoiseTex->bind();


            // NOTE: keep original Viewport
            // glViewport( 0, 0, view.getViewportSize().x, view.getViewportSize().y );
            logGLError();
            GLenum drawBuffersStep3[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers( 2, drawBuffersStep3 );
            logGLError();

            glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            // Render
            glBindVertexArray( m_screenQuadVAO );
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle
            logGLError();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 4 -

            view.bind();
            glEnable( GL_BLEND );

            // draw a big quad and compose
            m_finalShaderProgram->bind();
            m_finalShaderProgram->setUniform( "u_viewportScale", view.getViewportSize() / glm::vec2( m_fboResolution.x, m_fboResolution.y ) );
            logGLError();

            // Textures
            glActiveTexture( GL_TEXTURE0 );
            m_step3ColorTex->bind();
            m_step3ColorTex->setTextureFilter( di::core::Texture::TextureFilter::LinearMipmapLinear, di::core::Texture::TextureFilter::Linear );
            glGenerateMipmap( GL_TEXTURE_2D );

            glActiveTexture( GL_TEXTURE1 );
            m_step3DepthTex->bind();
            m_step3DepthTex->setTextureFilter( di::core::Texture::TextureFilter::LinearMipmapLinear, di::core::Texture::TextureFilter::Linear );
            glGenerateMipmap( GL_TEXTURE_2D );

            glActiveTexture( GL_TEXTURE2 );
            m_step3AOTex->bind();
            m_step3AOTex->setTextureFilter( di::core::Texture::TextureFilter::LinearMipmapLinear, di::core::Texture::TextureFilter::Linear );
            glGenerateMipmap( GL_TEXTURE_2D );

            // Render
            glBindVertexArray( m_screenQuadVAO );
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle
            logGLError();
        }

        void RenderIllustrativeLines::update( const core::View& view, bool reload )
        {
            // Force update if resolution mismatch
            auto resolution = di::core::Texture::powerOfTwoResolution( view.getViewportSize() );
            if( m_fboResolution != resolution )
            {
                LogD << "Framebuffer resolution not optimal:" <<
                        " View: " <<  view.getViewportSize().x << "x" <<  view.getViewportSize().y <<
                        ", Current FBO: " << m_fboResolution.x << "x" << m_fboResolution.y <<
                        ", New FBO: " << resolution.x << "x" << resolution.y << LogEnd;

                reload = true;
                m_fboResolution = resolution;
            }

            if( !m_visTriangleData || !m_visTriangleVectorData || !m_visTriangleLabelData )
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
            if( m_fboTransform )
            {
                glDeleteFramebuffers( 1, &m_fboTransform );
            }
            if( m_fboArrow )
            {
                glDeleteFramebuffers( 1, &m_fboArrow );
            }
            if( m_fboCompose )
            {
                glDeleteFramebuffers( 1, &m_fboCompose );
            }

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
            GLint vectorsLoc = m_transformShaderProgram->getAttribLocation( "vectors" );
            GLint labelsLoc = m_transformShaderProgram->getAttribLocation( "label" );

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
            m_labelsBuffer = std::make_shared< core::Buffer >();
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

            m_labelsBuffer->realize();
            m_labelsBuffer->bind();
            m_labelsBuffer->data( m_visTriangleLabelData->getAttributes() );
            glEnableVertexAttribArray( labelsLoc );
            glVertexAttribIPointer( labelsLoc, 1, GL_UNSIGNED_INT, 0, 0 );
            logGLError();

            m_indexBuffer->realize();
            m_indexBuffer->bind();
            m_indexBuffer->data( m_visTriangleData->getGrid()->getTriangles() );
            logGLError();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create Vertex Array Object VAO and the corresponding Vertex Buffer Objects VBO for the mesh itself
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            LogD << "Creating Point VAO" << LogEnd;

            m_arrowShaderProgram->bind();
            logGLError();

            // get the location of attribute "position" in program
            GLint vertexPointLoc = m_arrowShaderProgram->getAttribLocation( "position" );
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

            // NOTE: filled during rendering

            glEnableVertexAttribArray( vertexLoc );
            glVertexAttribPointer( vertexPointLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create a Framebuffer Object (FBO) and setup LIC pipeline
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 1: Render and transform to image space
            LogD << "Creating Transform Pass FBO" << LogEnd;

            m_transformShaderProgram->bind();

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
            m_step1ColorTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
            logGLError();

            m_step1VecTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1VecTex->realize();
            m_step1VecTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1VecTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT );
            logGLError();

            m_step1NormalTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1NormalTex->realize();
            m_step1NormalTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1NormalTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT );
            logGLError();

            m_step1PosTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1PosTex->realize();
            m_step1PosTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1PosTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT );
            logGLError();

            m_step1DepthTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step1DepthTex->realize();
            m_step1DepthTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step1DepthTex->setTextureFilter( core::Texture::TextureFilter::LinearMipmapLinear, core::Texture::TextureFilter::Linear );
            m_step1DepthTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT );
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

            // Final Check
            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed for Step 1." << LogEnd;
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 2: Render and transform to image space
            LogD << "Creating Arrow Pass FBO" << LogEnd;

            m_arrowShaderProgram->bind();

            // The framebuffer
            glGenFramebuffers( 1, &m_fboArrow );

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboArrow );
            logGLError();

            // We need two target texture: color and noise
            m_step2ColorTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step2ColorTex->realize();
            m_step2ColorTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            // TODO(sebastian): fixed size textures are a problem ...
            m_step2ColorTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
            logGLError();

            m_step2DepthTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step2DepthTex->realize();
            m_step2DepthTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step2DepthTex->setTextureFilter( core::Texture::TextureFilter::LinearMipmapLinear, core::Texture::TextureFilter::Linear );
            m_step2DepthTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT );
            logGLError();

            // Bind textures to FBO
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_step2ColorTex->getObjectID() , 0 );
            logGLError();
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  m_step2DepthTex->getObjectID() , 0 );
            logGLError();

            // Define the out vars to bind to the attachments
            glBindFragDataLocation( m_arrowShaderProgram->getObjectID(), 0, "fragColor" );
            logGLError();

            // Check for validity
            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed for Step 2." << LogEnd;
            }

            // We need 3D noise.
            m_whiteNoiseTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_whiteNoiseTex->realize();
            m_whiteNoiseTex->bind();
            logGLError();

            const size_t noiseWidth = 128;

            // create some noise
            std::srand( time( 0 ) );
            std::vector< unsigned char > randData;
            randData.reserve( noiseWidth * noiseWidth * 3 );
            for( size_t i = 0; i < noiseWidth * noiseWidth * 3; ++i )
            {
                unsigned char r = static_cast< unsigned char >( std::rand() % 255 );  // NOLINT - no we want std::rand instead of rand_r
                randData.push_back( r );
            }

            // Commit data
            m_whiteNoiseTex->data( randData.data(), noiseWidth, noiseWidth, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE );

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 3 - Compose
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

            // Do not forget to make the textures available to the final step
            if( view.isHQMode() )
            {
                m_composeShaderProgram->setDefine( "d_samples", 64 );
            }
            else
            {
                m_composeShaderProgram->setDefine( "d_samples", 16 );
            }
            m_composeShaderProgram->bind();
            m_composeShaderProgram->setUniform( "u_meshColorSampler",  0 );
            m_composeShaderProgram->setUniform( "u_arrowColorSampler", 1 );
            m_composeShaderProgram->setUniform( "u_meshDepthSampler",  2 );
            m_composeShaderProgram->setUniform( "u_arrowDepthSampler", 3 );
            m_composeShaderProgram->setUniform( "u_meshNormalSampler",  4 );
            m_composeShaderProgram->setUniform( "u_noiseSampler",  5 );
            logGLError();

            LogD << "Creating Compose Pass FBO" << LogEnd;

            // The framebuffer
            glGenFramebuffers( 1, &m_fboCompose );

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fboCompose );
            logGLError();

            // We need two target texture: color and noise
            m_step3ColorTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step3ColorTex->realize();
            m_step3ColorTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            // TODO(sebastian): fixed size textures are a problem ...
            m_step3ColorTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
            logGLError();

            // We need two target texture: color and noise
            m_step3AOTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step3AOTex->realize();
            m_step3AOTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            // TODO(sebastian): fixed size textures are a problem ...
            m_step3AOTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
            logGLError();

            m_step3DepthTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D );
            m_step3DepthTex->realize();
            m_step3DepthTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_step3DepthTex->setTextureFilter( core::Texture::TextureFilter::LinearMipmapLinear, core::Texture::TextureFilter::Linear );
            m_step3DepthTex->data( nullptr, m_fboResolution.x, m_fboResolution.y, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT );
            logGLError();

            // Bind textures to FBO
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_step3ColorTex->getObjectID() , 0 );
            logGLError();
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_step3AOTex->getObjectID() , 0 );
            logGLError();
            glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  m_step3DepthTex->getObjectID() , 0 );
            logGLError();

            // Define the out vars to bind to the attachments
            glBindFragDataLocation( m_composeShaderProgram->getObjectID(), 0, "fragColor" );
            glBindFragDataLocation( m_composeShaderProgram->getObjectID(), 1, "fragAO" );
            logGLError();

            // Check for validity
            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed for Step 3." << LogEnd;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Step 4 - Compose

            m_finalShaderProgram->bind();
            m_finalShaderProgram->setUniform( "u_colorSampler",  0 );
            m_finalShaderProgram->setUniform( "u_depthSampler", 1 );
            m_finalShaderProgram->setUniform( "u_aoSampler",  2 );
            logGLError();
        }
    }
}


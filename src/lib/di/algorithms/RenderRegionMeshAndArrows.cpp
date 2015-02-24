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

#include "RenderRegionMeshAndArrows.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/RenderRegionMeshAndArrows"

namespace di
{
    namespace algorithms
    {
        RenderRegionMeshAndArrows::RenderRegionMeshAndArrows():
            Algorithm( "Render Region Mesh and Arrows",
                       "This algorithm takes pre-calculated region information and presents it in an appealing way." ),
            Visualization()
        {
            // We require some inputs.
            m_regionInput = addInput< ExtractRegions::RegionDataSet >(
                                "Region Information",
                                "Collection of necessary data created by the ExtractRegions algorithm."
                            );
        }

        RenderRegionMeshAndArrows::~RenderRegionMeshAndArrows()
        {
            // nothing to clean up so far
        }

        void RenderRegionMeshAndArrows::process()
        {
            // Get input data
            auto regionData = m_regionInput->getData();
            auto regionCenters = regionData->getGrid();
            auto regionNormal = regionData->getAttributes<0>();
            auto regionColors = regionData->getAttributes<1>();
            auto regionNeighbours = regionData->getAttributes<2>();
            auto regionConnections = regionData->getAttributes<3>();

            auto lineStrips = std::make_shared< std::vector< glm::vec3 > >();
            auto lineStripNormals = std::make_shared< std::vector< glm::vec3 > >();
            auto lineStripColors = std::make_shared< di::RGBAArray >();
            core::BoundingBox bb;

            // go through each region,
            for( size_t regID = 0; regID < regionCenters->getNumVertices(); ++regID )
            {
                // get this regions center and normal
                auto normal = ( *regionNormal )[ regID ];
                auto center = regionCenters->getVertex( regID );
                auto color = ( *regionColors )[ regID ];
                bb.include( center );

                // create an arrow for each connected region as line strip
                for( auto connectToRegID : ( *regionConnections )[ regID ] )
                {
                    auto targetNormal = ( *regionNormal )[ connectToRegID ];
                    auto targetCenter = regionCenters->getVertex( connectToRegID );
                    auto targetColor = ( *regionColors )[ connectToRegID ];

                    bb.include( targetCenter );
                    lineStrips->push_back( center );
                    lineStrips->push_back( targetCenter );

                    lineStripNormals->push_back( normal );
                    lineStripNormals->push_back( targetNormal );

                    lineStripColors->push_back( color );
                    lineStripColors->push_back( targetColor );
                }
            }

            // Provide the needed information to the visualizer itself.
            m_visLineStrips = lineStrips;
            m_visLineStripNormals = lineStripNormals;
            m_visLineStripColors = lineStripColors;
            m_visBB = bb;

            // As the rendering system does not render permanently, inform about the update.
            renderRequest();
        }

        core::BoundingBox RenderRegionMeshAndArrows::getBoundingBox() const
        {
            return m_visBB;
        }

        void RenderRegionMeshAndArrows::prepare()
        {
            LogD << "Vis Prepare" << LogEnd;

            SPtr< di::core::Shader > vertexShader = nullptr;
            SPtr< di::core::Shader > fragmentShader = nullptr;

            std::string localShaderPath = core::getResourcePath() + "/algorithms/shaders/";
            vertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "RenderRegionMeshAndArrows-vertex.glsl" ) );
            fragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "RenderRegionMeshAndArrows-fragment.glsl" ) );

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

        void RenderRegionMeshAndArrows::finalize()
        {
            LogD << "Vis Finalize" << LogEnd;
        }

        void RenderRegionMeshAndArrows::render( const core::View& view )
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
            glDrawArrays( GL_LINES, 0, m_visLineStrips->size() );

            glDisable(  GL_BLEND );
            logGLError();
        }

        void RenderRegionMeshAndArrows::update( const core::View& /* view */, bool reload )
        {
            // Be warned: this method is huge. I did not yet use a VAO and VBO abstraction. This causes the code to be quite long. But I structured it
            // and many code parts repeat again and again.

            if( !m_visLineStrips )
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
            LogD << "Creating Line Strip VAO" << LogEnd;

            m_shaderProgram->bind();
            logGLError();

            // get the location of attribute "position" in program
            GLint vertexLoc = m_shaderProgram->getAttribLocation( "position" );
            GLint normalLoc = m_shaderProgram->getAttribLocation( "normal" );
            GLint colorLoc = m_shaderProgram->getAttribLocation( "color" );
            logGLError();

            // Create the VAO
            glGenVertexArrays( 1, &m_VAO );
            glBindVertexArray( m_VAO );
            logGLError();

            // Create some buffers
            m_vertexBuffer = std::make_shared< core::Buffer >();
            m_normalBuffer = std::make_shared< core::Buffer >();
            m_colorBuffer = std::make_shared< core::Buffer >();
            logGLError();

            // Set the data using the triangle mesh. Also set the location mapping of the shader using the VAO
            m_vertexBuffer->realize();
            m_vertexBuffer->bind();
            m_vertexBuffer->data( m_visLineStrips );
            logGLError();

            glEnableVertexAttribArray( vertexLoc );
            glVertexAttribPointer( vertexLoc, 3, GL_FLOAT, 0, 0, 0 );
            logGLError();

            m_normalBuffer->realize();
            m_normalBuffer->bind();
            m_normalBuffer->data( m_visLineStripNormals );
            glEnableVertexAttribArray( normalLoc );
            glVertexAttribPointer( normalLoc, 3, GL_FLOAT, 0, 0, 0 );

            m_colorBuffer->realize();
            m_colorBuffer->bind();
            m_colorBuffer->data( m_visLineStripColors );
            glEnableVertexAttribArray( colorLoc );
            glVertexAttribPointer( colorLoc, 4, GL_FLOAT, 0, 0, 0 );

            logGLError();
        }
    }
}


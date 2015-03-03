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

#ifndef DI_RENDERREGIONMESHANDARROWS_H
#define DI_RENDERREGIONMESHANDARROWS_H

#include <vector>

#include <di/gfx/GL.h>

#include <di/core/Algorithm.h>
#include <di/core/Visualization.h>
#include <di/algorithms/ExtractRegions.h>

namespace di
{
    namespace core
    {
        class LineDataSet;
        class View;
    }

    namespace algorithms
    {
        /**
         * Render line Data. This class implements the algorithm and the visualization.
         */
        class RenderRegionMeshAndArrows: public di::core::Algorithm,
                                         public di::core::Visualization
        {
        public:
            /**
             * Constructor. Initialize all inputs, outputs and parameters.
             */
            RenderRegionMeshAndArrows();

            /**
             * Destructor. Clean up if needed.
             */
            virtual ~RenderRegionMeshAndArrows();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Algorithm Specific Methods

            /**
             * Process the data in the inputs and update output data. Keep in mind that this might be called in its own thread thread.
             */
            virtual void process();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Visualization Specific Methods

            /**
             * Prepare your visualization. This includes creation of resources, buffers, and others.
             * If an error occurs, throw an exception accordingly.
             *
             * \note this runs in the OpenGL thread and the context is bound.
             */
            virtual void prepare();

            /**
             * Finalize your OpenGL resources here. Free buffers and shaders.
             * If an error occurs, throw an exception accordingly.
             *
             * \note this runs in the OpenGL thread and the context is bound.
             */
            virtual void finalize();

            /**
             * Do actual rendering.
             * If an error occurs, throw an exception accordingly.
             *
             * \note this runs in the OpenGL thread and the context is current.
             *
             * \param view the view to render to. This contains probably useful information.
             */
            virtual void render( const core::View& view );

            /**
             * This method is called between the frames. Use this to update resources. Immediately return if nothing needs to update. If you do not
             * want to update anything at all, do not overwrite.
             * If an error occurs, throw an exception accordingly.
             *
             * \note this runs in the OpenGL thread and the context is current.
             *
             * \param view the view to render to. This contains probably useful information.
             * \param reload to force a reload of all resources.
             */
            virtual void update( const core::View& view, bool reload = false );

            /**
             * Each visualization needs to know the rendering area it will use. In most cases, this is the bounding box of the rendered geometry.
             * Avoid long running functions, since they block the OpenGL thread.
             *
             * \return bounding box of this visualization
             */
            virtual core::BoundingBox getBoundingBox() const;

        protected:
        private:
            /**
             * Region information to use.
             */
            SPtr< di::core::Connector< ExtractRegions::RegionDataSet > > m_regionInput;

            /**
             * The line input to use.
             */
            SPtr< di::core::Connector< di::core::LineDataSet > > m_lineDataInput;

            /**
             * The line data to visualize. We keep the pointer separate since process() and update()/render() work in different threads.
             */
            ConstSPtr< std::vector< glm::vec3 > > m_visLineStrips = nullptr;

            /**
             * The line data to visualize. We keep the pointer separate since process() and update()/render() work in different threads.
             */
            ConstSPtr< std::vector< glm::vec3 > > m_visLineStripNormals = nullptr;

            /**
             * The line data to visualize. We keep the pointer separate since process() and update()/render() work in different threads.
             */
            ConstSPtr< RGBAArray > m_visLineStripColors = nullptr;

            /**
             * The vis bounding box.
             */
            core::BoundingBox m_visBB = core::BoundingBox();

            /**
             * The Vertex Attribute Array Object (VAO) used for the data.
             */
            GLuint m_VAO = 0;

            /**
             * The shader used for rendering
             */
            SPtr< di::core::Program > m_shaderProgram = nullptr;

            /**
             * Vertex data.
             */
            SPtr< di::core::Buffer > m_vertexBuffer = nullptr;

            /**
             * Normal data.
             */
            SPtr< di::core::Buffer > m_normalBuffer = nullptr;

            /**
             * Color data.
             */
            SPtr< di::core::Buffer > m_colorBuffer = nullptr;
        };
    }
}

#endif  // DI_RENDERREGIONMESHANDARROWS_H

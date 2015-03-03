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

#ifndef DI_OFFSCREENVIEW_H
#define DI_OFFSCREENVIEW_H

#include <utility>

#include <di/gfx/PixelData.h>
#include <di/gfx/View.h>
#include <di/GfxTypes.h>

namespace di
{
    namespace core
    {
        /**
         * Class represents the offscreen rendering view with a fixed size.
         */
        class OffscreenView: public View
        {
        public:
            /**
             * Construct the view.
             *
             * \param size the size to use
             * \param samples set number of samples to take and combine for anti-aliasing.
             */
            OffscreenView( glm::vec2 size, int samples = 1 );

            /**
             * Destruct. Cleanup.
             */
            virtual ~OffscreenView() = default;

            /**
             * Get the origin of the viewport.
             *
             * \return the origin
             */
            glm::vec2 getViewportOrigin() const override;

            /**
             * Gets the size of the viewport in x and y directions. Keep in mind that the physical pixel coordinate of the upper-right corner will be
             * origin + size - (1,1). Alternatively, use \ref getViewport() to query this directly.
             *
             * \return
             */
            glm::vec2 getViewportSize() const override;

            /**
             * Get the camera of this view.
             *
             * \return the camera.
             */
            const Camera& getCamera() const override;

            /**
             * Set the camera to use.
             *
             * \param camera the camera.
             */
            void setCamera( const core::Camera& camera );

            /**
             * Prepare offscreen buffers. Call this before using it.
             */
            void prepare();

            /**
             * Bind this view as rendering target. You should issue this if you use your own FBOs.
             */
            void bind() const override;

            /**
             * Finalize the buffers. Call this to free the allocated data.
             */
            void finalize();

            /**
             * Read back the texture. Width and height are the viewport size.
             *
             * \return the image
             */
            SPtr< RGBA8Image > read() const;
        protected:
        private:
            /**
             * Size of the viewport
             */
            glm::vec2 m_size = glm::vec2( 0.0f, 0.0f );

            /**
             * Multisampling if more than 1
             */
            int m_samples = 1;

            /**
             * The camera of the view.
             */
            core::Camera m_camera;

            /**
             * The fbo ID
             */
            GLuint m_fbo = 0;

            /**
             * Result texture
             */
            SPtr< di::core::Texture > m_outputTex = nullptr;

            /**
             * Depth texture output
             */
            SPtr< di::core::Texture > m_outputDepth = nullptr;
        };
    }
}

#endif  // DI_OFFSCREENVIEW_H


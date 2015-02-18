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

#ifndef CAMERA_H
#define CAMERA_H

#include <di/GfxTypes.h>

namespace di
{
    namespace core
    {
        /**
         * Class represents the camera of a view. In OpenGL, a camera is nothing more than a matrix. This class provides
         * some functionality to set the matrix. This camera additionally differentiates between view and projection.
         */
        class Camera
        {
        public:
            /**
             * Construct the camera. The camera uses a identity matrix for projection and a identity view matrix.
             */
            Camera();

            /**
             * Destruct. Cleanup.
             */
            virtual ~Camera();

            /**
             * Get the camera view matrix. This matrix transforms the scene prior to projection. So, it is the view-part of the well-known OpenGL
             * ModelViewMatrix concept.
             *
             * \return the view.
             */
            glm::mat4 getViewMatrix() const;

            /**
             * Get the camera projection matrix.
             *
             * \return the projection matrix.
             */
            glm::mat4 getProjectionMatrix() const;

            /**
             * Set the view matrix.
             *
             * \param matrix the view matrix.
             */
            void setViewMatrix( const glm::mat4& matrix );

            /**
             * Set the camera projection matrix.
             *
             * \param matrix the projection matrix.
             */
            void setProjectionMatrix( const glm::mat4& matrix );

        protected:
        private:
            /**
             * The view matrix.
             */
            glm::mat4 m_view;

            /**
             * The projection matrix.
             */
            glm::mat4 m_projection;
        };
    }
}

#endif  // CAMERA_H


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

#include <utility>

#include <di/gfx/GL.h>
#include <di/gfx/GLError.h>

#include "OffscreenView.h"

#include <di/core/Logger.h>
#define LogTag "gui/OffscreenView"

namespace di
{
    namespace core
    {
        OffscreenView::OffscreenView( glm::vec2 size, int samples ):
            View(),
            m_size( size ),
            m_samples( samples )
        {
        }

        glm::vec2 OffscreenView::getViewportOrigin() const
        {
            return glm::vec2( 0.0f, 0.0f );
        }

        glm::vec2 OffscreenView::getViewportSize() const
        {
            return m_size;
        }

        const Camera& OffscreenView::getCamera() const
        {
            return m_camera;
        }

        void OffscreenView::setCamera( const core::Camera& camera )
        {
            m_camera = camera;
        }

        SPtr< std::vector< uint8_t > > OffscreenView::read() const
        {
            LogD << "FBO read-back." << LogEnd;

            auto pixels = std::make_shared< std::vector< uint8_t > >( 4 * getViewportSize().x * getViewportSize().y );

            m_outputTex->bind();
            m_outputTex->read( pixels->data(), GL_RGBA8, GL_UNSIGNED_BYTE );

            return pixels;
        }

        void OffscreenView::bind() const
        {
            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );
            logGLError();
            GLenum drawBuffersEdge[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers( 1, drawBuffersEdge );
            logGLError();
        }

        void OffscreenView::prepare()
        {
            LogD << "Creating FBO" << LogEnd;

            // The framebuffer
            glGenFramebuffers( 1, &m_fbo );

            // Bind it to be able to modify and configure:
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );
            logGLError();

            // We need two target texture: color and noise
            m_outputTex = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D_MultiSample );
            m_outputTex->realize();
            m_outputTex->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_outputTex->data( getViewportSize().x, getViewportSize().y, 1, m_samples, GL_RGBA8 );
            // m_outputTex->setTextureFilter( core::Texture::TextureFilter::LinearMipmapLinear, core::Texture::TextureFilter::Linear );
            logGLError();

            // We require a depth attachment to have depth buf operations
            m_outputDepth = std::make_shared< core::Texture >( core::Texture::TextureType::Tex2D_MultiSample );
            m_outputDepth->realize();
            m_outputDepth->bind();
            // NOTE: to use an FBO, the texture needs to be initalized empty.
            m_outputDepth->data( getViewportSize().x, getViewportSize().y, 1, m_samples, GL_DEPTH_COMPONENT24 );
            // m_outputDepth->setTextureFilter( core::Texture::TextureFilter::LinearMipmapLinear, core::Texture::TextureFilter::Linear );
            logGLError();

            // Bind textures to FBO
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_outputTex->getObjectID(), 0 );
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_outputDepth->getObjectID(), 0 );
            logGLError();

            if( glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            {
                LogE << "glCheckFramebufferStatus failed." << LogEnd;
            }
        }

        void OffscreenView::finalize()
        {
            LogD << "Deleting FBO" << LogEnd;

            glDeleteFramebuffers( 1, &m_fbo );
            logGLError();
            m_outputTex = nullptr;
        }
    }
}


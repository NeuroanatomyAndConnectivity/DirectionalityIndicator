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
#include <algorithm>

#include <di/GfxTypes.h>
#include <di/gfx/ViewEventListener.h>

#include "View.h"

namespace di
{
    namespace core
    {
        View::View()
        {
            // nothing
        }

        View::~View()
        {
            // nothing
        }

        std::pair< glm::vec2, glm::vec2 > View::getViewport() const
        {
            return std::make_pair( getViewportOrigin(), getViewportOrigin() + getViewportSize() );
        }

        double View::getAspectRatio() const
        {
            return getViewportSize().x / getViewportSize().y;
        }

        bool View::isHQMode() const
        {
            return m_hqMode;
        }

        void View::setHQMode( bool hq )
        {
            m_hqMode = hq;
        }

        void View::pushEvent( SPtr< ViewEvent > event )
        {
            std::unique_lock< std::mutex > lock( m_eventListenerMutex );
            for( auto listener : m_eventListeners )
            {
                listener->handle( event );
            }
        }

        SPtrVec< ViewEventListener > View::getEventListener() const
        {
            std::unique_lock< std::mutex > lock( m_eventListenerMutex );
            return m_eventListeners;
        }

        void View::addEventListener( SPtr< ViewEventListener > listener )
        {
            // Used to lock the command queue mutex
            std::unique_lock< std::mutex > lock( m_eventListenerMutex );
            auto result = std::find( m_eventListeners.begin(), m_eventListeners.end(), listener );
            if( result == m_eventListeners.end() )
            {
                m_eventListeners.push_back( listener );
            }
        }

        void View::removeEventListener( SPtr< ViewEventListener > listener )
        {
            // Used to lock the command queue mutex
            std::unique_lock< std::mutex > lock( m_eventListenerMutex );
            m_eventListeners.erase( std::remove( m_eventListeners.begin(), m_eventListeners.end(), listener ), m_eventListeners.end() );
        }
    }
}


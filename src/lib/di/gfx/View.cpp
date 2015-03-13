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
    }
}


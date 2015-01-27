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

#include "Visualization.h"

namespace di
{
    namespace core
    {
        Visualization::Visualization()
        {
            // nothing to do
            m_renderingRequested.store( false );
        }

        Visualization::~Visualization()
        {
            // nothing to do
        }

        void Visualization::update()
        {
            // nothing to do
        }

        void Visualization::renderRequest()
        {
            // This is a temporary solution. This will be replaced with a push-based approach soon.
            m_renderingRequested.store( true );
        }

        void Visualization::resetRenderingRequest()
        {
            // This is a temporary solution. This will be replaced with a push-based approach soon.
            m_renderingRequested.store( false );
        }

        bool Visualization::isRenderingRequested() const
        {
            return m_renderingRequested.load();
        }
    }
}


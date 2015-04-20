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

#include "QueryState.h"

namespace di
{
    namespace commands
    {
        QueryState::QueryState( SPtr< di::core::CommandObserver > observer ):
            Command( observer )
        {
        }

        QueryState::~QueryState()
        {
        }

        std::string QueryState::getName() const
        {
            return "Query Network State";
        }

        std::string QueryState::getDescription() const
        {
            return "Query the network state.";
        }

        void QueryState::setState( const di::core::State& state )
        {
            m_state = state;
        }

        const di::core::State& QueryState::getState() const
        {
            return m_state;
        }
    }
}

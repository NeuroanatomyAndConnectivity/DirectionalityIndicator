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

#define  LogTag "core/Algorithm"
#include "Logger.h"

#include "Algorithm.h"

namespace di
{
    namespace core
    {
        Algorithm::Algorithm( const std::string& name, const std::string& description ):
            m_name( name ),
            m_description( description )
        {
            // init
        }

        Algorithm::~Algorithm()
        {
            // cleanup
        }

        const ConstSPtrSet< ConnectorBase >& Algorithm::getInputs() const
        {
            return m_inputs;
        }

        const ConstSPtrSet< ConnectorBase >& Algorithm::getOutputs() const
        {
            return m_outputs;
        }

        void Algorithm::addInput( SPtr< ConnectorBase > input )
        {
            m_inputs.insert( input );
        }

        void Algorithm::addOutput( SPtr< ConnectorBase > output )
        {
            m_outputs.insert( output );
        }

        const std::string& Algorithm::getName() const
        {
            return m_name;
        }

        const std::string& Algorithm::getDescription() const
        {
            return m_description;
        }
    }
}


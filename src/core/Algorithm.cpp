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

#include <algorithm>

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

        ConstSPtr< ConnectorBase > Algorithm::searchConnector( const ConstSPtrSet< ConnectorBase >& where, const std::string& name ) const
        {
            // search for the output with the specified name
            auto iter = std::find_if( where.begin(), where.end(),
                    [ &name ]( const ConstSPtr< di::core::ConnectorBase >& arg )
                    {
                        return arg->getName() == name;
                    }
            );

            // Not found? Throw exception
            if( iter == where.end() )
            {
                throw std::invalid_argument( "Could not find connector \"" + name + "\" in algorithm \"" + getName() + "\"." );
            }

            return *iter;
        }

        ConstSPtr< ConnectorBase > Algorithm::searchConnector( const ConstSPtrSet< ConnectorBase >& where, size_t index ) const
        {
            // is index in bounds?
            if( index >= where.size() )
            {
                throw std::out_of_range( "The connector index " + std::to_string( index ) + " is invalid for algorithm \"" + getName() + "\"." );
            }

            // get the indexed element.
            return *std::next( where.begin(), index );
        }

        ConstSPtr< ConnectorBase > Algorithm::getInput( const std::string& name ) const
        {
            return searchConnector( getInputs(), name );
        }

        ConstSPtr< ConnectorBase > Algorithm::getInput( size_t index ) const
        {
            return searchConnector( getInputs(), index );
        }

        ConstSPtr< ConnectorBase > Algorithm::getOutput( const std::string& name ) const
        {
            return searchConnector( getOutputs(), name );
        }

        ConstSPtr< ConnectorBase > Algorithm::getOutput( size_t index ) const
        {
            return searchConnector( getOutputs(), index );
        }
    }
}


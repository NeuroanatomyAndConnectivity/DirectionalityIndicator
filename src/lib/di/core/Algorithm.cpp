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
#include <string>

#include <di/core/ObserverCallback.h>
#include <di/core/ObserverParameter.h>

#include "Algorithm.h"

#include <di/core/Logger.h>
#define  LogTag "core/Algorithm"

namespace di
{
    namespace core
    {
        Algorithm::Algorithm( const std::string& name, const std::string& description ):
            Observable(),
            m_name( name ),
            m_description( description )
        {
            // init
            m_active.store( true );
        }

        Algorithm::~Algorithm()
        {
            // cleanup
        }

        std::string Algorithm::getInstanceInfo() const
        {
            return getName() + " (" + Observable::getInstanceInfo() + ")";
        }

        const SPtrSet< ConnectorBase >& Algorithm::getInputs() const
        {
            return m_inputs;
        }

        const ConstSPtrSet< ConnectorBase >& Algorithm::getOutputs() const
        {
            return m_outputs;
        }

        const SPtrVec< ParameterBase >& Algorithm::getParameters() const
        {
            return m_parameters;
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

        void Algorithm::addParameter( SPtr< ParameterBase > parameter )
        {
            parameter->observe(
                    std::make_shared< ObserverParameter >( [ & ]( SPtr< ParameterBase > param )
                    {
                        this->onParameterChange( param );
                    },
                    parameter
                )
            );
            m_parameters.push_back( parameter );
        }

        void Algorithm::onParameterChange( SPtr< ParameterBase > parameter )
        {
            LogD << "Change in parameter: " << parameter->getName() << ". Requesting update." << LogEnd;
            requestUpdate();
        }

        bool Algorithm::isInput( ConstSPtr< ConnectorBase > connector ) const
        {
            return ( std::find( getInputs().begin(), getInputs().end(), connector ) != getInputs().end() );
        }

        bool Algorithm::isOutput( ConstSPtr< ConnectorBase > connector ) const
        {
            return ( std::find( getOutputs().begin(), getOutputs().end(), connector ) != getOutputs().end() );
        }

        bool Algorithm::hasConnector( ConstSPtr< ConnectorBase > connector ) const
        {
            return isInput( connector ) || isOutput( connector );
        }

        SPtr< ConnectorBase > Algorithm::getInput( const std::string& name ) const
        {
            return searchConnector( getInputs(), name );
        }

        SPtr< ConnectorBase > Algorithm::getInput( size_t index ) const
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

        void Algorithm::requestUpdate( bool request )
        {
            if( ( m_updateRequested == request ) || ( !request ) )
            {
                m_updateRequested = request;
                return;
            }
            m_updateRequested = request;
            notify();
        }

        bool Algorithm::isUpdateRequested() const
        {
            return m_updateRequested;
        }

        bool Algorithm::isSource() const
        {
            return ( getInputs().empty() && !getOutputs().empty() );
        }

        bool Algorithm::isSink() const
        {
            return ( getOutputs().empty() && !getInputs().empty() );
        }

        bool Algorithm::isActive() const
        {
            return m_active.load();
        }

        void Algorithm::setActive( bool active )
        {
            if( active )
            {
                LogD << "Activating \"" << m_name << "\"." << LogEnd;
            }
            else
            {
                LogD << "De-activating \"" << m_name << "\"." << LogEnd;
            }
            m_active.store( active );
        }

        std::ostream& operator<<( std::ostream& os, const Algorithm& obj )
        {
            os << obj.getInstanceInfo();
            return os;
        }

        void Algorithm::run()
        {
            process();
            requestUpdate( false );
        }
    }
}


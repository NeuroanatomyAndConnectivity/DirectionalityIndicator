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

#include "Connect.h"

namespace di
{
    namespace commands
    {
        Connect::Connect( ConstSPtr< di::core::Algorithm > from, const std::string& fromConnector,
                          ConstSPtr< di::core::Algorithm > to, const std::string& toConnector,
                          SPtr< di::core::CommandObserver > observer ):
            di::core::Command( observer ),
            m_from( from ),
            m_fromConnectorName( fromConnector ),
            m_to( to ),
            m_toConnectorName( toConnector )
        {
        }

        Connect::Connect( ConstSPtr< di::core::ConnectorBase > from, SPtr< di::core::ConnectorBase > to,
                          SPtr< di::core::CommandObserver > observer ):
            di::core::Command( observer ),
            m_fromConnector( from ),
            m_toConnector( to )
        {
        }

        Connect::~Connect()
        {
        }

        std::string Connect::getName() const
        {
            return "Connect Data";
        }

        std::string Connect::getDescription() const
        {
            return "Connect the algorithm connectors to allow data-flow between them.";
        }

        ConstSPtr< di::core::ConnectorBase > Connect::getFromConnector() const
        {
            // if the connector was already specified ... return
            if( m_fromConnector )
            {
                return m_fromConnector;
            }

            // if not, we need to find it. Note: this might throw an exception. Handled by the command-queue.
            return m_from->getOutput( m_fromConnectorName );
        }

        SPtr< di::core::ConnectorBase > Connect::getToConnector() const
        {
            // if the connector was already specified ... return
            if( m_toConnector )
            {
                return m_toConnector;
            }

            // if not, we need to find it. Note: this might throw an exception. Handled by the command-queue.
            return m_to->getInput( m_toConnectorName );
        }
    }
}

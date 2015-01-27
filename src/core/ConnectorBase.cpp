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

#include "ConnectorBase.h"

namespace di
{
    namespace core
    {
        ConnectorBase::ConnectorBase( const std::string& name, const std::string& description ):
            m_name( name ),
            m_description( description )
        {
            // nothing
        }

        ConnectorBase::~ConnectorBase()
        {
            // clean up
        }

        const std::string& ConnectorBase::getName() const
        {
            return m_name;
        }

        const std::string& ConnectorBase::getDescription() const
        {
            return m_description;
        }
    }
}


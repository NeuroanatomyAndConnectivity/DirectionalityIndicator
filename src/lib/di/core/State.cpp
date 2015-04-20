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

#include <map>
#include <string>
#include <sstream>
#include <fstream>

#include "State.h"

namespace di
{
    namespace core
    {
        void State::set( const std::string& name, const State& state )
        {
            m_keyStateStore[ name ] = state;
        }

        const std::map< std::string, std::string >& State::get() const
        {
            return m_keyValueStore;
        }

        const std::map< std::string, State >& State::getNestedStates() const
        {
            return m_keyStateStore;
        }

        std::string State::toString( const std::string& prefix ) const
        {
            std::stringstream ss;
            // write obj to stream
            for( auto kv : get() )
            {
                ss << prefix << kv.first << ":" << kv.second << std::endl;
            }

            for( auto ks : getNestedStates() )
            {
                ss << ks.second.toString( prefix + ks.first + "/" ) << std::endl;
            }

            return ss.str();
        }

        void State::toFile( const std::string& filename ) const
        {
            std::ofstream fs;
            fs.open( filename );
            fs << *this;
            fs.close();
        }
    }
}

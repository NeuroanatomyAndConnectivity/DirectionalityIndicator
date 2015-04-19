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
#include <regex>

#include <di/core/Filesystem.h>
#include <di/core/StringUtils.h>

#include "State.h"

namespace di
{
    namespace core
    {
        void State::set( const std::string& name, const State& state )
        {
            if( name.empty() )
            {
                throw std::runtime_error( "Cannot set state without name." );
            }

            // is a path?
            if( core::split( name, '/' ).size() != 1 )
            {
                throw std::runtime_error( "Cannot set state with the name being a path." );
            }

            m_keyStateStore[ name ] = state;
        }

        const std::map< std::string, std::string >& State::get() const
        {
            return m_keyValueStore;
        }

        bool State::isSet( const std::string& name ) const
        {
            return ( m_keyValueStore.count( name ) != 0 );
        }

        bool State::isState( const std::string& name ) const
        {
            return ( m_keyStateStore.count( name ) != 0 );
        }

        bool State::empty() const
        {
            return ( size() == 0 );
        }

        bool State::size() const
        {
            return m_keyStateStore.size() + m_keyValueStore.size();
        }

        size_t State::countStates() const
        {
            return m_keyStateStore.size();
        }

        size_t State::countValues() const
        {
            return m_keyValueStore.size();
        }

        const std::string& State::getValue( const std::string& name, const std::string& def ) const
        {
            if( name.empty() )
            {
                throw std::runtime_error( "Cannot get value without name." );
            }

            // is a path?
            if( core::split( name, '/' ).size() != 1 )
            {
                throw std::runtime_error( "Cannot get value with the name being a path." );
            }

            if( m_keyValueStore.count( name ) )
            {
                return m_keyValueStore.at( name );
            }
            else
            {
                return def;
            }
        }

        const State& State::getState( const std::string& name ) const
        {
            if( name.empty() )
            {
                throw std::runtime_error( "Cannot get value without name." );
            }

            // is a path?
            if( core::split( name, '/' ).size() != 1 )
            {
                throw std::runtime_error( "Cannot get value with the name being a path." );
            }

            if( m_keyStateStore.count( name ) == 0 )
            {
                throw std::runtime_error( "State not found." );
            }

            return m_keyStateStore.at( name );
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
                ss << prefix << kv.first << "=" << kv.second << std::endl;
            }

            for( auto ks : getNestedStates() )
            {
                auto stateString = ks.second.toString( prefix + ks.first + "/" );
                if( !stateString.empty() )
                {
                    // ss << "// " << prefix << std::endl;
                    ss << stateString;// << std::endl;
                }
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

        State State::fromFile( const std::string& filename )
        {
            LogD << "Loading state file \"" << filename << "\"." << LogEnd;
            auto file = core::readTextFile( filename );

            di::core::State s;

            // each line
            auto items = core::split( file );
            for( auto line : items )
            {
                if( line.empty() )
                {
                    continue;
                }

                // Ok. Split into parts:
                std::regex rgx( "(.*)=(.*)$");
                std::smatch match;

                // Matches our format?
                if( std::regex_search( line, match, rgx ) )
                {
                    s.set( match[ 1 ], match[ 2 ] );
                }
            }

            return s;
        }
    }
}

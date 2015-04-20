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

#ifndef DI_STATE_H
#define DI_STATE_H

#include <map>
#include <string>
#include <sstream>
#include <stdexcept>

#include <di/core/StringUtils.h>
#include <di/core/Conversion.h>

#include <di/GfxTypes.h>

#include <di/core/Logger.h>
#define LogTag "core/State"

namespace di
{
    namespace core
    {
        /**
         * Class represents a key-value pair collection that represents a state.
         */
        class State
        {
        public:
            /**
             * Construct and empty state.
             */
            State() = default;

            /**
             * Destructor.
             */
            virtual ~State() = default;

            /**
             * Check if the given name has a value assigned.
             *
             * \param name the name. No path allowed.
             *
             * \return true if name is valid and has a value assigned.
             */
            bool isSet( const std::string& name ) const;

            /**
             * Check if the given name has a state assigned.
             *
             * \param name the name. No path allowed.
             *
             * \return true if name is valid and has a value assigned.
             */
            bool isState( const std::string& name ) const;

            /**
             * Check if empty.
             *
             * \return true if no state or value is assigned.
             */
            bool empty() const;

            /**
             * Sum of all states and values.
             *
             * \return the number of assigned names.
             */
            bool size() const;

            /**
             * The number of states assigned.
             *
             * \return the size
             */
            size_t countStates() const;

            /**
             * The number of values.
             *
             * \return the number of values.
             */
            size_t countValues() const;

            /**
             * Set the value of the specified key
             *
             * \tparam ValueType the type of the value. Needs to provide << operator
             * \param name name. Can be a nested name in a path.
             * \param value the value
             *
             * \throw std::runtime_error if the name is invalid (empty)
             */
            template< typename ValueType >
            void set( const std::string& name, const ValueType& value )
            {
                if( name.empty() )
                {
                    throw std::runtime_error( "Cannot set value without name." );
                }

                // is a path?
                auto names = core::split( name, '/' );

                // Is a key?
                if( names.size() == 1 )
                {
                    // LogD << "Set Value: " << names[ 0 ] << LogEnd;
                    m_keyValueStore[ name ] = di::core::toString( value );
                }
                else
                {
                    // No it is part of a path. Create as many sub-states as needed
                    State* current = this;
                    // Iterate each path element, excluding the last one (the key itself)
                    for( size_t i = 0; i < names.size() - 1; ++i )
                    {
                        // LogD << "Create State: " << names[ i ] << "-" << current->m_keyStateStore.count( names[ i ] ) << LogEnd;
                        current = &current->m_keyStateStore[ names[ i ] ];
                    }

                    // The last path element is the key. Set:
                    current->set( names[ names.size() - 1 ], value );
                }
            }

            /**
             * Add a state. Useful for grouping things.
             *
             * \param name the name. Paths NOT allowed.
             * \param state the state to add
             *
             * \throw std::runtime_error if the name is invalid somehow (empty, is path).
             */
            void set( const std::string& name, const State& state );

            /**
             * Get all key/value pairs.
             *
             * \return the key-value-pairs
             */
            const std::map< std::string, std::string >& get() const;

            /**
             * Get the value at a given name as string. Returns the specified default if no value was set.
             *
             * \param name the name. Path is not allowed.
             * \param def the default to return in case of errors
             *
             * \throw std::runtime_error if the name is invalid somehow (empty, is path).
             *
             * \return the value as string
             */
            const std::string& getValue( const std::string& name, const std::string& def ) const;

            /**
             * Get the value at a given name as string. Returns the specified default if no value was set.
             *
             * \param name the name. Path is not allowed.
             * \param def the default to return in case of errors
             *
             * \tparam ValueType the desired type of the result. Provide some conversion for your type is available in di/core/Conversion.h
             *
             * \throw std::runtime_error if the name is invalid somehow (empty, is path).
             *
             * \return the value or the specified default
             */
            template< typename ValueType >
            ValueType getValue( const std::string& name, const ValueType& def ) const
            {
                auto result = getValue( name, "" );
                if( result.empty() )
                {
                    return def;
                }

                return di::core::fromString< ValueType >( result );
            }

            /**
             * Get the value at a given name as string. Returns empty state if the state is not available.
             *
             * \param name the name. Path is not allowed.
             *
             * \throw std::runtime_error if the name is invalid somehow (empty, is path).
             *
             * \return the state
             */
            State getState( const std::string& name ) const;

            /**
             * Get all nested states.
             *
             * \return the nested states
             */
            const std::map< std::string, State >& getNestedStates() const;

            /**
             * Print as string but use a given prefix.
             *
             * \param prefix the prefix to use. Contains the separator already.
             *
             * \return the string
             */
            std::string toString( const std::string& prefix = "" ) const;

            /**
             * Save to file.
             *
             * \param filename the filename
             *
             * \throw std::exception if something goes wrong.
             */
            void toFile( const std::string& filename ) const;

            /**
             * Load state from file. This method is very forgiving. Only IO errors will be reported as std::exceptions.
             *
             * \param filename the file to load
             */
            static State fromFile( const std::string& filename );

        protected:
        private:
            /**
             * Store each name and value as string in here.
             */
            std::map< std::string, std::string > m_keyValueStore;

            /**
             * Handle states separately.
             */
            std::map< std::string, State > m_keyStateStore;
        };

        /**
         * Output state to stream.
         *
         * \param os the stream to write to
         * \param obj the state to write
         *
         * \return the stream
         */
        inline std::ostream& operator<<( std::ostream& os, const State& obj )
        {
            os << obj.toString();
            return os;
        }
    }
}

#endif  // DI_STATE_H


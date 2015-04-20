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
             * Set the value of the specified key
             *
             * \tparam ValueType the type of the value. Needs to provide << operator
             * \param name name
             * \param value the value
             */
            template< typename ValueType >
            void set( const std::string& name, const ValueType& value )
            {
                std::stringstream ss;
                ss << value;
                m_keyValueStore[ name ] = ss.str();
            }

            /**
             * Add a state. Useful for grouping things.
             *
             * \param name the name
             * \param state the state to add
             */
            void set( const std::string& name, const State& state );

            /**
             * Get all key/value pairs.
             *
             * \return the key-value-pairs
             */
            const std::map< std::string, std::string >& get() const;

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


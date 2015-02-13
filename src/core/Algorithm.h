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

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <atomic>
#include <algorithm>
#include <string>

#include "ConnectorBase.h"
#include "Connector.h"
#include "ConnectorTransferable.h"

#include "Types.h"

namespace di
{
    namespace core
    {
        /**
         * Interface to define the basic operations of all algorithms.
         */
        class Algorithm
        {
        public:
            /**
             * Returns a useful name for the algorithm. Everything is allowed in the string. Please keep in mind that the user needs to grasp the
             * purpose of your algorithm using this name.
             *
             * \return the name.
             */
            virtual const std::string& getName() const;

            /**
             * Returns a useful description for the algorithm. Everything is allowed in the string. Please use a description that helps the user to
             * understand your algorithm. Refer to papers if necessary.
             *
             * \return the algorithm.
             */
            virtual const std::string& getDescription() const;

            /**
             * Process the inputs and update the outputs here. This method might be called in its own thread.
             */
            virtual void process() = 0;

            /**
             * Method checks whether this algorithm is a source. This means, whether it only has outputs and no inputs.
             *
             * \return true if it only has outputs.
             */
            virtual bool isSource() const;

            /**
             * Method checks whether this algorithm is a sink. This means, whether it only has inputs and no outputs.
             *
             * \return true if it only has inputs.
             */
            virtual bool isSink() const;

            /**
             * Check active state of the algorithm. An inactive algorithm is not notified about updates on its inputs.
             *
             * \return true if active
             */
            virtual bool isActive() const;

            /**
             * Change active-state of the algorithm.
             *
             * \param active if true (default), the algorithm is active and processes when updates arrive.
             */
            virtual void setActive( bool active = true );

            /**
             * Get the list of inputs of this algorithm.
             *
             * \return the list of inputs.
             */
            const SPtrSet< ConnectorBase >& getInputs() const;

            /**
             * Get the list of outputs of this algorithm.
             *
             * \return the list of outputs.
             */
            const ConstSPtrSet< ConnectorBase >& getOutputs() const;

            /**
             * Query an input by name.
             *
             * \note This is thread-safe since no one has non-const access to the connector vector.
             *
             * \param name name of the input to get.
             *
             * \throw std::invalid_argument if the input does not exist.
             *
             * \return the input
             */
            SPtr< ConnectorBase > getInput( const std::string& name ) const;

            /**
             * Query an input by index.
             *
             * \note This is thread-safe since no one has non-const access to the connector vector.
             *
             * \param index the index of the connector.
             *
             * \throw std::out_of_range if the connector index is invalid.
             *
             * \return the input
             */
            SPtr< ConnectorBase > getInput( size_t index ) const;

            /**
             * Query an output by name.
             *
             * \note This is thread-safe since no one has non-const access to the connector vector.
             *
             * \param name name of the output to get.
             *
             * \throw std::invalid_argument if the input does not exist.
             *
             * \return the output
             */
            ConstSPtr< ConnectorBase > getOutput( const std::string& name ) const;

            /**
             * Query an output by index.
             *
             * \note This is thread-safe since no one has non-const access to the connector vector.
             *
             * \param index the index of the connector.
             *
             * \throw std::out_of_range if the connector index is invalid.
             *
             * \return the output
             */
            ConstSPtr< ConnectorBase > getOutput( size_t index ) const;

        protected:
            /**
             * Constructor.
             *
             * \param name name of this algorithm. Everything is allowed in the string. Please keep in mind that the user needs to grasp the
             * purpose of your algorithm using this name.
             * \param description the algorithm description. This provides more detail into the algorithm. Maybe also add a Paper reference if
             * appropriate.
             */
            Algorithm( const std::string& name, const std::string& description );

            /**
             * Destructor.
             */
            virtual ~Algorithm();

            /**
             * Add an input to this algorithm. This works only during construction.
             *
             * \param input the input to add
             */
            void addInput( SPtr< ConnectorBase > input );

            /**
             * Add an output to this algorithm. This works only during construction.
             *
             * \param name the name of the algorithm data
             * \param description a useful description of the data
             *
             * \tparam DataType the type of the data itself
             *
             * \return the created data object. Keep this to get access!
             */
            template< typename DataType >
            SPtr< Connector< DataType > > addInput( const std::string& name, const std::string& description )
            {
                auto theData = SPtr< Connector< DataType > >( new Connector< DataType >( name, description ) );
                addInput( theData );
                return theData;
            }

            /**
             * Add an output to this algorithm. This works only during construction.
             *
             * \param output the output to add
             */
            void addOutput( SPtr< ConnectorBase > output );

            /**
             * Add an output to this algorithm. This works only during construction.
             *
             * \param name the name of the algorithm data
             * \param description a useful description of the data
             *
             * \tparam DataType the type of the data itself
             *
             * \return the created data object. Keep this to get access!
             */
            template< typename DataType >
            SPtr< Connector< DataType > > addOutput( const std::string& name, const std::string& description )
            {
                auto theData = SPtr< Connector< DataType > >( new Connector< DataType >( name, description ) );
                addOutput( theData );
                return theData;
            }

            /**
             * Searches the specified connector name in the given set.
             *
             * \param where the set to search
             * \param name the connector name
             *
             * \note This is thread-safe since no one has non-const access to the connector vector.
             *
             * \throw std::invalid_argument if the connector name does not exist.
             *
             * \return the connector
             */
            template< typename ContainerType >
            typename ContainerType::value_type searchConnector( const ContainerType& where, const std::string& name ) const
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

            /**
             * Searches the specified connector in the given set.
             *
             * \param where the set to search
             * \param index the connector index
             *
             * \note This is thread-safe since no one has non-const access to the connector vector.
             *
             * \throw std::out_of_range if the connector index is invalid.
             *
             * \return the connector
             */
            template< typename ContainerType >
            typename ContainerType::value_type searchConnector( const ContainerType& where, size_t index ) const
            {
                // is index in bounds?
                if( index >= where.size() )
                {
                    throw std::out_of_range( "The connector index " + std::to_string( index ) + " is invalid for algorithm \"" + getName() + "\"." );
                }

                // get the indexed element.
                return *std::next( where.begin(), index );
            }

            /**
             * This method should be called by an algorithm that somehow thinks it needs to be run again. This is the case when algorithm parameters
             * have been changed for example.
             */
            void requestUpdate();
        private:
            /**
             * Algorithm inputs. Fill during construction.
             */
            SPtrSet< ConnectorBase > m_inputs;

            /**
             * Algorithm outputs. Fill during construction.
             */
            ConstSPtrSet< ConnectorBase > m_outputs;

            /**
             * The name of the algorithm.
             */
            std::string m_name = "";

            /**
             * The description of the algorithm.
             */
            std::string m_description = "";

            /**
             * Active-state
             */
            std::atomic< bool > m_active;
        };
    }
}

#endif  // ALGORITHM_H


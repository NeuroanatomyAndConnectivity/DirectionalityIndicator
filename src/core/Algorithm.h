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
             * Get the list of inputs of this algorithm.
             *
             * \return the list of inputs.
             */
            const ConstSPtrSet< ConnectorBase >& getInputs() const;

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
            ConstSPtr< ConnectorBase > getInput( const std::string& name ) const;

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
            ConstSPtr< ConnectorBase > getInput( size_t index ) const;

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
            ConstSPtr< ConnectorBase > searchConnector( const ConstSPtrSet< ConnectorBase >& where, const std::string& name ) const;

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
            ConstSPtr< ConnectorBase > searchConnector( const ConstSPtrSet< ConnectorBase >& where, size_t index ) const;

        private:
            /**
             * Algorithm inputs. Fill during construction.
             */
            ConstSPtrSet< ConnectorBase > m_inputs;

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
        };
    }
}

#endif  // ALGORITHM_H


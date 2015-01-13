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

#include "AlgorithmDataBase.h"
#include "AlgorithmData.h"
#include "AlgorithmDataCompatible.h"

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
            const ConstSPtrSet< AlgorithmDataBase >& getInputs() const;

            /**
             * Get the list of outputs of this algorithm.
             *
             * \return the list of outputs.
             */
            const ConstSPtrSet< AlgorithmDataBase >& getOutputs() const;

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
            void addInput( SPtr< AlgorithmDataBase > input );

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
            SPtr< AlgorithmData< DataType > > addInput( const std::string& name, const std::string& description )
            {
                auto theData = SPtr< AlgorithmData< DataType > >( new AlgorithmData< DataType >( name, description ) );
                addInput( theData );
                return theData;
            }

            /**
             * Add an output to this algorithm. This works only during construction.
             *
             * \param output the output to add
             */
            void addOutput( SPtr< AlgorithmDataBase > output );

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
            SPtr< AlgorithmData< DataType > > addOutput( const std::string& name, const std::string& description )
            {
                auto theData = SPtr< AlgorithmData< DataType > >( new AlgorithmData< DataType >( name, description ) );
                addOutput( theData );
                return theData;
            }

        private:
            /**
             * Algorithm inputs. Fill during construction.
             */
            ConstSPtrSet< AlgorithmDataBase > m_inputs;

            /**
             * Algorithm outputs. Fill during construction.
             */
            ConstSPtrSet< AlgorithmDataBase > m_outputs;

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


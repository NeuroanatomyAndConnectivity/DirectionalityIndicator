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

#ifndef DI_PARAMETER_H
#define DI_PARAMETER_H

#include <string>

#include <di/core/ParameterBase.h>

#include <di/core/Logger.h>
#define LogTag "core/Parameter"

#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * Implements a class to handle parameters conveniently. Created by algorithms, these instances can be shared among UI and others. Changing
         * the value causes a notification to an observer.
         */
        template< typename ValueType >
        class Parameter: public ParameterBase
        {
        public:
            /**
             * The type of the contained value
             */
            typedef ValueType value_type;

            /**
             * Constructor. Does nothing.
             */
            Parameter( std::string name, std::string description, const ValueType& initial = ValueType() );

            /**
             * Destructor. Does nothing.
             */
            virtual ~Parameter() = default;

            /**
             * Assign a value to this parameter
             *
             * \param value the value to assign
             * \tparam CompatibleValueType the type of the value to assign. Needs to be cast-able to value_type
             *
             * \return this
             */
            template< typename CompatibleValueType >
            Parameter< ValueType >& operator=( const CompatibleValueType& value )
            {
                m_value = static_cast< const ValueType& >( value );
                notify();
                return *this;
            }

            /**
             * Dereference operator.
             *
             * \return the reference to the value
             */
            const ValueType& operator*() const;

            /**
             * Dereference-like operator.
             *
             * \return the reference to the value
             */
            const ValueType& operator()() const;

            /**
             * Convert to type.
             *
             * \return the value as value_type.
             */
            operator ValueType() const;

            /**
             * Get the value reference
             *
             * \return the value reference.
             */
            const ValueType& get() const;

            /**
             * Set the value. Triggers a notify. Only sets if \ref valid( value ) returns true
             *
             * \param value the value to set
             */
            void set( const ValueType& value );

            /**
             * Check if the value is valid.
             *
             * \param value the value to check
             *
             * \return true of valid.
             */
            bool valid( const ValueType& value );

            /**
             * Define the given functor to be used as validator. If no validator was set, all values are valid.
             *
             * \param validator the validator
             */
            void setValidator( std::function< bool( const ValueType& ) > validator );
        protected:
        private:
            /**
             * The value itself. Stored as SPtr to ease handling
             */
            ValueType m_value;

            /**
             * Validator function
             */
            std::function< bool( const ValueType& ) > m_validator = []( const ValueType& )
            {
                return true;
            };
        };

        template< typename ValueType >
        Parameter< ValueType >::Parameter( std::string name, std::string description, const ValueType& initial ):
            ParameterBase( name, description ),
            m_value( initial )
        {
        }

        template< typename ValueType >
        const ValueType& Parameter< ValueType >::operator*() const
        {
            return m_value;
        }

        template< typename ValueType >
        const ValueType& Parameter< ValueType >::operator()() const
        {
            return m_value;
        }

        template< typename ValueType >
        Parameter< ValueType >::operator ValueType() const
        {
            return m_value;
        }

        template< typename ValueType >
        const ValueType& Parameter< ValueType >::get() const
        {
            return m_value;
        }

        template< typename ValueType >
        void Parameter< ValueType >::set( const ValueType& value )
        {
            if( valid( value ) )
            {
                m_value = value;
                notify();
            }
        }

        template< typename ValueType >
        bool Parameter< ValueType >::valid( const ValueType& value )
        {
            return m_validator( value );
        }

        template< typename ValueType >
        void Parameter< ValueType >::setValidator( std::function< bool( const ValueType& ) > validator )
        {
            m_validator = validator;
        }
    }
}

#endif  // DI_PARAMETER_H


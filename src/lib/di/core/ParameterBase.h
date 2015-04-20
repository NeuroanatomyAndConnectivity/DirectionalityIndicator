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

#ifndef DI_PARAMETERBASE_H
#define DI_PARAMETERBASE_H

#include <string>
#include <ostream>

#include <di/core/Observable.h>

namespace di
{
    namespace core
    {
        class Observer;

        /**
         * Implements a class to handle parameters conveniently. Created by algorithms, these instances can be shared among UI and others. Changing
         * the value causes a notification to an observer.
         */
        class ParameterBase: public Observable
        {
        public:
            /**
             * Constructor. Does nothing.
             */
            ParameterBase( std::string name, std::string description );

            /**
             * Destructor. Does nothing.
             */
            virtual ~ParameterBase() = default;

            /**
             * Get the name of this parameter
             *
             * \return name
             */
            const std::string& getName() const;

            /**
             * Get the description of this parameter
             *
             * \return description
             */
            const std::string& getDescription() const;

            /**
             * Get the parameter itself
             *
             * \param param the param to convert.
             * \return the parameter. Nullptr if not valid (not cast-able).
             */
            template< typename ValueType >
            static ValueType as( SPtr< ParameterBase > param )
            {
                return std::dynamic_pointer_cast< typename ValueType::element_type >( param );
            }

            /**
             * Convert to string.
             *
             * \return the string
             */
            virtual std::string toString() const = 0;
        protected:
        private:
            /**
             * The name.
             */
            std::string m_name = "";

            /**
             * A description.
             */
            std::string m_description = "";
        };

        /**
         * Output parameter to stream.
         *
         * \param os the stream to write to
         * \param obj the parameter  to write
         *
         * \return the stream
         */
        inline std::ostream& operator<<( std::ostream& os, const ParameterBase& obj )
        {
            os << obj.toString();
            return os;
        }
    }
}

#endif  // DI_PARAMETERBASE_H


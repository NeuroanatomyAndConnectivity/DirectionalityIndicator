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

#ifndef CONNECTORBASE_H
#define CONNECTORBASE_H

#include <string>

#include <di/core/ConnectorTransferable.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * Base class for all kinds of data exchange between algorithms. Please keep in mind that everything that can be transferred between
         * algorithms needs to be derived from ConnectorTransferable somehow.
         */
        class ConnectorBase
        {
        public:
            /**
             * Get the name of the algorithm data object.
             *
             * \return the name
             */
            virtual const std::string& getName() const;

            /**
             * Get the description of the algorithm data object.
             *
             * \return the description
             */
            virtual const std::string& getDescription() const;

            /**
             * Check if the specified thingy is compatible with this ConnectoBase instance
             *
             * \param checkAgainst the thing to check
             *
             * \return true if yes.
             */
            virtual bool isTransferable( ConstSPtr< ConnectorTransferable > checkAgainst ) const = 0;

            /**
             * Set the transferable data, if compatible. If the given thing is not compatible, nullptr is set as new data.
             *
             * \param data the data to set. Can be nullptr.
             *
             * \return true if the data was compatible and was set
             */
            virtual bool setTransferable( ConstSPtr< ConnectorTransferable > data ) = 0;

            /**
             * Get the current transferable data.
             *
             * \return the data. Can be nullptr.
             */
            virtual ConstSPtr< ConnectorTransferable > getTransferable() const = 0;

        protected:
            /**
             * Constructor.
             *
             * \param name the name of the input. Please specify something useful.
             * \param description description of the input.
             */
            ConnectorBase( const std::string& name, const std::string& description );

            /**
             * Destructor.
             */
            virtual ~ConnectorBase();
        private:
            /**
             * The name.
             */
            std::string m_name = "";

            /**
             * The description.
             */
            std::string m_description = "";
        };
    }
}

#endif  // CONNECTORBASE_H


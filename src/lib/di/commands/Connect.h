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

#ifndef CONNECT_H
#define CONNECT_H

#include <string>

#include <di/core/CommandObserver.h>
#include <di/core/Command.h>
#include <di/core/Connector.h>
#include <di/core/Algorithm.h>

#include <di/Types.h>

namespace di
{
    namespace commands
    {
        /**
         * Implements a command to connect two algorithm connectors.
         */
        class Connect: public di::core::Command
        {
        public:
            /**
             * Constructs the command. The constructor will not check validity of its parameters, since they might not yet be valid (because the
             * algorithms are still in the command queue).
             *
             * \param from from this algorithm
             * \param fromConnector connector name of the source algorithm
             * \param to to this algorithm
             * \param toConnector connector name of the target algorithm
             * \param observer an object that gets notified upon changes in this command's state
             */
            Connect( ConstSPtr< di::core::Algorithm > from, const std::string& fromConnector,
                     ConstSPtr< di::core::Algorithm > to, const std::string& toConnector,
                     SPtr< di::core::CommandObserver > observer = nullptr );

            /**
             * Constructs the command. The constructor will not check validity of its parameters, since they might not yet be valid (because the
             * algorithms are still in the command queue).
             *
             * \param from from this connector
             * \param to to this connector
             * \param observer an object that gets notified upon changes in this command's state
             */
            Connect( ConstSPtr< di::core::ConnectorBase > from, SPtr< di::core::ConnectorBase > to,
                     SPtr< di::core::CommandObserver > observer = nullptr );

            /**
             * Clean up.
             */
            virtual ~Connect();

            /**
             * Get the human-readable title of this command. This should be something like "Adding Algorithm".
             *
             * \return the title
             */
            virtual std::string getName() const;

            /**
             * Get the human-readable description of this command. This is a more detailed description of what is going on, like "Adding a algorithm
             * to the network without connecting them".
             *
             * \return the description
             */
            virtual std::string getDescription() const;

            /**
             * Get the source connector.
             *
             * \throw std::invalid_argument if the construction-specified connector or algorithm does not exist.
             *
             * \return the connector
             */
            virtual ConstSPtr< di::core::ConnectorBase > getFromConnector() const;

            /**
             * Get the target connector.
             *
             * \throw std::invalid_argument if the construction-specified connector or algorithm does not exist.
             *
             * \return the connector
             */
            virtual SPtr< di::core::ConnectorBase > getToConnector() const;

        protected:
        private:
            /**
             * Source algorithm.
             */
            ConstSPtr< di::core::Algorithm > m_from = nullptr;

            /**
             * Source connector name.
             */
            std::string m_fromConnectorName = "";

            /**
             * Target algorithm.
             */
            ConstSPtr< di::core::Algorithm > m_to = nullptr;

            /**
             * Target connector name.
             */
            std::string m_toConnectorName = "";

            /**
             * Source connector.
             */
            ConstSPtr< di::core::ConnectorBase > m_fromConnector = nullptr;

            /**
             * Target connector.
             */
            SPtr< di::core::ConnectorBase > m_toConnector = nullptr;
        };
    }
}

#endif  // CONNECT_H


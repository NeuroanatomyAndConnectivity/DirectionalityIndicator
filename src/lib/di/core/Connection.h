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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <string>

#include <di/core/ConnectorBase.h>
#include <di/core/ConnectorObserver.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * Class represents a direct connection between two \ref Connector. As the connectors do not differentiate between in and out, the direction
         * of this connection defines the direction of the data flow. A connection is basically an observer of the source \ref ConnectorBase. This
         * way, the connection gets to know when it is "dirty" and needs an update.
         *
         * \note For now, it uses poll to check status. This will change to a push-based system using observers.
         */
        class Connection
        {
            friend std::ostream& operator<<( std::ostream& os, const Connection& obj );
        public:
            /**
             * Constructor. Represents the connection between two connectors. Please keep in mind that the data flows from the source connector to the
             * target connector only.
             *
             * \param from the source connector.
             * \param to the target connector.
             */
            Connection( ConstSPtr< ConnectorBase > from, SPtr< ConnectorBase > to );

            /**
             * Destructor.
             */
            virtual ~Connection();

            /**
             * Get the source connector.
             *
             * \return the source connector.
             */
            ConstSPtr< ConnectorBase > getSource() const;

            /**
             * Get the source connector.
             *
             * \return the source connector.
             */
            SPtr< ConnectorBase > getTarget() const;

            /**
             * Propagate changes from the source to the target.
             *
             * \return true if data changed.
             */
            bool propagate();
        protected:
        private:
            /**
             * Source connector.
             */
            ConstSPtr< ConnectorBase > m_source;

            /**
             * Target connector.
             */
            SPtr< ConnectorBase > m_target;

            /**
             * Store some information about the last transferred package. This is very useful for debugging and serves no further purpose.
             */
            std::string m_packageInfo = "0";
        };

        /**
         * Pretty-print information about this connection
         *
         * \param os the stream to write to
         * \param obj the connection to print
         *
         * \return the stream
         */
        std::ostream& operator<<( std::ostream& os, const Connection& obj );
    }
}

#endif  // CONNECTION_H


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

#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>

#include <di/core/ConnectorBase.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * This class provides the interface to connect data between algorithms. It represents a typed data.
         *
         * \tparam DataType the type of data to transfer.
         */
        template< typename DataType >
        class Connector: public ConnectorBase
        {
        public:
            /**
             * Constructor. The data is initially unset.
             *
             * \param name the name of the input. Please specify something useful.
             * \param description description of the input.
             */
            Connector( const std::string& name, const std::string& description );

            /**
             * Destructor.
             */
            virtual ~Connector();

            /**
             * The data to set. This only accepts const pointers to avoid side effects in algorithms that change data they do not own.
             * \param data the data to set. Can be nullptr.
             */
            virtual void setData( ConstSPtr< DataType > data );

            /**
             * Returns the data that has been set.
             *
             * \return the data.
             */
            virtual ConstSPtr< DataType > getData() const;

            /**
             * Check if the specified thingy is compatible with this ConnectoBase instance
             *
             * \param checkAgainst the thing to check
             *
             * \return true if yes.
             */
            virtual bool isTransferable( ConstSPtr< ConnectorTransferable > checkAgainst ) const;

            /**
             * Set the transferable data, if compatible. If the given thing is not compatible, nullptr is set as new data.
             *
             * \param data the data to set. Can be nullptr.
             *
             * \return true if the data was compatible and was set
             */
            virtual bool setTransferable( ConstSPtr< ConnectorTransferable > data );

            /**
             * Get the current transferable data.
             *
             * \return the data. Can be nullptr.
             */
            virtual ConstSPtr< ConnectorTransferable > getTransferable() const;

        protected:
        private:
            /**
             * The data to transfer.
             */
            ConstSPtr< DataType > m_data;
        };

        template< typename DataType >
        Connector< DataType >::Connector( const std::string& name, const std::string& description ):
            ConnectorBase( name, description )
        {
            // init
        }

        template< typename DataType >
        Connector< DataType >::~Connector()
        {
            // clean up. The shared_ptr clean up automatically.
        }

        template< typename DataType >
        void Connector< DataType >::setData( ConstSPtr< DataType > data )
        {
            m_data = data;
        }

        template< typename DataType >
        ConstSPtr< DataType > Connector< DataType >::getData() const
        {
            return m_data;
        }

        template< typename DataType >
        bool Connector< DataType >::isTransferable( ConstSPtr< ConnectorTransferable > checkAgainst ) const
        {
            // it is compatible if it can be cast:
            auto result = std::dynamic_pointer_cast< const DataType >( checkAgainst );
            return ( result != nullptr );
        }

        template< typename DataType >
        bool Connector< DataType >::setTransferable( ConstSPtr< ConnectorTransferable > data )
        {
            auto result = std::dynamic_pointer_cast< const DataType >( data );
            setData( result ); // NOTE: if incompatible, the pointer cast returns a nullptr. This will be set.
            return ( result != nullptr );
        }

        template< typename DataType >
        ConstSPtr< ConnectorTransferable > Connector< DataType >::getTransferable() const
        {
            return m_data;
        }
    }
}

#endif  // CONNECTOR_H


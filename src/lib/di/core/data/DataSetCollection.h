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

#ifndef DI_DATASETCOLLECTION_H
#define DI_DATASETCOLLECTION_H

#include <string>
#include <tuple>

#include <di/core/data/DataSetBase.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * This defines the interface to dataset types. In contrast to \ref DataSet, this is only a collection of attributes. No grid.
         *
         * \note Keep in mind that you can specify multiple attribute types. Optional attributes should be implemented using runtime functions.
         */
        template< typename... AttributeT >
        class DataSetCollection: public DataSetBase
        {
        public:
            /**
             * The tuple containing all attributes.
             */
            typedef std::tuple< ConstSPtr< AttributeT >... > AttributeTypes;

            /**
             * Create a new dataset.
             *
             * \param name a useful name to help the user identify this data.
             * \param attributes
             */
            DataSetCollection( const std::string& name, ConstSPtr< AttributeT >... attributes );

            /**
             * Destructor. Does NOT free the contained data. Data is freed automatically if no one keeps a reference anymore.
             */
            virtual ~DataSetCollection();

            /**
             * Get the attributes.
             *
             * \tparam Index the attribute index if any. Do not use this function on datasets without attributes.
             * \return the attributes
             */
            template< int Index = 0 >
            typename std::tuple_element< Index, AttributeTypes >::type getAttributes() const
            {
                return std::get< Index >( m_attributes );
            }

        protected:
        private:
            /**
             * The attributes
             */
            AttributeTypes m_attributes;
        };

        template< typename... AttributeT >
        DataSetCollection< AttributeT... >::DataSetCollection( const std::string& name,
                                                               ConstSPtr< AttributeT >... attributes ):
            DataSetBase( name ),
            m_attributes( std::make_tuple( attributes... ) )
        {
        }

        template< typename... AttributeT >
        DataSetCollection< AttributeT... >::~DataSetCollection()
        {
            // cleanup is done by the SPtr.
        }
    }
}

#endif  // DI_DATASETCOLLECTION_H


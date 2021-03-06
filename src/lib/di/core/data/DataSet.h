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

#ifndef DI_DATASET_H
#define DI_DATASET_H

#include <string>
#include <tuple>

#include <di/core/data/DataSetBase.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        /**
         * This defines the interface to dataset types. As nearly all kinds of data (relevant to us) is made up of a grid and a certain amount of
         * attributes, we can define a dataset to be a grid and a set of indexable attributes. The dataset itself only stores const pointers, as it is
         * not the owner of the data. Only the owner is allowed to modify the data. Create copies if you need to write to the data.
         *
         * \note Keep in mind that you can specify multiple attribute types. Optional attributes should be implemented using runtime functions.
         */
        template< typename GridT, typename... AttributeT >
        class DataSet: public DataSetBase
        {
        public:
            /**
             * The tuple containing all attributes.
             */
            typedef std::tuple< ConstSPtr< AttributeT >... > AttributeTypes;

            /**
             * The type of the grid
             */
            typedef GridT GridType;

            /**
             * Create a new dataset. Datasets only exist if they have a grid and attribute(s).
             *
             * \param name a useful name to help the user identify this data.
             * \param grid the grid
             * \param attributes
             */
            DataSet( const std::string& name, ConstSPtr< GridT > grid, ConstSPtr< AttributeT >... attributes );

            /**
             * Destructor. Does NOT free the contained data. Data is freed automatically if no one keeps a reference anymore.
             */
            virtual ~DataSet();

            /**
             * Get the grid.
             *
             * \return the grid
             */
            ConstSPtr< GridT > getGrid() const;

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
             * The grid of the dataset.
             */
            ConstSPtr< GridT > m_grid = nullptr;

            /**
             * The attributes
             */
            AttributeTypes m_attributes;
        };

        template< typename GridT, typename... AttributeT >
        DataSet< GridT, AttributeT... >::DataSet( const std::string& name,
                                                     ConstSPtr< GridT > grid,
                                                     ConstSPtr< AttributeT >... attributes ):
            DataSetBase( name ),
            m_grid( grid ),
            m_attributes( std::make_tuple( attributes... ) )
        {
        }

        template< typename GridT, typename... AttributeT >
        DataSet< GridT, AttributeT... >::~DataSet()
        {
            // cleanup is done by the SPtr.
        }

        template< typename GridT, typename... AttributeT >
        ConstSPtr< GridT > DataSet< GridT, AttributeT... >::getGrid() const
        {
            return m_grid;
        }
    }
}

#endif  // DI_DATASET_H


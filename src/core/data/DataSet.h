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

#ifndef DATASET_H
#define DATASET_H

#include <string>

#include "Types.h"

namespace di
{
    namespace core
    {
        /**
         * This defines the interface to dataset types. As nearly all kinds of data (relevant to us) is made up of a grid and a certain amount of
         * attributes, we can define a dataset to be a grid and a set of indexable attributes. The dataset itself only stores const pointers, as it is
         * not the owner of the data. Only the owner is allowed to modify the data. Create copies if you need to write to the data.
         *
         * \note Keep in mind that the type of the attribute can also be a std::tuple to create more complex datasets. Optional attributes should be
         * implemented using runtime functions.
         */
        template< typename GridType, typename AttributeType >
        class DataSet
        {
        public:
            /**
             * Create a new dataset. Datasets only exist if they have a grid and attribute(s).
             *
             * \param name a useful name to help the user identify this data.
             * \param grid the grid
             * \param attributes
             */
            DataSet( const std::string& name, ConstSPtr< GridType > grid, ConstSPtr< AttributeType > attributes );

            /**
             * Destructor. Does NOT free the contained data. Data is freed automatically if no one keeps a reference anymore.
             */
            virtual ~DataSet();

            /**
             * Get the name of this dataset.
             *
             * \return the dataset name
             */
            const std::string& getName() const;

            /**
             * Get the grid.
             *
             * \return the grid
             */
            ConstSPtr< GridType > getGrid() const;

            /**
             * Get the attributes.
             *
             * \return the attributes
             */
            ConstSPtr< AttributeType > getAttributes() const;

        protected:
        private:
            /**
             * The name
             */
            std::string m_name = "";

            /**
             * The grid of the dataset.
             */
            ConstSPtr< GridType > m_grid = nullptr;

            /**
             * The attributes
             */
            ConstSPtr< AttributeType > m_attributes = nullptr;
        };

        template< typename GridType, typename AttributeType >
        DataSet< GridType, AttributeType >::DataSet( const std::string& name,
                                                     ConstSPtr< GridType > grid,
                                                     ConstSPtr< AttributeType > attributes ):
            m_name( name ),
            m_grid( grid ),
            m_attributes( attributes )
        {
        }

        template< typename GridType, typename AttributeType >
        DataSet< GridType, AttributeType >::~DataSet()
        {
            // cleanup is done by the SPtr.
        }

        template< typename GridType, typename AttributeType >
        const std::string&  DataSet< GridType, AttributeType >::getName() const
        {
            return m_name;
        }


        template< typename GridType, typename AttributeType >
        ConstSPtr< GridType > DataSet< GridType, AttributeType >::getGrid() const
        {
            return m_grid;
        }

        template< typename GridType, typename AttributeType >
        ConstSPtr< AttributeType > DataSet< GridType, AttributeType >::getAttributes() const
        {
            return m_attributes;
        }
    }
}

#endif  // DATASET_H


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

#ifndef DATASETBASE_H
#define DATASETBASE_H

#include <string>

#include "../ConnectorTransferable.h"

#include "Types.h"

namespace di
{
    namespace core
    {
        /**
         * This defines the interface to dataset types. It only requires a name and the visibility.
         */
        class DataSetBase: public ConnectorTransferable
        {
        public:
            /**
             * Create a new dataset.
             *
             * \param name a useful name to help the user identify this data.
             */
            explicit DataSetBase( const std::string& name );

            /**
             * Destructor. Does NOT free the contained data. Data is freed automatically if no one keeps a reference anymore.
             */
            virtual ~DataSetBase();

            /**
             * Get the name of this dataset.
             *
             * \return the dataset name
             */
            const std::string& getName() const;
        protected:
        private:
            /**
             * The name
             */
            std::string m_name = "";
        };
    }
}

#endif  // DATASETBASE_H


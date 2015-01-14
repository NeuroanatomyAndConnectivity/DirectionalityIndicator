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

#ifndef DATAINJECT_H
#define DATAINJECT_H

#include <mutex>

#include "core/Algorithm.h"

namespace di
{
    namespace algorithms
    {
        class TriangleDataSet;

        /**
         * Data Inject. This class allows to inject an arbitrary ConnectorTransferable instance into the processing network.
         */
        class DataInject: public di::core::Algorithm
        {
        public:
            /**
             * Constructor. Initialize all inputs, outputs and parameters.
             */
            DataInject();

            /**
             * Destructor. Clean up if needed.
             */
            virtual ~DataInject();

            /**
             * Does nothing in this case, besides setting the injection data.
             */
            virtual void process();

            /**
             * Inject this data.
             *
             * \param data the data. Can be nullptr.
             */
            void inject( ConstSPtr< di::core::ConnectorTransferable > data );

        protected:
        private:
            /**
             * The triangle mesh input to use.
             */
            SPtr< di::core::Connector< di::core::ConnectorTransferable > > m_dataOutput;

            /**
             * Mutex to secure access.
             *
             * \note std::atomic would be a nice option, but gcc has problems there.
             */
            std::mutex m_injectionDataMutex;

            /**
             * The data to inject
             */
            ConstSPtr< di::core::ConnectorTransferable > m_injectionData;
        };
    }
}

#endif  // DATAINJECT_H


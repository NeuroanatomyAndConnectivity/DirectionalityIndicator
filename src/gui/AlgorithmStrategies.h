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

#ifndef ALGORITHMSTRATEGIES_H
#define ALGORITHMSTRATEGIES_H

#include <string>
#include <vector>

#include <QToolBox>

#include "Types.h"

namespace di
{
    namespace core
    {
        class Algorithm;
    }

    namespace gui
    {
        class AlgorithmStrategy;

        /**
         * A simple widget to switch between several bunches of algorithms, deactivating invisible ones.
         */
        class AlgorithmStrategies: public QToolBox
        {
            Q_OBJECT
        public:
            /**
             * Create the parameter widget.
             *
             * \param parent the parent widget.
             */
            explicit AlgorithmStrategies( QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~AlgorithmStrategies();

            /**
             * Allows this widget to prepare everything in the network. This is only a temporary solution.
             */
            virtual void prepareProcessingNetwork();

            /**
             * Add the given group of algorithms as strategy. Be aware that strategies added after the call to \ref prepareProcessingNetwork will not
             * have any effect to the network.
             *
             * \param strategy the strategy to add
             *
             * \return the strategy added.
             */
            virtual AlgorithmStrategy* addStrategy( AlgorithmStrategy* strategy );

            /**
             * Connect the given output to all strategy inputs with the given name.
             *
             * \param from the source algorithm
             * \param outputName the output name
             * \param inputName the target input name.
             */
            virtual void connectToAll( ConstSPtr< di::core::Algorithm > from, std::string outputName,
                                       std::string inputName );

        protected:
        protected slots:
            /**
             * Called when the active strategy changes.
             *
             * \param index the index of the new strategy
             */
            void onStrategyChange( int index );

        private:
            /**
             * The strategies managed.
             */
            std::vector< AlgorithmStrategy* > m_strategies;
        };
    }
}

#endif  // ALGORITHMSTRATEGIES_H


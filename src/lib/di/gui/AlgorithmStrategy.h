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

#ifndef DI_ALGORITHMSTRATEGY_H
#define DI_ALGORITHMSTRATEGY_H

#include <vector>
#include <string>

#include <QWidget>
#include <QVBoxLayout>

#include <di/Types.h>

namespace di
{
    namespace gui
    {
        class AlgorithmWidget;

        /**
         * A simple widget to represent a bunch of algorithms.
         */
        class AlgorithmStrategy: public QWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the parameter group widget.
             *
             * \param parent the parent widget.
             * \param name the name of the group/strategy
             */
            AlgorithmStrategy( const QString& name, QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~AlgorithmStrategy();

            /**
             * Allows this widget to prepare everything in the network. This is only a temporary solution.
             */
            virtual void prepareProcessingNetwork();

            /**
             * The name of this strategy.
             *
             * \return the name
             */
            virtual QString getName() const;

            /**
             * Add the given algorithm to this strategy.
             *
             * \param algorithm the algorithm
             *
             * \return the input algorithm
             */
            virtual AlgorithmWidget* addAlgorithm( AlgorithmWidget* algorithm );

            /**
             * Activate the strategy and its visualizations.
             *
             * \param active true to activate.
             */
            virtual void setActive( bool active = true );

            /**
             * Connect the given output to all strategy inputs with the given name.
             *
             * \param from the source algorithm
             * \param outputName the output name
             * \param inputName the target input name.
             */
            virtual void connect( ConstSPtr< di::core::Algorithm > from, std::string outputName,
                                  std::string inputName );

        protected:
        private:
            /**
             * List of algorithms in this strategy
             */
            std::vector< AlgorithmWidget* > m_algorithms;

            /**
             * Layout for all algorithm widgets
             */
            QVBoxLayout* m_layout;

            /**
             * Active strategy?
             */
            bool m_active = true;
        };
    }
}

#endif  // DI_ALGORITHMSTRATEGY_H


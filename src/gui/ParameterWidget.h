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

#ifndef PARAMETERWIDGET_H
#define PARAMETERWIDGET_H

#include <QWidget>
#include <QDockWidget>

#include "Types.h"

namespace di
{
    namespace algorithms
    {
        class DirectionalityVisualization;
    }

    namespace core
    {
        class Algorithm;
    }

    namespace gui
    {
        /**
         * A simple widget to show the algorithm parameters.
         */
        class ParameterWidget: public QDockWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the parameter widget.
             *
             * \param parent the parent widget.
             */
            explicit ParameterWidget( QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~ParameterWidget();

            /**
             * Allows this widget to prepare everything in the network. This is only a temporary solution.
             */
            void prepareProcessingNetwork();

            /**
             * The algorithm handled by this widget.
             *
             * \return the algorithm
             */
            ConstSPtr< di::core::Algorithm > getAlgorithm() const;

        protected:
        private:
            /**
             * The algorithm handled by this widget.
             */
            SPtr< di::algorithms::DirectionalityVisualization > m_algorithm  = nullptr;

        };
    }
}

#endif  // PARAMETERWIDGET_H


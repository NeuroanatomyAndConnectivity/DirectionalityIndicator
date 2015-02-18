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

#ifndef DI_DATAWIDGET_H
#define DI_DATAWIDGET_H

#include <vector>

#include <QDockWidget>
#include <QVBoxLayout>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        class Algorithm;
    }

    namespace gui
    {
        class AlgorithmStrategies;
        class FileWidget;

        /**
         * A simple widget to provide the data-loading functionality. For now, it hard-codes the required files of our use-case.
         */
        class DataWidget: public QDockWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the data widget.
             *
             * \param parent the parent widget.
             */
            explicit DataWidget( QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~DataWidget();

            /**
             * Add the given widget to the list of file widgets managed here.
             *
             * \param widget the widget to add
             */
            void addFileWidget( FileWidget* widget );

            /**
             * Allows this widget to prepare everything in the network. This is only a temporary solution.
             */
            void prepareProcessingNetwork();

            /**
             * Connect the data to the algorithm.
             *
             * \note Hard-coded. Ugly. Do not copy.
             *
             * \param to to this algorithm strategy.
             */
            void connectDataToStrategies( AlgorithmStrategies* to );

        protected:
        private:
             /**
             * The load/file widget layout.
             */
            QVBoxLayout* m_contentLayout;

            /**
             * The widget list
             */
            std::vector< FileWidget* > m_loaders;
        };
    }
}

#endif  // DI_DATAWIDGET_H


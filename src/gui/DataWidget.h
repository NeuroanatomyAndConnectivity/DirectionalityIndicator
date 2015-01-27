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

#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QDockWidget>

#include "Types.h"

class FileWidget;

namespace di
{
    namespace core
    {
        class Algorithm;
    }

    namespace gui
    {
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
             * Allows this widget to prepare everything in the network. This is only a temporary solution.
             */
            void prepareProcessingNetwork();

            /**
             * Connect the data to the algorithm.
             *
             * \note Hard-coded. Ugly. Do not copy.
             *
             * \param to to this algorithm.
             */
            void connectDataToAlgo( ConstSPtr< di::core::Algorithm > to );

        protected:
        private:
             /**
             * The mesh load widget.
             */
            FileWidget* m_meshLoad = nullptr;

            /**
             * The label load widget.
             */
            FileWidget* m_labelLoad = nullptr;
        };
    }
}

#endif  // DATAWIDGET_H


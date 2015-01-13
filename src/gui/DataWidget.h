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

#include <QWidget>
#include <QDockWidget>
#include <QToolButton>

class ScaleLabel;

namespace di
{
    namespace gui
    {
        /**
         * A simple widget to provide the data-loading functionality.
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
             * Event handler. We use it to handle \ref CommandObserverQt updates.
             *
             * \param event the event to handle
             *
             * \return true on success.
             */
            virtual bool event( QEvent* event );

        protected:
        private:
            /**
             * The label used for the labeling data
             */
            ScaleLabel* m_labelLoadLabel = nullptr;

            /**
             * The label used for the mesh data
             */
            ScaleLabel* m_meshLoadLabel = nullptr;

            /**
             * The mesh load button
             */
            QToolButton* m_meshLoadBtn = nullptr;

            /**
             * The label load button.
             */
            QToolButton* m_labelLoadBtn = nullptr;

        private slots:
            /**
             * Load the mesh data.
             */
            void loadMesh();

            /**
             * Load the label data.
             */
            void loadLabels();
        };
    }
}

#endif  // DATAWIDGET_H


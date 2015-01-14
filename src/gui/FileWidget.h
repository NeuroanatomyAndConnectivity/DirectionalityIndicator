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

#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <QIcon>
#include <QWidget>
#include <QToolButton>

#include "Types.h"

class ScaleLabel;

namespace di
{
    namespace algorithms
    {
        class DataInject;
    }

    namespace gui
    {
        /**
         * A simple widget to provide the data-loading functionality of a single file.
         */
        class FileWidget: public QWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the data widget.
             *
             * \param icon the icon to use for the buttons
             * \param parent the parent widget
             * \param fileFilter the file filter to use
             */
            FileWidget( const QIcon& icon, const QString& fileFilter, QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~FileWidget();

            /**
             * Event handler. We use it to handle \ref CommandObserverQt updates.
             *
             * \param event the event to handle
             *
             * \return true on success.
             */
            virtual bool event( QEvent* event );

            /**
             * Allows this widget to prepare everything in the network. This is only a temporary solution.
             */
            void prepareProcessingNetwork();
        protected:
        private:
            /**
             * The label used for the file data
             */
            ScaleLabel* m_fileLoadLabel = nullptr;

            /**
             * The file load button
             */
            QToolButton* m_fileLoadBtn = nullptr;

            /**
             * File filter to use in the open dialogs
             */
            QString m_fileFilter = "All Files (*.*)";

            /**
             * The data inject algorithm used to place loaded data into the network.
             */
            SPtr< di::algorithms::DataInject > m_dataInject = nullptr;

        private slots:
            /**
             * Load the file data.
             */
            void loadFile();
        };
    }
}

#endif  // FILEWIDGET_H


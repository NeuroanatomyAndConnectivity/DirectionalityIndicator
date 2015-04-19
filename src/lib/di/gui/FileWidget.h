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

#ifndef DI_FILEWIDGET_H
#define DI_FILEWIDGET_H

#include <QIcon>
#include <QWidget>
#include <QToolButton>

#include <di/algorithms/DataInject.h>
#include <di/core/Reader.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        class State;
    }

    namespace gui
    {
        class ScaleLabel;

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
             * \param title the title of the file widget
             */
            FileWidget( const QString& title, const QIcon& icon, const QString& fileFilter, QWidget* parent = nullptr );

            /**
             * Create the data widget.
             *
             * \param reader the reader to use for loading the data.
             * \param icon the icon to use for the buttons
             * \param parent the parent widget
             * \param fileFilter the file filter to use
             * \param title the title of the file widget
             */
            FileWidget( SPtr< core::Reader > reader, const QString& title, const QIcon& icon, const QString& fileFilter, QWidget* parent = nullptr );

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

            /**
             * The data inject algorithm used to place loaded data into the network.
             *
             * \return the algorithm
             */
            ConstSPtr< di::algorithms::DataInject > getDataInject() const;

            /**
             * Title of the file widget.
             *
             * \return the title
             */
            std::string getTitle() const;

            /**
             * Get the state object representing this object at the moment of the call.
             *
             * \return  the state
             */
            virtual di::core::State getState() const;

            /**
             * Apply the state to this instance.
             *
             * \param state the state to set
             *
             * \return  true if everything was fine.
             */
            virtual bool setState( const di::core::State& state );

        protected:
        private:
            /**
             * The reader to use for loading.
             */
            SPtr< core::Reader > m_reader = nullptr;

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

            /**
             * Title of the file widget.
             */
            QString m_title = "File";

            /**
             * The current file. Empty if none.
             */
            QString m_currentFile;

            /**
             * Trigger the load command.
             */
            void postLoadCommand();

        private slots:
            /**
             * Load the file data.
             */
            void loadFile();
        };
    }
}

#endif  // DI_FILEWIDGET_H


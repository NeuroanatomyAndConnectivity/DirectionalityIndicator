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

#ifndef DI_MAINWINDOW_H
#define DI_MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>

namespace di
{
    namespace gui
    {
        /**
         * The main window of the application.
         */
        class MainWindow: public QMainWindow
        {
            Q_OBJECT
        public:
            /**
             * Create the main window.
             *
             * \param parent the parent widget.
             * \param title the window title to use.
             */
            MainWindow( QString title, QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~MainWindow();

            /**
             * Loads and restores previous states  (if any). Useful to retain user GUI setup during sessions.
             */
            virtual void loadStates();

            /**
             * Save the current GUI states and sizes. Useful to retain user GUI setup during sessions.
             */
            virtual void saveStates();

            /**
             * Add a simple project file menu. Override to provide a more customized menu.
             */
            virtual void addProjectMenu();

            /**
             * Add a default menu to the application. This call all the add***Menu methods. Override for a more customized menu.
             */
            virtual void addMenu();

        signals:
            /**
             * On shutdown ...
             */
            void shutdown();

        protected:
            /**
             * Called on close. For more details, refer to QWidget::closeEvent.
             *
             * \param event the close event.
             */
            virtual void closeEvent( QCloseEvent* event );

            /**
             * Handle Qt Events.
             *
             * \param event the event
             *
             * \return true if handled.
             */
            virtual bool event( QEvent* event );

        protected slots:
            /**
             * Handle the load menu.
             */
            void loadProjectHandler();

            /**
             * Handle save project menu.
             *
             * \param filename the desired state filename.
             * \param all save all. Try by default.
             * \param viewOnly only save camera/view settings.
             * \param paramsOnly only save the parameters.
             * \param dataOnly only save the loaded data files.
             */
            void saveProjectHandler( bool all = true, bool viewOnly = false, bool paramsOnly = false, bool dataOnly = false );

            /**
             * Save parameters only. Forwards call to \ref saveProjectHandler.
             */
            void saveParamHandler();

            /**
             * Save data info only. Forwards call to \ref saveProjectHandler.
             */
            void saveDataHandler();

            /**
             * Save only camera information. Forwards call to \ref saveProjectHandler.
             */
            void saveCamHandler();
        private:
        };
    }
}

#endif  // DI_MAINWINDOW_H


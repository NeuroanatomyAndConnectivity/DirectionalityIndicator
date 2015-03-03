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

#ifndef DI_APPLICATION_H
#define DI_APPLICATION_H

#include <string>

#include <di/core/ProcessingNetwork.h>

#include <di/gui/MainWindow.h>

#include <QSettings>

#include <di/Types.h>

namespace di
{
    namespace gui
    {
        /**
         * This represents the application user interface. It handles creation/destruction of the UI and is the central entry point of the application.
         * For now, the UI is very simplistic and hard-codes all parameters and algorithms. This will change in the future. Now, it is sufficient as
         * we want to work on the visualization itself.
         *
         * This class implements the singleton pattern.
         *
         * \note This class is not virtual.
         */
        class Application
        {
            friend class MainWindow;
        public:
            /**
             * Create application instance. It does not create any GUI. To actually initialize and startup the application, use \ref run.
             *
             * \param argc argument count
             * \param argv argument array
             * \param name define a name of this application. It is used as namespace for settings and more. No spaces!
             */
            Application( std::string name, int argc, char** argv );

            /**
             * Destroy and clean up.
             */
            virtual ~Application();

            /**
             * Run the application. It initializes the application, OpenGL and the visualization framework.
             *
             * \return 0 on success and graceful shutdown. Everything else denotes an error.
             */
            int run();

            /**
             * Get the settings object of this application.
             *
             * \return the settings
             */
            static QSettings* getSettings();

            /**
             * Get the running instance.
             *
             * \return the instance.
             */
            static Application* getInstance();

            /**
             * Get the processing container controlled by this UI
             *
             * \return the container.
             */
            static SPtr< di::core::ProcessingNetwork > getProcessingNetwork();

            /**
             * Get app name. The application namespace.
             *
             * \return the name
             */
            std::string getName() const;

            /**
             * Get the main window used by the application. Add your widgets in here.
             *
             * \return the mainwindow
             */
            MainWindow* getMainWindow();

        protected:
            /**
             * Implement your specific UI code here. The network was not yet started. So only do GUI stuff here.
             */
            virtual void prepareUI() = 0;

            /**
             * Implement your specific network code here. Network was started.
             */
            virtual void prepareNetwork() = 0;

            /**
             * Implement the code that will be called after all preparations to start the UI.
             */
            virtual void show() = 0;

            /**
             * Called when using the default MainWindow on shutdown. Implement all your state-save operations here.
             */
            virtual void close() = 0;

            /**
             * On shutdown. Called externally by the main window.
             */
            void onShutdown();

        private:
            /**
             * The application main window.
             */
            MainWindow* m_mainWindow = nullptr;

            /**
             * The namespace the app is operating in.
             */
            std::string m_name;

            /**
             * Singleton instance.
             */
            static Application* m_instance;

            /**
             * Argument count.
             */
            int m_argc;

            /**
             * Argument values.
             */
            char** m_argv;

            /**
             * Application settings. Can be accessed via getSettings().
             */
            QSettings* m_settings = nullptr;

            /**
             * The processing container managed by this application instance.
             */
            SPtr< core::ProcessingNetwork > m_processingNetwork = nullptr;
        };
    }
}

#endif  // DI_APPLICATION_H


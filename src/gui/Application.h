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

#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/ProcessingNetwork.h"

#include <QSettings>

#include "Types.h"

class QToolBox;

namespace di
{
    namespace core
    {
        class Connection;
    }

    namespace gui
    {
        // Forward declarations
        class OGLWidget;
        class DataWidget;
        class MainWindow;
        class AlgorithmStrategies;

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
        public:
            /**
             * Create application instance. It does not create any GUI. To actually initialize and startup the application, use \ref run.
             *
             * \param argc
             * \param argv
             */
            Application( int argc, char** argv );

            /**
             * Destroy and clean up.
             */
            ~Application();

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

        protected:
        private:
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
             * The application main window.
             */
            MainWindow* m_mainWindow = nullptr;

            /**
             * The main graphics widget widget.
             */
            OGLWidget* m_mainGLWidget = nullptr;

            /**
             * The data-handling widget.
             */
            DataWidget* m_dataWidget = nullptr;

            /**
             * The processing container managed by this application instance.
             */
            SPtr< core::ProcessingNetwork > m_processingNetwork = nullptr;

            /**
             * Different use-cases are managed in this class.
             */
            AlgorithmStrategies* m_algorithmStrategies = nullptr;
        };
    }
}

#endif  // APPLICATION_H


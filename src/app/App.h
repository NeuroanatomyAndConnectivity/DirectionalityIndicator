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

#ifndef DI_APP_H
#define DI_APP_H

#include <di/gui/Application.h>

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
        class AlgorithmStrategies;
        class DataWidget;
    }

    namespace app
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
        class App: public di::gui::Application
        {
        public:
            /**
             * Create application instance. It does not create any GUI. To actually initialize and startup the application, use \ref run.
             *
             * \param argc
             * \param argv
             */
            App( int argc, char** argv );

            /**
             * Destroy and clean up.
             */
            ~App() override;

        protected:
            /**
             * Implement your specific UI code here. The network was not yet started. So only do GUI stuff here.
             */
            void prepareUI() override;

            /**
             * Implement your specific network code here. Network was started.
             */
            void prepareNetwork() override;

            /**
             * Implement the code that will be called after all preparations to start the UI.
             */
            void show() override;

        private:
            /**
             * The main graphics widget widget.
             */
            di::gui::OGLWidget* m_mainGLWidget = nullptr;

            /**
             * The data-handling widget.
             */
            di::gui::DataWidget* m_dataWidget = nullptr;

            /**
             * Different use-cases are managed in this class.
             */
            di::gui::AlgorithmStrategies* m_algorithmStrategies = nullptr;
        };
    }
}

#endif  // DI_APP_H


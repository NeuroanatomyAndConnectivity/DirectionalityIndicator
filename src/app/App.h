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
        class AlgorithmStrategies;
        class DataWidget;
        class ViewWidget;
        class AlgorithmWidget;
        class FileWidget;
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
            virtual~App() override;

        protected:
            /**
             * Implement your specific UI code here. The network was not yet started. So only do GUI stuff here.
             */
            virtual void prepareUI() override;

            /**
             * Implement your specific network code here. Network was started.
             */
            virtual void prepareNetwork() override;

            /**
             * Implement the code that will be called after all preparations to start the UI.
             */
            virtual void show() override;

            /**
             * Called when using the default MainWindow on shutdown. Implement all your state-save operations here.
             */
            virtual void close() override;

            /**
             * Called when the processing network is marked as dirty. Use this to re-run the network or delay or ...
             */
            virtual void onDirtyNetwork() override;

            /**
             * Called when the user wants to load a project from file. Does nothing by default. Implement this if you want to save specific things.
             *
             * \param filename the state-file to restore
             *
             * \return true if everything was OK.
             */
            virtual void loadProject( const QString& filename ) override;

            /**
             * Called when the user wants to save a project file.
             *
             * \param filename the desired state filename.
             * \param all save all. Try by default.
             * \param viewOnly only save camera/view settings.
             * \param paramsOnly only save the parameters.
             * \param dataOnly only save the loaded data files.
             */
            virtual void saveProject( const QString& filename,
                                      bool all = true, bool viewOnly = false, bool paramsOnly = false, bool dataOnly = false ) override;

            /**
             * Handle the provided switches and parameters. Return false if the provided parameters are faulty. This causes the application to stop.
             * It is your choice to either use the argument vector or argc,argv. If you do not override this method, it will allow all parameters but
             * prints a warning that they are ignored.
             *
             * \param arguments each space separated argument. It does NOT contain the program name (as argv[0] does).
             * \param argc the argument count in the char* array argv
             * \param argv the argument char* array
             *
             * \return true if everything is ok
             */
            virtual bool handleCommandLine( const std::vector< std::string >& arguments, int argc, char** argv ) override;

        private:
            /**
             * The data-handling widget.
             */
            di::gui::DataWidget* m_dataWidget = nullptr;

            /**
             * Different use-cases are managed in this class.
             */
            di::gui::AlgorithmStrategies* m_algorithmStrategies = nullptr;

            /**
             * Different use-cases are managed in this class.
             */
            di::gui::AlgorithmWidget* m_extractRegions = nullptr;

            /**
             * Algorithm property dock
             */
            QDockWidget* m_tbDock = nullptr;

            /**
             * The view
             */
            di::gui::ViewWidget* m_viewWidget = nullptr;

            /**
             * Deferred load of files. Provided before starting the processing network.
             */
            std::vector< std::string > m_deferLoad;

            /**
             * Widget to handle label orders.
             */
            gui::FileWidget* m_labelOrderFile = nullptr;

            /**
             * Widget to handle label files.
             */
            gui::FileWidget* m_labelFile = nullptr;

            /**
             * Widget to handle mesh files.
             */
            gui::FileWidget* m_meshFile = nullptr;

            /**
             * If true, the app starts, loads, takes a screenshot and quits.
             */
            bool m_screenShotMode = false;

            /**
             * The path where to store the screenshots. Needs to be absolute.
             */
            std::string m_screenShotPath;
        };
    }
}

#endif  // DI_APP_H


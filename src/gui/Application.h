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
// along with DirectionalityIndicator. If not, see <http:#www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------------------

#ifndef APPLICATION_H
#define APPLICATION_H

// Forward declarations
class OGLWidget;
class DataWidget;
class ParameterWidget;
class MainWindow;

namespace di
{
namespace gui
{
    /**
     * This represents the application user interface. It handles creation/destruction of the UI and is the central entry point of the application.
     * For now, the UI is very simplistic and hard-codes all parameters and algorithms. This will change in the future. Now, it is sufficient as we
     * want to work on the visualization itself.
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

    protected:
    private:
        /**
         * Argument count.
         */
        int m_argc;

        /**
         * Argument values.
         */
        char** m_argv;

        /**
         * The application main window.
         */
        MainWindow* m_mainWindow;

        /**
         * The main graphics widget widget.
         */
        OGLWidget* m_mainGLWidget;

        /**
         * The data-handling widget.
         */
        DataWidget* m_dataWidget;

        /**
         * Widget to show the algorithm parameters
         */
        ParameterWidget* m_parameterWidget;
    };
}
}

#endif  // APPLICATION_H


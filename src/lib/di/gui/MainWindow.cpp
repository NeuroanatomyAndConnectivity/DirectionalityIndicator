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

#include <QMainWindow>

#include <di/gui/Application.h>

#include "MainWindow.h"

namespace di
{
    namespace gui
    {
        MainWindow::MainWindow( QString title, QWidget* parent ):
            QMainWindow( parent )
        {
            setWindowTitle( title );
            setObjectName( "MainWindow" );
        }

        MainWindow::~MainWindow()
        {
        }

        void MainWindow::loadStates()
        {
            restoreGeometry( Application::getSettings()->value( "gui/geometry" ).toByteArray() );
            restoreState( Application::getSettings()->value( "gui/windowState" ).toByteArray() );
        }

        void MainWindow::saveStates()
        {
            Application::getSettings()->setValue( "gui/geometry", saveGeometry() );
            Application::getSettings()->setValue( "gui/windowState", saveState() );
        }

        void MainWindow::closeEvent( QCloseEvent* event )
        {
            saveStates();
            QMainWindow::closeEvent( event );
        }
    }
}

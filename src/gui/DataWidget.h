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

#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QDockWidget>

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

    protected:
    private:
        /**
         * The label used for the labeling data
         */
        ScaleLabel* m_labelLoadLabel;

        /**
         * The label used for the mesh data
         */
        ScaleLabel* m_meshLoadLabel;

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


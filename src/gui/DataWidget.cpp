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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

#include "ScaleLabel.h"

// include some icons as XPM. This will be replaced by a proper file loading.
#include "icons/iconMesh.xpm"
#include "icons/iconLabels.xpm"

#include "DataWidget.h"

di::gui::DataWidget::DataWidget( QWidget* parent ):
    QDockWidget( parent ),
    m_labelLoadLabel( nullptr ),
    m_meshLoadLabel( nullptr )
{
    setWindowTitle( tr( "Dataset" ) );
    setObjectName( "DataWidget" );    // needed for persistent GUI states

    // avoid closable docks.
    setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    // NOTE: for now, the file widgets are hard-coded. This might be changed in the future to allow programmers to define the data they require.

    // Create the content container
    QWidget* contentWidget( new QWidget( this ) );
    setWidget( contentWidget );
    QVBoxLayout* contentLayout( new QVBoxLayout );
    contentWidget->setLayout( contentLayout );

    // Add the mesh-load-button thingy
    QWidget* meshLoad( new QWidget );
    QHBoxLayout* meshLoadLayout( new QHBoxLayout );
    meshLoad->setLayout( meshLoadLayout );
    QToolButton* meshLoadBtn( new QToolButton );
    meshLoadBtn->setIcon( QIcon( QPixmap( iconMesh_xpm ) ) );
    meshLoadBtn->setIconSize( QSize( 64, 64 ) );
    m_meshLoadLabel = new ScaleLabel;
    meshLoadLayout->addWidget( meshLoadBtn );
    meshLoadLayout->addWidget( m_meshLoadLabel );
    m_meshLoadLabel->setText( tr( "No Data Loaded" ) );

    // Add the label-load-button thingy
    QWidget* labelLoad( new QWidget );
    QHBoxLayout* labelLoadLayout( new QHBoxLayout );
    labelLoad->setLayout( labelLoadLayout );
    QToolButton* labelLoadBtn( new QToolButton );
    labelLoadBtn->setIcon( QIcon( QPixmap( iconLabels_xpm ) ) );
    labelLoadBtn->setIconSize( QSize( 64, 64 ) );
    m_labelLoadLabel = new ScaleLabel;
    labelLoadLayout->addWidget( labelLoadBtn );
    labelLoadLayout->addWidget( m_labelLoadLabel );
    m_labelLoadLabel->setText( tr( "No Data Loaded" ) );

    contentLayout->addWidget( meshLoad );
    contentLayout->addWidget( labelLoad );
    contentLayout->setAlignment( Qt::AlignTop );

    // setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );

    // Connect the tool buttons to an actual file dialog
    connect( meshLoadBtn, SIGNAL( clicked( bool ) ), this, SLOT( loadMesh() ) );
    connect( labelLoadBtn, SIGNAL( clicked( bool ) ), this, SLOT( loadLabels() ) );
}

di::gui::DataWidget::~DataWidget()
{
}

void di::gui::DataWidget::loadMesh()
{
}

void di::gui::DataWidget::loadLabels()
{
}

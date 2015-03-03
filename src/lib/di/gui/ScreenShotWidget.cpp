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

#include <algorithm>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QDir>
#include <QComboBox>
#include <QCheckBox>

#include <di/gui/Application.h>
#include <di/gui/ScaleLabel.h>
#include <di/ext/bitmap_image.hpp>

#include "icons/folder.xpm"
#include "ScreenShotWidget.h"

#include <di/core/Logger.h>
#define LogTag "gui/ScreenShotWidget"

namespace di
{
    namespace gui
    {
        ScreenShotWidget::ScreenShotWidget( QWidget* parent ):
            QWidget( parent )
        {
            // Common 4:3 resolutions
            m_resolutions.push_back( std::make_tuple( "4:3, XGA",     1024, 768 ) );
            m_resolutions.push_back( std::make_tuple( "4:3, SXGA",    1280, 960 ) );
            m_resolutions.push_back( std::make_tuple( "4:3, UXGA",    1600, 1200 ) );
            m_resolutions.push_back( std::make_tuple( "4:3",          1920, 1440 ) );
            m_resolutions.push_back( std::make_tuple( "4:3, QXGA",    2048, 1536 ) );
            m_resolutions.push_back( std::make_tuple( "4:3, QUXGA",   3200, 2400 ) );
            m_resolutions.push_back( std::make_tuple( "4:3, HXGA",    4096, 3072 ) );
            /* m_resolutions.push_back( std::make_tuple( "4:3, HUXGA",   6400, 4800 ) );
            m_resolutions.push_back( std::make_tuple( "4:3",          8192, 6144 ) );*/

            // Common 16:9 resolutions
            m_resolutions.push_back( std::make_tuple( "16:9",           1024, 576 ) );
            m_resolutions.push_back( std::make_tuple( "16:9, HD-Ready", 1280, 720 ) );
            m_resolutions.push_back( std::make_tuple( "16:9"     ,      1600, 900 ) );
            m_resolutions.push_back( std::make_tuple( "16:9, HD",       1920, 1080 ) );
            m_resolutions.push_back( std::make_tuple( "16:9, 2K",       2048, 1152 ) );
            m_resolutions.push_back( std::make_tuple( "16:9, WQXGA+",   3200, 1800 ) );
            m_resolutions.push_back( std::make_tuple( "16:9, 4K",       4096, 2304 ) );
            /*
            m_resolutions.push_back( std::make_tuple( "16:9, UHD+",     5120, 2880 ) );
            m_resolutions.push_back( std::make_tuple( "16:9, 8K",       8192, 4608 ) );
            */

            // Samplings
            m_samples.push_back( std::make_tuple( "None", 1 ) );
            m_samples.push_back( std::make_tuple( "2x", 2 ) );
            m_samples.push_back( std::make_tuple( "4x", 4 ) );
            m_samples.push_back( std::make_tuple( "8x", 8 ) );
            m_samples.push_back( std::make_tuple( "16x", 16 ) );
            m_samples.push_back( std::make_tuple( "32x", 32 ) );


            setWindowTitle( tr( "Screenshot" ) );
            setObjectName( "ScreenShotWidget" );    // needed for persistent GUI states

            // Create the content container
            m_contentLayout = new QVBoxLayout;
            setLayout( m_contentLayout );

            // Resolution Selection
            m_resolutionCombo = new QComboBox( this );
            m_resolutionCombo->setToolTip( "Select the resolution of the recorded image." );

            for( auto res : m_resolutions )
            {
                std::string name = std::get< 0 >( res );
                std::string w = std::to_string( std::get< 1 >( res ) );
                std::string h = std::to_string( std::get< 2 >( res ) );
                std::string entry = w + " x " + h + " (" + name + ")";
                m_resolutionCombo->addItem( QString::fromStdString( entry ) );
            }
            // Add a multisample combo:
            m_sampleCombo = new QComboBox( this );
            for( auto sample : m_samples )
            {
                m_sampleCombo->addItem( QString::fromStdString( std::get< 0 >( sample ) ) );
            }

            m_sampleCombo->setToolTip(
                "The amount of samples to take for each screenshot. A higher number generates a more smooth result but takes more time."
            );

            // Path to the files:
            m_location = new QToolButton( this );
            m_location->setToolTip( "Change the location where images will be saved." );
            m_location->setIcon( QIcon( QPixmap( xpm_folder ) ) );
            m_location->setText( QDir::homePath() );
            m_location->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
            m_location->setAutoRaise( true );
            m_location->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
            connect( m_location, SIGNAL( released() ), this, SLOT( queryImagePath() ) );

            // Finish layout
            m_contentLayout->addWidget( new QLabel( "File Location", this ) );
            m_contentLayout->addWidget( m_location );
            m_contentLayout->addWidget( new QLabel( "Resolution", this ) );
            m_contentLayout->addWidget( m_resolutionCombo );
            m_contentLayout->addWidget( new QLabel( "Multi-Sampling", this ) );
            m_contentLayout->addWidget( m_sampleCombo );
            m_contentLayout->setAlignment( Qt::AlignTop );

            // Restore values:
            m_resolutionCombo->setCurrentIndex( Application::getSettings()->value( "ScreenShotWidget_Resolution", 4 ).toInt() );
            m_sampleCombo->setCurrentIndex( Application::getSettings()->value( "ScreenShotWidget_Samples", 3 ).toInt() );
            m_location->setText( Application::getSettings()->value( "ScreenShotWidget_Location", QDir::homePath() ).toString() );

            // As we want to store values, register for shutdown. The Mainwindow notifies everyone.
            connect( Application::getInstance()->getMainWindow(), SIGNAL( shutdown() ), this, SLOT( shutdown() ) );
        }

        void ScreenShotWidget::queryImagePath()
        {
            QString dir = QFileDialog::getExistingDirectory( this, tr( "Select Screenshot Directory" ), m_location->text(),
                                                             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
            if( !dir.isEmpty() )
            {
                m_location->setText( dir );
            }
        }

        int ScreenShotWidget::getWidth() const
        {
            return std::min( std::get< 1 >( m_resolutions[ m_resolutionCombo->currentIndex() ] ), m_maxRes );
        }

        int ScreenShotWidget::getHeight() const
        {
            return std::min( std::get< 2 >( m_resolutions[ m_resolutionCombo->currentIndex() ] ), m_maxRes );
        }

        int ScreenShotWidget::getSamples() const
        {
            return std::min( std::get< 1 >( m_samples[ m_sampleCombo->currentIndex() ] ), m_maxSamples );
        }

        void ScreenShotWidget::setMaxSamples( int samples )
        {
            m_maxSamples = samples;
        }

        void ScreenShotWidget::setMaxResolution( int res )
        {
            m_maxRes = res;
        }

        bool ScreenShotWidget::saveScreenShot( SPtr< core::RGBA8Image > pixels )
        {
            // Use time to construct filename
            auto now = std::time( nullptr );
            auto localTime = std::localtime( &now );    // NOLINT: do not use localtime_r

            std::ostringstream fn;
            fn << m_location->text().toStdString() << "/Screenshot_"
               << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << "_"
               << localTime->tm_hour << "-" << localTime->tm_min << "-" << localTime->tm_sec
               << ".bmp";

            LogD << "Saving screenshot to file \"" << fn.str() << "\"" << LogEnd;
            // and store as image
            bitmap_image image( pixels->getWidth(), pixels->getHeight() );

            // set background to red. If we see a red pixel -> something was wrong
            image.set_all_channels( 0, 0, 0 );

            // Now write each pixel
            for( size_t y = 0; y < pixels->getHeight(); ++y )
            {
                for( size_t x = 0; x < pixels->getWidth(); ++x )
                {
                    auto color = ( *pixels )( x, y );

                    // Bitmap has its (0,0) in the upper left corner. Fix this:
                    image.set_pixel( x, pixels->getHeight() - y - 1, color.x, color.y, color.z );
                }
            }

            // finally, save the file
            // We ensure we can open the target file before we use save_image as this only returns without any info on success:
            std::ofstream file( fn.str().c_str(), std::ios::binary );
            auto status = file.good();
            file.close();
            if( status )
            {
                try
                {
                    image.save_image( fn.str() );
                }
                catch( ... )
                {
                    LogE << "Cannot write file \"" + fn.str() + "\"." << LogEnd;
                    return false;
                }
            }
            else
            {
                LogE << "Cannot open file \"" + fn.str() + "\" for writing." << LogEnd;
                return false;
            }

            return true;
        }

        void ScreenShotWidget::shutdown()
        {
            // Save values
            Application::getSettings()->setValue( "ScreenShotWidget_Resolution", m_resolutionCombo->currentIndex() );
            Application::getSettings()->setValue( "ScreenShotWidget_Samples", m_sampleCombo->currentIndex() );
            Application::getSettings()->setValue( "ScreenShotWidget_Location", m_location->text() );
        }
    }
}


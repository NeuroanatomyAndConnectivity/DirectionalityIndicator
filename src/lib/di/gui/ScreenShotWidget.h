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

#ifndef DI_SCREENSHOTWIDGET_H
#define DI_SCREENSHOTWIDGET_H

#include <tuple>
#include <string>
#include <vector>

#include <QWidget>
#include <QComboBox>
#include <QToolButton>
#include <QCheckBox>
#include <QVBoxLayout>

#include <di/gfx/PixelData.h>
#include <di/GfxTypes.h>
#include <di/Types.h>

namespace di
{
    namespace gui
    {
        class ScaleLabel;
        class ColorPicker;

        /**
         * A simple widget to provide screenshot functionality.
         */
        class ScreenShotWidget: public QWidget
        {
            Q_OBJECT
        public:
            /**
             * Create the widget.
             *
             * \param parent the parent widget.
             */
            explicit ScreenShotWidget( QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~ScreenShotWidget() = default;

            /**
             * Get width of the screenshot
             *
             * \return the width
             */
            int getWidth() const;

            /**
             * Get height of the screenshot
             *
             * \return the height
             */
            int getHeight() const;

            /**
             * Get the number of samples for the screenshot
             *
             * \return the number of samples
             */
            int getSamples() const;

            /**
             * Set the max number of samples
             *
             * \param samples the max num of samples
             */
            void setMaxSamples( int samples );

            /**
             * Set maximum resolution for screenshots.
             *
             * \param res the max resolution
             */
            void setMaxResolution( int res );

            /**
             * Save the pixel data as screenshot.
             *
             * \param pixels the image
             * \param nameHint hint how to name the file.
             *
             * \return false on error
             */
            bool saveScreenShot( SPtr< core::RGBA8Image > pixels, const std::string& nameHint );

            /**
             * Check if the background should be different from the default
             *
             * \return true if the screenshot-color should be used
             */
            bool getBackgroundOverride() const;

            /**
             * Get the currently set default background color.
             *
             * \return the color
             */
            di::Color getBackgroundColor() const;

            /**
             * Checks if the user wants all views to be captured.
             *
             * \return true if enabled.
             */
            bool getCaptureAll() const;
        protected slots:
            /**
             * Handle shutdown. Emited by the main windows
             */
            virtual void shutdown();

            /**
             * Query the image path for screenshots.
             */
            virtual void queryImagePath();
        private:
             /**
             * The load/file widget layout.
             */
            QVBoxLayout* m_contentLayout;

            /**
             * Max amount of samples.
             */
            int m_maxSamples = 4;

            /**
             * Max amount of samples.
             */
            int m_maxRes = 4;

            /**
             * The resolutions to support.
             */
            std::vector< std::tuple< std::string, int, int > > m_resolutions;

            /**
             * The supported samples.
             */
            std::vector< std::tuple< std::string, int > > m_samples;

            /**
             * Sampling combobox
             */
            QComboBox* m_sampleCombo = nullptr;

            /**
             * Resolution combobox.
             */
            QComboBox* m_resolutionCombo = nullptr;

            /**
             * Where to place the screenshots.
             */
            QToolButton* m_location = nullptr;

            /**
             * Choose a BG color for screenshots.
             */
            ColorPicker* m_bgColor = nullptr;

            /**
             * Override default background with this one.
             */
            QCheckBox* m_bgColorOverride = nullptr;

            /**
             * Checkbox to force screenshots of all default views.
             */
            QCheckBox* m_allDefaultViews = nullptr;
        };
    }
}

#endif  // DI_SCREENSHOTWIDGET_H


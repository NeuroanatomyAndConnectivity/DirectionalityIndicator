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

#ifndef DI_OBSERVERQT_H
#define DI_OBSERVERQT_H

#include <vector>
#include <memory>
#include <functional>

#include <QWidget>

#include <di/core/Observer.h>

namespace di
{
    namespace gui
    {
        /**
         * A wrapper around \ref di::core::Observer to enable GUI feedback. It forwards the event to the GUI using specific QEvents.
         */
        class ObserverQt: public di::core::Observer,
                          public std::enable_shared_from_this< ObserverQt >
        {
        public:
            /**
             * Constructor. Does nothing.
             *
             * \param receiver the widget that handles the event.
             * \param affected the widgets that caused the command. Can be the same as receiver.
             */
            ObserverQt( QWidget* receiver, std::vector< QWidget* > affected = {} );

            /**
             * Constructor. Does nothing.
             *
             * \param receiver the widget that handles the event.
             * \param affected the widgets that caused the command. Can be the same as receiver.
             * \param callback allows to define a callback in the Qt thread. This will not be called by this event automatically. You have to take
             * care of it in your event handler.
             */
            ObserverQt( QWidget* receiver, std::function<void()> callback, std::vector< QWidget* > affected = {} );

            /**
             * Destructor. Does nothing.
             */
            virtual ~ObserverQt();

            /**
             * Notification. Implemented using QEvents
             */
            virtual void notify();

            /**
             * The receiver widget.
             *
             * \return the receiver widget.
             */
            QWidget* getReceiver() const;

            /**
             * Get the affected widgets. Can be empty.
             *
             * \return the affected widgets.
             */
            const std::vector< QWidget* >& getAffected() const;

            /**
             * Call the specified callback, if any. Use this in your Qt event handler. If no callback was set, nothing happens.
             */
            void callback();

        protected:
        private:
            /**
             * The widget to receive the event.
             */
            QWidget* m_receiver = nullptr;

            /**
             * The widget that is affected by the command. This might be the receiver, but sometimes you construct widgets using others.
             */
            std::vector< QWidget* > m_affected = {};

            /**
             * A callback that will be called in the Qt thread. Useful to implant further functionality without extending this class.
             */
            std::function<void()> m_callback;

            /**
             * true if a callback was set
             */
            bool m_hasCallback = false;
        };
    }
}

#endif  // DI_OBSERVERQT_H


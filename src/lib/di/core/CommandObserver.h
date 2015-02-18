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

#ifndef COMMANDOBSERVER_H
#define COMMANDOBSERVER_H

#include <di/core/Observer.h>

#include <di/Types.h>

namespace di
{
    namespace core
    {
        class Command;

        /**
         * Implements a class to monitor changes in a \ref Command. It uses the observer pattern to inform anyone interested in changes. This class is
         * useful for implementing GUI interaction with the visualization system and its commands.
         *
         * This specific observer, namely the CommandObserver provides some methods that match the Command interface. If you do not override them,
         * they will always call the notify function.
         *
         * \note All notifications provide a shared_ptr to the calling command. This is important, as there is no other way to query the command that
         * triggered the notification (unless, of course, you manage a list of obsevers<->commands). The easy identification of the calling command
         * allows to use one observer instance for multiple commands.
         */
        class CommandObserver: public Observer
        {
        public:
            /**
             * Mark the command as being processed right now.
             *
             * \param command the command that issued this notification.
             */
            virtual void busy( SPtr< Command > command ) = 0;

            /**
             * The command is now waiting in the command-queue.
             *
             * \param command the command that issued this notification.
             */
            virtual void waiting( SPtr< Command > command ) = 0;

            /**
             * Called after finishing the command successfully.
             *
             * \param command the command that issued this notification.
             */
            virtual void success( SPtr< Command > command ) = 0;

            /**
             * Called if the command was aborted.
             *
             * \param command the command that issued this notification.
             */
            virtual void abort( SPtr< Command > command ) = 0;

            /**
             * Called if the command failed somehow.
             *
             * \param command the command that issued this notification.
             */
            virtual void fail( SPtr< Command > command ) = 0;

        protected:
            /**
             * Constructor. Does nothing.
             */
            CommandObserver();

            /**
             * Destructor. Does nothing.
             */
            virtual ~CommandObserver();

        private:
        };
    }
}

#endif  // COMMANDOBSERVER_H


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

#ifndef COMMANDOBSERVER_H
#define COMMANDOBSERVER_H

#include "Observer.h"

namespace di
{
    namespace core
    {
        /**
         * Implements a class to monitor changes in a \ref Command. It uses the observer pattern to inform anyone interested in changes. This class is
         * useful for implementing GUI interaction with the visualization system and its commands.
         *
         * This specific observer, namely the CommandObserver provides some methods that match the Command interface. If you do not override them,
         * they will always call the notify function.
         */
        class CommandObserver: public Observer
        {
        public:
            /**
             * Mark the command as being processed right now.
             */
            virtual void busy();

            /**
             * The command is now waiting in the command-queue.
             */
            virtual void waiting();

            /**
             * Called after finishing the command successfully.
             */
            virtual void success();

            /**
             * Called if the command was aborted.
             */
            virtual void abort();

            /**
             * Called if the command failed somehow.
             */
            virtual void fail();
        protected:
        private:
        };
    }
}

#endif  // COMMANDOBSERVER_H


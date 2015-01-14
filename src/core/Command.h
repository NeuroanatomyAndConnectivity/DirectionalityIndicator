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

#ifndef COMMAND_H
#define COMMAND_H

#include <memory>
#include <exception>
#include <string>

#include "CommandObserver.h"

#include "Types.h"

namespace di
{
    namespace core
    {
        /**
         * Implements a command. It represents any command that needs to be executed by the visualization system. Before a command does anything, it
         * has to be added to a command-queue. The command-queue marks it as "waiting". The moment where the command is forwarded for processing, it
         * is marked "busy". It now can either be "done", "failed", or "aborted".
         *
         * You can specify an CommandObserver instance during construction to get informed about changes in the command's state.
         *
         * Derive to implement specific commands.
         */
        class Command: public std::enable_shared_from_this< Command >
        {
        public:
            /**
             * Create an empty command. Derive to add a meaning.
             *
             * \param observer an object that gets notified upon changes in this command's state.
             */
            explicit Command( SPtr< CommandObserver > observer = nullptr );

            /**
             * Clean up.
             */
            virtual ~Command();

            /**
             * Get the human-readable title of this command. This should be something like "Calculating Gradients".
             *
             * \return the title
             */
            virtual std::string getTitle() const = 0;

            /**
             * Get the human-readable description of this command. This is a more detailed description of what is going on, like "Calculating the
             * gradients using the differential quotient on scalar data.".
             *
             * \return the description
             */
            virtual std::string getDescription() const = 0;

            /**
             * Get the current observer, if any. If not, nullptr is returned.
             *
             * \return the observer or nullptr.
             */
            virtual SPtr< CommandObserver > getObserver() const;

            // Predefined functionality for status reports:

            /**
             * Checks if the command is now done. This includes the positive and negative cases; in contrast to \ref isSuccessful.
             *
             * \return true == isSuccessful || isAborted || isFailed.
             */
            virtual bool isDone() const;

            /**
             * Mark the command as being processed right now.
             */
            virtual void busy();

            /**
             * Checks for the busy-state. If the command is currently running, it is marked busy.
             *
             * \return true if busy.
             */
            virtual bool isBusy() const;

            /**
             * The command is now waiting in the command-queue.
             */
            virtual void waiting();

            /**
             * The command is waiting in the command-queue.
             *
             * \return true if waiting in the queue.
             */
            virtual bool isWaiting() const;

            /**
             * Called after finishing the command successfully.
             */
            virtual void success();

            /**
             * Checks for the finished-state. If the command has been finished successfully, this is set to true.
             *
             * \return true if done.
             */
            virtual bool isSuccessful() const;

            /**
             * Called if the command was aborted.
             */
            virtual void abort();

            /**
             * Checks for the abort-state. If true, the command was aborted and has not finished.
             *
             * \return true if aborted.
             */
            virtual bool isAborted() const;

            /**
             * Called if the command failed somehow.
             *
             * \param reason the reason why it failed. Specify something useful.
             */
            virtual void fail( const std::string& reason );

            /**
             * Called if the command failed due to an exception. Always use this if you are catching an exception.
             *
             * \param reason the reason why it failed.
             */
            virtual void fail( const std::exception& reason );

            /**
             * Checks for the fail-state. If true, the command failed due to a certain reason.
             *
             * \return true if failed.
             */
            virtual bool isFailed() const;

            /**
             * Query the reason for the failure.
             *
             * \return the reason or an empty string if not failed.
             */
            virtual const std::string& getFailureReason() const;

        protected:
        private:
            /**
             * The observer
             */
            SPtr< CommandObserver > m_observer = nullptr;

            /**
             * State: waiting.
             */
            bool m_isWaiting = false;

            /**
             * State: busy.
             */
            bool m_isBusy = false;

            /**
             * State: done successfully.
             */
            bool m_isSuccessful = false;

            /**
             * State: aborted.
             */
            bool m_isAborted = false;

            /**
             * State: failed.
             */
            bool m_isFailed = false;

            /**
             * The reason for failure.
             */
            std::string m_failureReason = "";
        };
    }
}

#endif  // COMMAND_H


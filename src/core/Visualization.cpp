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

#include <string>

#define LogTag "core/Visualization"
#include "Logger.h"
#include "Reader.h"

#include "commands/ReadFile.h"

#include "io/PlyReader.h"

#include "Visualization.h"

namespace di
{
    namespace core
    {
        Visualization::Visualization():
            CommandQueue()
        {
        }

        Visualization::~Visualization()
        {
        }

        void Visualization::start()
        {
            // Fill the list of readers. IMPORTANT: in the future, readers will be added dynamically (loaded from DLLs/SOs/DyLibs)
            m_reader.push_back( SPtr< di::io::PlyReader >( new di::io::PlyReader() ) );

            CommandQueue::start();
        }

        void Visualization::stop()
        {
            CommandQueue::stop();
        }

        void Visualization::loadFile( const std::string& fileName, SPtr< CommandObserver > observer )
        {
            commit(
                SPtr< Command >(
                    new di::commands::ReadFile( fileName, observer )
                )
            );
        }

        void Visualization::process( SPtr< Command > command )
        {
            // IMPORTANT: in the future, it is planned to make this dynamic... for now, it is sufficient to do it the hard-coded way as we need to get
            // started with the visualization itself.

            LogD << "Processing command: \"" << command->getTitle() << "\"" << LogEnd;

            // Is a ReadFile command?
            SPtr< di::commands::ReadFile > readFile = std::dynamic_pointer_cast< di::commands::ReadFile >( command );
            if( readFile )
            {
                std::string fn = readFile->getFilename();
                LogD << "Try loading: \"" << fn << "\"" << LogEnd;
                // iterate all known readers to find the best:
                for( auto aReader : m_reader )
                {
                    if( aReader->canLoad( fn ) )
                    {
                        // NOTE: exceptions get handled in CommandQueue
                        aReader->load( fn );
                        break;
                    }
                }
            }
        }
    }
}

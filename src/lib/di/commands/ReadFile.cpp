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

#include <di/core/Reader.h>

#include "ReadFile.h"

namespace di
{
    namespace commands
    {
        ReadFile::ReadFile( const std::string& filename, SPtr< di::core::CommandObserver > observer ):
            ReadFile( nullptr, filename, observer )
        {
        }

        ReadFile::ReadFile( SPtr< core::Reader > reader, const std::string& filename,
                            SPtr< di::core::CommandObserver > observer ):
            Command( observer ),
            m_filename( filename ),
            m_reader( reader )
        {
        }

        ReadFile::~ReadFile()
        {
        }

        std::string ReadFile::getName() const
        {
            return "Read File";
        }

        std::string ReadFile::getDescription() const
        {
            return "Read a file from disk. This command tries to use the optimal loader.";
        }

        std::string ReadFile::getFilename() const
        {
            return m_filename;
        }

        SPtr< di::core::DataSetBase > ReadFile::getResult() const
        {
            return m_result;
        }

        void ReadFile::setResult( SPtr< di::core::DataSetBase > result )
        {
            m_result = result;
        }

        SPtr< core::Reader > ReadFile::getReader() const
        {
            return m_reader;
        }
    }
}

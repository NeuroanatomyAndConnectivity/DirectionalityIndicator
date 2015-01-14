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

#include "AddAlgorithm.h"

namespace di
{
    namespace commands
    {
        AddAlgorithm::AddAlgorithm( SPtr< di::core::Algorithm > algorithm, SPtr< di::core::CommandObserver > observer ):
            Command( observer ),
            m_algorithm( algorithm )
        {
        }

        AddAlgorithm::~AddAlgorithm()
        {
        }

        std::string AddAlgorithm::getTitle() const
        {
            return "Add Algorithm";
        }

        std::string AddAlgorithm::getDescription() const
        {
            return "Add a algorithm as node to the processing network without connecting it.";
        }

        SPtr< di::core::Algorithm > AddAlgorithm::getAlgorithm() const
        {
            return m_algorithm;
        }
    }
}

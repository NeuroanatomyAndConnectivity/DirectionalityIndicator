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

#ifndef TYPES_H
#define TYPES_H

#include <memory>

namespace di
{
    /**
     * Alias for abbreviating the often used std::shared_ptr< Type >.
     *
     * \tparam T the type to embed into the shared_ptr.
     */
    template< typename T > using SPtr = std::shared_ptr< T >;

    /**
     * Alias for abbreviating the often used std::shared_ptr< Type >.
     *
     * \tparam T the type to embed into the shared_ptr.
     */
    template< typename T > using ConstSPtr = std::shared_ptr< const T >;
}

#endif  // TYPES_H


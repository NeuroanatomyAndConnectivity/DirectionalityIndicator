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

#ifndef DI_TYPES_H
#define DI_TYPES_H

#include <vector>
#include <set>

// shared_ptr
#include <memory>

// NOTE: This file is mostly used for type abbreviations to make life easier.
// Thanks to the new C++11 "using" statement, you can also define "templated" type aliases. Example: alias a vector of shared_ptr:
//  template< typename T >
//  using SPtrVec = std::vector< std::shared_ptr< T > >
// ... Nice, isn't it?

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

    /**
     * Alias for abbreviating the often used std::weak_ptr< Type >.
     *
     * \tparam T the type to embed into the weak_ptr.
     */
    template< typename T > using WPtr = std::weak_ptr< T >;

    /**
     * Alias for abbreviating the often used std::weak_ptr< Type >.
     *
     * \tparam T the type to embed into the weak_ptr.
     */
    template< typename T > using ConstWPtr = std::weak_ptr< const T >;

    /**
     * Alias for abbreviating often used shared pointer vector containers
     *
     * \tparam T the type in the shared_ptr vector.
     */
    template< typename T >
    using SPtrVec = std::vector< std::shared_ptr< T > >;

    /**
     * Alias for abbreviating often used shared pointer vector containers
     *
     * \tparam T the type in the shared_ptr vector.
     */
    template< typename T >
    using ConstSPtrVec = std::vector< std::shared_ptr< const T > >;

    /**
     * Alias for abbreviating often used shared pointer set containers
     *
     * \tparam T the type in the shared_ptr vector.
     */
    template< typename T >
    using SPtrSet = std::set< std::shared_ptr< T > >;

    /**
     * Alias for abbreviating often used shared pointer set containers
     *
     * \tparam T the type in the shared_ptr vector.
     */
    template< typename T >
    using ConstSPtrSet = std::set< std::shared_ptr< const T > >;
}

#endif  // DI_TYPES_H


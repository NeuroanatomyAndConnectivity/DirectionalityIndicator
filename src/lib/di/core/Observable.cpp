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

#include <algorithm>

#include <di/core/Observer.h>

#include "Observable.h"

namespace di
{
    namespace core
    {
        void Observable::observe( SPtr< Observer > observer )
        {
            // Used to lock the command queue mutex
            std::unique_lock< std::mutex > lock( m_observersMutex );
            m_observers.push_back( observer );
        }

        void Observable::observe( Observer* observer )
        {
            // Used to lock the command queue mutex
            std::unique_lock< std::mutex > lock( m_observersMutex );
            m_observersPtr.push_back( observer );
        }

        void Observable::notify()
        {
            std::unique_lock< std::mutex > lock( m_observersMutex );
            for( auto o : m_observers )
            {
                o->notify();
            }
        }

        void Observable::removeObserver( SPtr< Observer > observer )
        {
            std::unique_lock< std::mutex > lock( m_observersMutex );
            m_observers.erase( std::remove( m_observers.begin(),
                                            m_observers.end(),
                                            observer ), m_observers.end() );
        }

        void Observable::removeObserver( Observer* observer )
        {
            std::unique_lock< std::mutex > lock( m_observersMutex );
            m_observersPtr.erase( std::remove( m_observersPtr.begin(),
                                               m_observersPtr.end(),
                                               observer ), m_observersPtr.end() );
        }
    }
}


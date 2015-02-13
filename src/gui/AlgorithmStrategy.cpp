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

#include <QWidget>

#include "Application.h"
#include "AlgorithmWidget.h"

#include "AlgorithmStrategy.h"

namespace di
{
    namespace gui
    {
        AlgorithmStrategy::AlgorithmStrategy( const QString& name, QWidget* parent ):
            QWidget( parent )
        {
            // Init
            setWindowTitle( name );
        }

        AlgorithmStrategy::~AlgorithmStrategy()
        {
        }

        void AlgorithmStrategy::prepareProcessingNetwork()
        {
            for( auto a : m_algorithms )
            {
                a->prepareProcessingNetwork();
            }
        }

        QString AlgorithmStrategy::getName() const
        {
            return windowTitle();
        }

        AlgorithmWidget* AlgorithmStrategy::addAlgorithm( AlgorithmWidget* algorithm )
        {
            m_algorithms.push_back( algorithm );
            setActive( m_active ); // force active state to new algo
            return algorithm;
        }

        void AlgorithmStrategy::setActive( bool active )
        {
            m_active = active;
            for( auto a : m_algorithms )
            {
                a->setActive( active );
            }
        }

        void AlgorithmStrategy::connect( ConstSPtr< di::core::Algorithm > from, std::string outputName,
                                         std::string inputName )
        {
            for( auto a : m_algorithms )
            {
                Application::getProcessingNetwork()->connectAlgorithms( from, outputName, a->getAlgorithm(), inputName );
            }
        }
    }
}


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

#include <di/core/data/TriangleDataSet.h>
#include <di/core/data/LineDataSet.h>
#include <di/core/data/Lines.h>

#define LogTag "algorithms/ExtractRegions"
#include <di/core/Logger.h>

#include "ExtractRegions.h"

namespace di
{
    namespace algorithms
    {
        ExtractRegions::ExtractRegions():
            Algorithm( "Extract Regions",
                       "Extract regions on a given triangle dataset defined by different colors." )
        {
            // 1: the output
            m_dataOutput = addOutput< di::core::LineDataSet >(
                    "Regions",
                    "Extracted regions as lines."
            );

            // 2: the input
            m_dataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle data to process."
            );
        }

        ExtractRegions::~ExtractRegions()
        {
            // nothing to clean up so far
        }

        void ExtractRegions::process()
        {
            // Get input data
            auto triangleDataSet = m_dataInput->getData();
            auto triangles = triangleDataSet->getGrid();
            auto attribute = triangleDataSet->getAttributes< 0 >(); // the color in our case

            // Create the buffers we need to store lines and color information
            auto lines = std::make_shared< di::core::Lines >();
            auto colors = std::make_shared< di::RGBAArray >();

            // Iterate all triangles and transform to lines
            for( auto t : triangles->getTriangles() )
            {
                // t is an ivec3 -> 3 indices to color, vertex, normal
                auto c1 = attribute->operator[]( t.x );
                auto c2 = attribute->operator[]( t.y );
                auto c3 = attribute->operator[]( t.z );

                // Always add the triangle itself?
                bool includeTriangle = true;
                if( includeTriangle )
                {
                    // Get Vertices, add to line data, connect ... nothing fancy here.
                    auto v1 = triangles->getVertex( t.x );
                    auto v2 = triangles->getVertex( t.y );
                    auto v3 = triangles->getVertex( t.z );
                    auto v1I = lines->addVertex( v1 );
                    auto v2I = lines->addVertex( v2 );
                    auto v3I = lines->addVertex( v3 );
                    lines->addLine( v1I, v2I );
                    lines->addLine( v1I, v3I );
                    lines->addLine( v2I, v3I );
                    float g = 0.5;
                    float a = 0.25;
                    colors->push_back( glm::vec4( g, g, g, a ) );
                    colors->push_back( glm::vec4( g, g, g, a ) );
                    colors->push_back( glm::vec4( g, g, g, a ) );
                }

                // There are two cases now:
                //  1: c1,c2,c3 are the same -> triangle is not a border triangle -> do not add a border line
                if( ( c1 == c2 ) && ( c2 == c3 ) )
                {
                    continue;
                }
                else
                {
                    float arrowLength = 3.00f;

                    // 2: the more interesting case: each color is different to each other.
                    if( ( c1 != c2 ) && ( c1 != c3 ) && ( c2 != c3 ) )
                    {
                        // all vertices have a different attribute value
                        // Here, it is not sufficient to connect the middle of each edge (as below), we need to add a center vertex to split the
                        // triangle into three regions.

                        // -> Calc center vertex of triangle
                        auto v1 = triangles->getVertex( t.x );
                        auto v2 = triangles->getVertex( t.y );
                        auto v3 = triangles->getVertex( t.z );
                        auto c = ( v1 + v2 + v3 ) / 3.0f;

                        // get a normal
                        auto nd = triangles->getNormal( t.x );
                        auto ne1 = triangles->getNormal( t.y );
                        auto ne2 = triangles->getNormal( t.z );
                        auto n = glm::normalize( ( nd + ne1 + ne2 ) / 3.0f );

                        // build border lines from each edge-middle-point to the center
                        auto em1 = v1 + ( 0.5f * ( v2 - v1 ) );
                        auto em2 = v1 + ( 0.5f * ( v3 - v1 ) );
                        auto em3 = v2 + ( 0.5f * ( v3 - v2 ) );

                        // Add these vertices
                        auto em1I = lines->addVertex( em1 );
                        auto em2I = lines->addVertex( em2 );
                        auto em3I = lines->addVertex( em3 );
                        auto cI = lines->addVertex( c );

                        // Do not forget to add enough colors
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                        colors->push_back( glm::vec4( 1.0, 0.0, 0.0, 1.0 ) );

                        // and connect edge middle points to center with lines:
                        lines->addLine( em1I, cI );
                        lines->addLine( em2I, cI );
                        lines->addLine( em3I, cI );

                        // An arrow is derived from the newly added border and the triangle normal:
                        auto arrow1 = glm::cross( n , glm::normalize( em1 - c ) );
                        auto arrow2 = glm::cross( n , glm::normalize( em2 - c ) );
                        auto arrow3 = glm::cross( n , glm::normalize( em3 - c ) );

                        auto arrI1 = lines->addVertex( em1 + arrowLength * arrow1 );
                        auto arrI2 = lines->addVertex( em1 - arrowLength * arrow1 );
                        auto arrI3 = lines->addVertex( em2 + arrowLength * arrow2 );
                        auto arrI4 = lines->addVertex( em2 - arrowLength * arrow2 );
                        auto arrI5 = lines->addVertex( em3 + arrowLength * arrow3 );
                        auto arrI6 = lines->addVertex( em3 - arrowLength * arrow3 );

                        lines->addLine( em1I, arrI1 );
                        lines->addLine( em1I, arrI2 );
                        lines->addLine( em2I, arrI3 );
                        lines->addLine( em2I, arrI4 );
                        lines->addLine( em3I, arrI5 );
                        lines->addLine( em3I, arrI6 );

                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                    }
                    else
                    {
                        // 3: finally, the last case -> one color is different from two others

                        // search the index of the one different color.
                        size_t differentI = 0;
                        size_t otherI1 = 0;
                        size_t otherI2 = 0;
                        if( ( c1 != c2 ) && ( c1 != c3 ) )
                        {
                            differentI = t.x;
                            otherI1 = t.y;
                            otherI2 = t.z;
                        }
                        else if( ( c2 != c1 ) && ( c2 != c3 ) )
                        {
                            differentI = t.y;
                            otherI1 = t.x;
                            otherI2 = t.z;
                        }
                        else if( ( c3 != c1 ) && ( c3 != c2 ) )
                        {
                            differentI = t.z;
                            otherI1 = t.x;
                            otherI2 = t.y;
                        }

                        // get the vertices for it
                        auto vd = triangles->getVertex( differentI );
                        auto ve1 = triangles->getVertex( otherI1 );
                        auto ve2 = triangles->getVertex( otherI2 );

                        // get a normal
                        auto nd = triangles->getNormal( differentI );
                        auto ne1 = triangles->getNormal( otherI1 );
                        auto ne2 = triangles->getNormal( otherI2 );
                        auto n = glm::normalize( ( nd + ne1 + ne2 ) / 3.0f );

                        // find the new vertices of the border. It just is the middle of the edge
                        auto bv1 = vd + ( 0.5f * ( ve1 - vd ) );
                        auto bv2 = vd + ( 0.5f * ( ve2 - vd ) );

                        // use this as border line segment -> add to lines
                        auto li1 = lines->addVertex( bv1 );
                        auto li2 = lines->addVertex( bv2 );
                        lines->addLine( li1, li2 );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );

                        // An arrow is derived from the newly added border and the triangle normal:
                        auto arrow = glm::cross( n , glm::normalize( bv2 - bv1 ) );
                        auto arrI1 = lines->addVertex( bv1 + arrowLength * arrow );
                        auto arrI2 = lines->addVertex( bv2 + arrowLength * arrow );
                        auto arrI3 = lines->addVertex( bv1 - arrowLength * arrow );
                        auto arrI4 = lines->addVertex( bv2 - arrowLength * arrow );
                        lines->addLine( li1, arrI1 );
                        lines->addLine( li2, arrI2 );
                        lines->addLine( li1, arrI3 );
                        lines->addLine( li2, arrI4 );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 0.5 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 0.5 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                        colors->push_back( glm::vec4( 0.0, 1.0, 0.0, 1.0 ) );
                    }
                }
            }

            // Create line dataset and set output
            m_dataOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Borders", lines, colors ) );
        }
    }
}


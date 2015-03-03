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

#include <map>
#include <utility>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <deque>

#include <di/core/data/TriangleDataSet.h>
#include <di/core/data/LineDataSet.h>
#include <di/core/data/PointDataSet.h>
#include <di/core/data/Points.h>
#include <di/core/data/Lines.h>

#include "ExtractRegions.h"

#include <di/core/Logger.h>
#define LogTag "algorithms/ExtractRegions"

namespace di
{
    namespace algorithms
    {
        ExtractRegions::ExtractRegions():
            Algorithm( "Extract Regions",
                       "Extract regions on a given triangle dataset defined by different colors." )
        {
            // 1: the output
            m_borderLinesOutput = addOutput< di::core::LineDataSet >(
                    "Regions",
                    "Extracted regions as lines."
            );

            m_regionMeshOutput = addOutput< di::core::LineDataSet >(
                    "Region Meshes",
                    "Extracted region meshes as lines."
            );

            m_centerPointOutput = addOutput< di::core::PointDataSet >(
                    "Region Centers",
                    "Extracted center points of the regions."
            );

            m_regionOutput = addOutput< RegionDataSet >(
                    "Region Information",
                    "Collection of useful information about the regions."
            );

            m_connectionsOutput = addOutput< di::core::LineDataSet >(
                    "Connections",
                    "Extracted connections between regions."
            );
            m_neighbourArrowOutput = addOutput<  RenderIllustrativeLines::LineDataSetWithNormals >(
                    "Neighbour Arrows",
                    "Extracted connections between neighbours as simple arrows."
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

        void marchRegion( size_t vertexID,
                          std::vector< size_t >& connectedAndEqual,
                          std::vector< bool >& visited,
                          ConstSPtr< core::TriangleMesh >& source,
                          ConstSPtr< RGBAArray >& attribute
                        )
        {
            auto directNeighbours = source->getNeighbourVertices( vertexID );
            auto thisAttribute = attribute->at( vertexID );
            for( auto n : directNeighbours )
            {
                if( !visited[ n ] && ( attribute->at( n ) == thisAttribute ) )
                {
                    connectedAndEqual.push_back( n );
                    visited[ n ] = true;
                    marchRegion( n, connectedAndEqual, visited, source, attribute );
                }
            }
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
            // Create the buffers we need to store lines and color information
            auto linesMesh = std::make_shared< di::core::Lines >();
            auto colorsMesh = std::make_shared< di::RGBAArray >();

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Extract Region Information
            //  - Find regions and their neighbourhood
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Create a list of regions, visit each vertex only once.
            std::vector< bool > visited( triangles->getNumVertices(), false );
            std::vector< std::vector< size_t > > regionVertices; // collect all vertices of the regions found
            std::vector< int > vertexRegion( triangles->getNumVertices(), -1 ); // associate the region to each vertex.
                                                                                // This is the dual list to regionVertices

            auto regionColors = std::make_shared< std::vector< glm::vec4 > >();       // create a palette of colors
            // Iterate all triangles and transform to lines
            size_t regionVertexCount = 0; // keep track of how many vertices where associated
            for( size_t vertID = 0; vertID < triangles->getNumVertices(); ++vertID )
            {
                // already visited?
                if( !visited[ vertID ] )
                {
                    // no. definitely a new region.

                    // -> find all direct and indirect neighbours:
                    std::vector< size_t > connectedAndEqual;
                    marchRegion( vertID, connectedAndEqual, visited, triangles, attribute );

                    // add new region to map and store the associated vertices.
                    regionVertices.push_back( connectedAndEqual );
                    regionColors->push_back( attribute->at( vertID ) ); // take source color as palette here
                    regionVertexCount += connectedAndEqual.size();

                    // also build the inverse list
                    for( auto v : connectedAndEqual )
                    {
                        vertexRegion[ v ] = regionVertices.size() - 1;
                    }
                }
            }

            LogD << "Associated " << regionVertexCount << " vertices of " << triangles->getNumVertices() << " with "  <<
                    regionVertices.size() << " non-connected regions." << LogEnd;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Extract Region Information 2
            //  - Find regions centers and their "front".
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


            // Use them to calculate the region centers
            auto regionCenterPoints = std::make_shared< di::core::Points >();
            auto regionFacingTo = std::make_shared< std::vector< glm::vec3 > >();
            auto triBB = triangles->getBoundingBox();
            for( auto reg : regionVertices )
            {
                // for each region, iterate vertices
                glm::vec3 center( 0.0f, 0.0f, 0.0f );
                for( auto v : reg )
                {
                     center += triangles->getVertex( v );
                }
                center /= static_cast< float >( reg.size() );
                regionCenterPoints->addVertex( center );

                // now, also calculate the directions pointing to "front" of the region
                auto facingTo = glm::vec3( 0.0 );
                for( auto v : reg )
                {
                    // Version 1: from BB center to vertex?
                    // auto bbCenter = triBB.getCenter();
                    // facingTo += glm::normalize(  triangles->getVertex( v ) - glm::vec3( bbCenter ) );

                    // Version 2: from each vertex to center
                    //facingTo -= glm::normalize( center - triangles->getVertex( v ) );

                    // Version 3: normals of the mesh
                    facingTo += triangles->getNormal( v );
                }

                // We need to let the normal point towards the bounding box
                facingTo /= static_cast< float >( reg.size() );
                facingTo = glm::normalize( facingTo );

                regionFacingTo->push_back( facingTo );
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build borders between regions
            //  - Find region borders and build index of lines splitting them
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Use the following iteration for determining the neighbourhood of each region
            auto regionNeighbours = std::make_shared< RegionNeighbourhood >( regionVertices.size() ); // keep a set of direct neighbours of each reg

            // Keep track of the regions that each line splits - the pair are the indices of the regions splitted. The vector is the list of lines
            // splitting those regions.
            std::map< std::pair< size_t, size_t >, std::vector< size_t > > regionSplitLines;

            std::map< size_t, glm::vec3 > lineNormals;
            std::map< size_t, glm::vec3 > lineBinormals;

            // Iterate all triangles and transform to lines
            for( auto t : triangles->getTriangles() )
            {
                // t is an ivec3 -> 3 indices to color, vertex, normal
                auto c1 = attribute->operator[]( t.x );
                auto c2 = attribute->operator[]( t.y );
                auto c3 = attribute->operator[]( t.z );

                auto v1 = triangles->getVertex( t.x );
                auto v2 = triangles->getVertex( t.y );
                auto v3 = triangles->getVertex( t.z );

                // Always add the triangle itself
                // Get Vertices, add to line data, connect ... nothing fancy here.
                auto v1I = linesMesh->addVertex( v1 );
                auto v2I = linesMesh->addVertex( v2 );
                auto v3I = linesMesh->addVertex( v3 );
                linesMesh->addLine( v1I, v2I );
                linesMesh->addLine( v1I, v3I );
                linesMesh->addLine( v2I, v3I );
                float a = 0.25;
                colorsMesh->push_back( glm::vec4( c1.r, c1.g, c1.b, a ) );
                colorsMesh->push_back( glm::vec4( c2.r, c2.g, c2.b, a ) );
                colorsMesh->push_back( glm::vec4( c3.r, c3.g, c3.b, a ) );

                // There are two cases now:
                //  1: c1,c2,c3 are the same -> triangle is not a border triangle -> do not add a border line
                if( ( c1 == c2 ) && ( c2 == c3 ) )
                {
                    continue;
                }
                else
                {
                    // get associated region
                    auto regID1 = -1;
                    auto regID2 = -1;
                    auto regID3 = -1;
                    for( size_t regID = 0; regID < regionVertices.size(); ++regID )
                    {
                        // are the current vertices in this region?
                        if( std::find( regionVertices[ regID ].begin(), regionVertices[ regID ].end(), t.x ) != regionVertices[ regID ].end() )
                        {
                            regID1 = regID;
                        }
                        if( std::find( regionVertices[ regID ].begin(), regionVertices[ regID ].end(), t.y ) != regionVertices[ regID ].end() )
                        {
                            regID2 = regID;
                        }
                        if( std::find( regionVertices[ regID ].begin(), regionVertices[ regID ].end(), t.z ) != regionVertices[ regID ].end() )
                        {
                            regID3 = regID;
                        }
                    }
                    if( regID1 < 0 )
                    {
                        LogE << "Could not find associated region?" << LogEnd;
                    }
                    if( regID2 < 0 )
                    {
                        LogE << "Could not find associated region?" << LogEnd;
                    }
                    if( regID3 < 0 )
                    {
                        LogE << "Could not find associated region?" << LogEnd;
                    }

                    // Avoid reverse connections
                    if( regID1 < regID2 )
                    {
                        ( *regionNeighbours )[ regID1 ].insert( regID2 );
                    }
                    if( regID1 < regID3 )
                    {
                        ( *regionNeighbours )[ regID1 ].insert( regID3 );
                    }
                    if( regID2 < regID1 )
                    {
                        ( *regionNeighbours )[ regID2 ].insert( regID2 );
                    }
                    if( regID2 < regID3 )
                    {
                        ( *regionNeighbours )[ regID2 ].insert( regID3 );
                    }
                    if( regID3 < regID1 )
                    {
                        ( *regionNeighbours )[ regID3 ].insert( regID1 );
                    }
                    if( regID3 < regID2 )
                    {
                        ( *regionNeighbours )[ regID3 ].insert( regID2 );
                    }

                    // 2: the more interesting case: each color is different to each other.
                    if( ( c1 != c2 ) && ( c1 != c3 ) && ( c2 != c3 ) )
                    {
                        // all vertices have a different attribute value
                        // Here, it is not sufficient to connect the middle of each edge (as below), we need to add a center vertex to split the
                        // triangle into three regions.

                        // -> Calc center vertex of triangle
                        auto c = ( v1 + v2 + v3 ) / 3.0f;

                        // build border lines from each edge-middle-point to the center
                        auto em1 = v1 + ( 0.5f * ( v2 - v1 ) );
                        auto em2 = v1 + ( 0.5f * ( v3 - v1 ) );
                        auto em3 = v2 + ( 0.5f * ( v3 - v2 ) );

                        // Add these vertices
                        auto em1I = lines->addVertex( em1, true );
                        auto em2I = lines->addVertex( em2, true );
                        auto em3I = lines->addVertex( em3, true );
                        auto cI = lines->addVertex( c ); // definitely unique.

                        // Do not forget to add enough colors
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );

                        // and connect edge middle points to center with lines:
                        lines->addLine( em1I, cI );
                        lines->addLine( em2I, cI );
                        lines->addLine( em3I, cI );
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

                        auto nd = triangles->getNormal( differentI );
                        auto ne1 = triangles->getNormal( otherI1 );
                        auto ne2 = triangles->getNormal( otherI2 );

                        // find the new vertices of the border. It just is the middle of the edge
                        auto bv1 = vd + ( 0.5f * ( ve1 - vd ) );
                        auto bv2 = vd + ( 0.5f * ( ve2 - vd ) );

                        // use this as border line segment -> add to lines
                        auto li1 = lines->addVertex( bv1, true );
                        auto li2 = lines->addVertex( bv2, true );
                        auto lineIdx = lines->addLine( std::min( li1, li2 ), std::max( li1, li2 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 0.5 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 0.5 ) );

                        // Build the pair of regions, use min/max to ensure commutativity
                        regionSplitLines[ std::make_pair( std::min( vertexRegion[ differentI ], vertexRegion[ otherI1 ] ),
                                                          std::max( vertexRegion[ differentI ], vertexRegion[ otherI1 ] )
                                                        )
                                        ].push_back( lineIdx ); // push the line index
                        lineNormals[ lineIdx ] = glm::normalize( nd + ne1 + ne2 );
                        lineBinormals[ lineIdx ] = glm::cross( lineNormals[ lineIdx ], glm::normalize( bv1 - bv2 ) );

                    }
                }
            }

            LogD << "Border Line Info: " << lines->getNumVertices() << " vertices and " << lines->getNumLines() << " lines." << LogEnd;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build a neighbourhood "arrow"
            //  - Find a suitable point for placing them and build some line data
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            auto neighbourLines = std::make_shared< di::core::Lines >();
            auto neighbourColors = std::make_shared< di::RGBAArray >();
            auto neighbourNormals = std::make_shared< di::NormalArray >();

            // regionSplitLines contains the info we need here: map two regions to the line indices splitting them. We now build a strip of lines for
            // the boders:
            for( auto regionPairToLineIndieces : regionSplitLines )
            {
                auto regionPair = regionPairToLineIndieces.first;
                auto lineIndices = regionPairToLineIndieces.second;

                // NOTE: the lines use shared vertices.

                // build strip:
                std::deque< size_t > sortedLineVertices; // the line strip
                sortedLineVertices.push_back( lines->getLine( lineIndices[ 0 ] ).x );
                sortedLineVertices.push_back( lines->getLine( lineIndices[ 0 ] ).y );
                std::deque< size_t > sortedLines;
                sortedLines.push_back( lineIndices[ 0 ] );

                // Brute-force find the connected line strip. Quadratic runtime! Needs to be improved!
                while( sortedLines.size() != lineIndices.size() )
                {
                    // front ids
                    int frontId = sortedLineVertices.front();
                    int backId = sortedLineVertices.back();
                    for( size_t i = 0; i < lineIndices.size(); ++i )
                    {
                        // is the next line before/after the current strip?
                        glm::ivec2 ids = lines->getLine( lineIndices[ i ] );

                        if( ids.x == frontId )
                        {
                            sortedLineVertices.push_front( ids.y );
                            sortedLines.push_front( lineIndices[ i ] );
                            break;
                        }
                        if( ids.y == frontId )
                        {
                            sortedLineVertices.push_front( ids.x );
                            sortedLines.push_front( lineIndices[ i ] );
                            break;
                        }

                        if( ids.x == backId )
                        {
                            sortedLineVertices.push_back( ids.y );
                            sortedLines.push_back( lineIndices[ i ] );
                            break;
                        }
                        if( ids.y == backId )
                        {
                            sortedLineVertices.push_back( ids.x );
                            sortedLines.push_back( lineIndices[ i ] );
                            break;
                        }
                    }
                }

                // We now have the strip.
                size_t centerIdx = sortedLines.size() / 2;

                auto normal = lineNormals[ sortedLines[ centerIdx ] ];
                auto binormal = lineBinormals[ sortedLines[ centerIdx ] ];
                auto point1 = lines->getVertex( sortedLineVertices[ centerIdx ] );
                auto point2 = lines->getVertex( sortedLineVertices[ centerIdx + 1 ] );
                auto centerPoint = point1 + 0.5f * ( point2 - point1 );

                float scale = 1.0f;
                auto arrV1Idx = neighbourLines->addVertex( centerPoint + scale * binormal );
                auto arrV2Idx = neighbourLines->addVertex( centerPoint - scale * binormal );
                neighbourLines->addLine( arrV1Idx, arrV2Idx );
                neighbourColors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                neighbourColors->push_back( glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
                neighbourNormals->push_back( normal );
                neighbourNormals->push_back( normal );
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build a connectivity graph
            //  - The dual graph to the region neighbourhood
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Create the buffers we need to store lines and color information of the connection graph
            auto connectionLines = std::make_shared< di::core::Lines >();
            auto connectionColors = std::make_shared< di::RGBAArray >();

            // We need to connect each regions. This is an experiment. We need to find out more about the "how".
            for( size_t regID = 0; regID < regionNeighbours->size(); ++regID )
            {
                // get the current region center point and its coordinates:
                auto c = regionCenterPoints->getVertex( regID );
                auto idx = connectionLines->addVertex( c );

                // Also use the color of the source region
                auto cCol = regionColors->at( regID );
                connectionColors->push_back( cCol );

                // Connect this with each neighbour
                for( auto n : regionNeighbours->operator[]( regID ) )
                {
                    auto nC = regionCenterPoints->getVertex( n );
                    auto idxN = connectionLines->addVertex( nC );
                    connectionLines->addLine( idx, idxN );

                    // Also use the color of the target region
                    auto nCol = regionColors->at( n );
                    connectionColors->push_back( nCol );
                }
            }

            auto regionConnections = regionNeighbours; // right now, we assume every region to be connected with each neighbour.

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Update outputs
            //  - Most of them are for debugging
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


            // Re-package the region list into a point dataset
            m_centerPointOutput->setData( std::make_shared< di::core::PointDataSet >( "Region Centers", regionCenterPoints,
                                                                                                        regionColors ) );

            // Collect everything into a dataset
            m_regionOutput->setData( std::make_shared< RegionDataSet >( "Region Information", regionCenterPoints,
                                                                                              regionFacingTo,
                                                                                              regionColors,
                                                                                              regionNeighbours,
                                                                                              regionConnections ) );

            // Output connections
            m_connectionsOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Connections", connectionLines,
                                                                                                           connectionColors ) );

            m_neighbourArrowOutput->setData(
                    std::make_shared< RenderIllustrativeLines::LineDataSetWithNormals >( "Region Neighbour Arrows", neighbourLines,
                                                                                                                    neighbourColors,
                                                                                                                    neighbourNormals ) );

            // Create line dataset and set output
            m_borderLinesOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Borders", lines, colors ) );
            m_regionMeshOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Mesh", linesMesh, colorsMesh ) );
        }
    }
}


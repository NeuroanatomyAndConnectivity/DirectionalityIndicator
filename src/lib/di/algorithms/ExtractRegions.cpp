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
#include <utility>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <queue>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <iostream>

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

            m_vectorOutput = addOutput< di::core::TriangleVectorField >(
                    "Directionality",
                    "Extracted continuous directions on the mesh."
            );

            // 2: the input
            m_dataInput = addInput< di::core::TriangleDataSet >(
                    "Triangle Mesh",
                    "The triangle data to process."
            );

            m_dataLabelInput = addInput< di::io::RegionLabelReader::DataSetType >(
                    "Triangle Labels",
                    "Labels to assign a region to each mesh vertex."
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

        void propagateDirectionBroad( size_t startRegion, // always a source
                                      SPtr< ExtractRegions::DirectedRegionNeighbourhood > directionGraph,
                                      SPtr< ExtractRegions::RegionConnections > regionNeighbours,
                                      SPtr< std::vector< size_t > > regionLabels )
        {
            std::queue< size_t > visitQueue;
            visitQueue.push( startRegion );
            while( !visitQueue.empty() )
            {
                auto currentRegion = visitQueue.front();
                visitQueue.pop();

                // Get neighbours and visit them
                auto neighbours = regionNeighbours->operator[]( currentRegion );
                for( auto neighbour : neighbours )
                {
                    // direction is defined by region labels:
                    auto myLabel = regionLabels->at( currentRegion );
                    auto neighbourLabel = regionLabels->at( currentRegion );

                    // is this connection already known? ( a connection FROM neighbour to this region
                    if( ( directionGraph->operator[]( neighbour ).count( currentRegion ) ) ||
                        ( directionGraph->operator[]( currentRegion ).count( neighbour ) ) )
                    {
                        // Already a known connection. Skip and do not propagate:
                        continue;
                    }

                    // Unknown connection. Add to graph and continue propagation
                    if( myLabel < neighbourLabel )
                    {
                        directionGraph->operator[]( currentRegion ).insert( neighbour );
                    }
                    else
                    {
                        directionGraph->operator[]( neighbour ).insert( currentRegion );
                    }

                    // Add to visit queue
                    visitQueue.push( neighbour );
                }
            }
        }

        void mergeVectors( std::map< size_t, glm::vec3 >& vectors, size_t where, const glm::vec3& vec )
        {
            if( vectors.count( where ) )
            {
                auto n = vectors[ where ];
                if( glm::dot( n, vec ) < 0.0 )
                {
                    vectors[ where ] += -vec;
                }
                else
                {
                    vectors[ where ] += vec;
                }
            }
            else
            {
                vectors[ where ] = vec;
            }
        }

        void ExtractRegions::process()
        {
            // Get input data
            auto triangleDataSet = m_dataInput->getData();
            auto triangleLabelDataSet = m_dataLabelInput->getData();
            if( !triangleDataSet || !triangleLabelDataSet )
            {
                return;
            }

            auto triangles = triangleDataSet->getGrid();
            auto attribute = triangleDataSet->getAttributes< 0 >(); // the color in our case
            auto labels = triangleLabelDataSet->getAttributes< 0 >();

            if( labels->size() != triangles->getNumVertices() )
            {
                LogE << "Number of labels needs to match the number of vertices in the triangle mesh." << LogEnd;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Extract Region Information
            //  - Find regions and their neighbourhood
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // Create a list of regions, visit each vertex only once.
            std::vector< bool > visited( triangles->getNumVertices(), false );

            // DATA: A list of regions, collecting each vertex belonging to it:
            std::vector< std::vector< size_t > > regionVertices;
            // DATA: Associate each vertex with its region
            std::vector< int > vertexRegion( triangles->getNumVertices(), -1 );
            // DATA: Color palette of the regions
            auto regionColors = std::make_shared< std::vector< glm::vec4 > >();
            // DATA: Mapping of internal regions to labels
            auto regionLabels = std::make_shared< std::vector< size_t > >();

            // Iterate all triangles and transform to lines
            size_t regionVertexCount = 0; // keep track of how many vertices where associated
            for( size_t vertID = 0; vertID < triangles->getNumVertices(); ++vertID )
            {
                // already visited?
                if( !visited[ vertID ] )
                {
                    // no. Definitely a new region.

                    // -> find all direct and indirect neighbours:
                    std::vector< size_t > connectedAndEqual;
                    marchRegion( vertID, connectedAndEqual, visited, triangles, attribute );

                    // add new region to map and store the associated vertices.
                    regionVertices.push_back( connectedAndEqual );
                    regionColors->push_back( attribute->at( vertID ) ); // take source color as palette here
                    regionLabels->push_back( labels->at( vertID ) );
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

            // Some output for verification.
            size_t internalID = 0;
            for( auto r : regionVertices )
            {
                size_t rmin = r.front();
                size_t rmax = r.front();

                for( auto v : r )
                {
                    rmin = std::min( rmin, v );
                    rmax = std::max( rmax, v );
                }

                LogD << "Region " << internalID << " Vertex ID range: [ " << rmin << ", " << rmax << " ]" << " Label: " << regionLabels->at(
                        internalID ) << "." << LogEnd;
                internalID++;
            }


            // DATA: the number of regions.
            auto numRegions = regionVertices.size();

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build a line mesh of the data. Useful for algorithm debugging.
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // DATA: the triangle mesh itself -> wireframe
            auto linesMesh = std::make_shared< di::core::Lines >();
            // DATA: the wireframe colors
            auto colorsMesh = std::make_shared< di::RGBAArray >();

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
                float a = 1.0;
                colorsMesh->push_back( glm::vec4( c1.r, c1.g, c1.b, a ) );
                colorsMesh->push_back( glm::vec4( c2.r, c2.g, c2.b, a ) );
                colorsMesh->push_back( glm::vec4( c3.r, c3.g, c3.b, a ) );
            }

            // Create a wireframe mesh for debug
            m_regionMeshOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Mesh", linesMesh, colorsMesh ) );

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build an directed neighbourhood between regions at the border vertices
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // DATA: Used to store the direction at each vertex
            auto vectorAttribute = std::make_shared< di::Vec3Array >( triangles->getNumVertices() );

            // DATA: Store if value is set for vertex ID
            auto vectorAttributeSet = std::vector< bool >( triangles->getNumVertices(), false );

            // DATA: Count the number of attributes per region
            auto regionAttributeCount = std::vector< size_t >( numRegions, 0 );

            // Iterate all triangles, find the border triangles
            for( size_t vertexID = 0; vertexID < triangles->getNumVertices(); ++vertexID )
            {
                // Get vertex region and ignore unknown or "null" regions
                auto vertexRegionID = vertexRegion[ vertexID ];
                if( regionLabels->at( vertexRegionID ) == 0 )
                {
                    vectorAttribute->at( vertexID ) = glm::vec3( 0.0f );
                    vectorAttributeSet.at( vertexID ) = true;
                    continue;
                }

                // Get all triangles sharing this vertex
                auto neighbourVertices = triangles->getNeighbourVertices( vertexID );

                // Collect directions of all borders
                std::vector< glm::vec3 > vertexBorderVectors;

                // Check each neighbour-region (if any different)
                for( auto neighbourID : neighbourVertices )
                {
                    // Get neighbour vertex region and ignore unknown or "null" regions
                    auto neighbourRegionID = vertexRegion[ neighbourID ];
                    if( regionLabels->at( neighbourRegionID ) == 0 )
                    {
                        continue;
                    }

                    // If the regions are different, build a direction vector as the weighted sum of all directions to all affected border vertices.
                    if( neighbourRegionID != vertexRegionID ) // << there is a border between vertex and neighbour
                    {
                        // point from this vertex towards the neighbour.
                        auto vertexSource = triangles->getVertex( vertexID );
                        auto vertexDest  = triangles->getVertex( neighbourID );

                        // TODO: not yet really correct!?!?
                        // float invert = ( vertexRegionID < neighbourRegionID ) ? -1.0f : 1.0f;
                        float invert = ( vertexID < neighbourID ) ? -1.0f : 1.0f;

                        // Finally, a direction. Add and go on to the next neighbour
                        auto direction = invert * glm::normalize( vertexDest - vertexSource );
                        vertexBorderVectors.push_back( direction );
                    }
                }

                // Is this vertex somehow participating in a border? If so, build a mean-direction:
                if( vertexBorderVectors.size() )
                {
                    glm::vec3 meanDirection;
                    for( auto dir : vertexBorderVectors )
                    {
                        meanDirection += dir;
                    }
                    meanDirection /= static_cast< float >( vertexBorderVectors.size() );

                    // Store
                    vectorAttribute->at( vertexID ) = meanDirection;
                    vectorAttributeSet.at( vertexID ) = true;

                    // Later, we need to know how much attributes have been set for this region
                    regionAttributeCount[ vertexRegionID ]++;
                }
            }

            LogD << "Done marching borders." << LogEnd;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Spread the field to have a direction for all vertices
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            bool keepRunning = true;
            while( keepRunning )
            {
                auto nowSet = vectorAttributeSet;

                // Iterate all triangles, find the border triangles
                for( size_t vertexID = 0; vertexID < triangles->getNumVertices(); ++vertexID )
                {
                    // already set?
                    if( vectorAttributeSet.at( vertexID ) )
                    {
                        continue;
                    }

                    // Get neighbours
                    auto neighbours = triangles->getNeighbourVertices( vertexID );
                    size_t includedNeighbours = 0;
                    float longestDistance = 0.0f;
                    for( auto neighbourID : neighbours )
                    {
                        if( vectorAttributeSet.at( neighbourID ) )
                        {
                            // for scaling the interpolation
                            longestDistance = std::max( longestDistance,
                                                        glm::distance( triangles->getVertex( vertexID ), triangles->getVertex( neighbourID ) )
                            );
                        }
                    }

                    glm::vec3 meanVec = glm::vec3( 0.0f );
                    float factor = 0.0;
                    for( auto neighbourID : neighbours )
                    {
                        if( vertexID == neighbourID )
                        {
                            continue;
                        }

                        if( vectorAttributeSet.at( neighbourID ) )
                        {
                            includedNeighbours++;
                            auto dist = glm::distance( triangles->getVertex( vertexID ), triangles->getVertex( neighbourID ) );

                            auto srcVec = vectorAttribute->at( neighbourID );
                            auto normal = glm::normalize( triangles->getNormal( vertexID ) );
                            auto biNormal = glm::normalize( glm::cross( normal, glm::normalize( srcVec ) ) );
                            auto vec = glm::normalize( glm::cross( biNormal, normal ) );

                            // ensure length again
                            vec *= glm::length( srcVec );
                            factor += ( dist / longestDistance );
                            meanVec += ( dist / longestDistance ) * vec;
                        }
                    }

                    if( includedNeighbours >= 2 )
                    {
                        vectorAttribute->at( vertexID ) = meanVec / factor;
                        nowSet.at( vertexID ) = true;
                    }
                }

                vectorAttributeSet = nowSet;
                keepRunning = false;
                for( auto vSet : vectorAttributeSet )
                {
                    if( !vSet )
                    {
                        keepRunning = true;
                        break;
                    }
                }
            }

            LogD << "Done propagating directions." << LogEnd;

            LogD << "Done. Updating output." << LogEnd;
            m_vectorOutput->setData( std::make_shared< di::core::TriangleVectorField >( "Directionality", triangles, vectorAttribute ) );


            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build an undirected neighbourhood between regions
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /*
            // DATA: Use the following iteration for determining the neighbourhood of each region.
            // NOTE: the vector stores a std::set of each neighbour, no duplicates
            auto regionNeighbours = std::make_shared< RegionNeighbourhood >( numRegions ); // keep a set of direct neighbours of each reg
            // DATA: the border vertices for each region
            auto borderVertices = std::map< std::pair< size_t, size_t >, std::set< size_t > >();
            // DATA: store the direction at each border vertex
            auto borderDirection = std::map< size_t, glm::vec3 >();

            // Iterate all triangles and transform to lines
            for( auto t : triangles->getTriangles() )
            {
                // get associated region
                auto regID1 = vertexRegion[ t.x ];
                auto regID2 = vertexRegion[ t.y ];
                auto regID3 = vertexRegion[ t.z ];

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

                // only border triangles are interesting here
                if( ( regID1 == regID2 ) && ( regID2 == regID3 ) )
                {
                    continue;
                }

                // store. Keep in mind that the element at each region is a set -> no duplicates automatically.
                if( regID1 != regID2 )
                {
                    ( *regionNeighbours )[ regID1 ].insert( regID2 );
                    ( *regionNeighbours )[ regID2 ].insert( regID1 );

                    // NOTE: first of pair is the smaller region ID to ensure commutativity
                    borderVertices[ std::make_pair( std::min( regID1, regID2 ),
                                                    std::max( regID1, regID2 ) ) ].insert( t.x );
                    borderVertices[ std::make_pair( std::min( regID1, regID2 ),
                                                    std::max( regID1, regID2 ) ) ].insert( t.y );

                }
                if( regID1 != regID3 )
                {
                    ( *regionNeighbours )[ regID1 ].insert( regID3 );
                    ( *regionNeighbours )[ regID3 ].insert( regID1 );

                    // NOTE: first of pair is the smaller region ID to ensure commutativity
                    borderVertices[ std::make_pair( std::min( regID1, regID3 ),
                                                    std::max( regID1, regID3 ) ) ].insert( t.x );
                    borderVertices[ std::make_pair( std::min( regID1, regID3 ),
                                                    std::max( regID1, regID3 ) ) ].insert( t.z );

                }
                if( regID2 != regID3 )
                {
                    ( *regionNeighbours )[ regID2 ].insert( regID3 );
                    ( *regionNeighbours )[ regID3 ].insert( regID2 );
                    // NOTE: first of pair is the smaller region ID to ensure commutativity
                    borderVertices[ std::make_pair( std::min( regID2, regID3 ),
                                                    std::max( regID2, regID3 ) ) ].insert( t.y );
                    borderVertices[ std::make_pair( std::min( regID2, regID3 ),
                                                    std::max( regID2, regID3 ) ) ].insert( t.z );
                }

                // Calc a somewhat useful direction between the regions
                // -> two cases:
                // 1: each vertex belongs to a different region
                if( ( regID1 != regID2 ) && ( regID1 != regID3 ) && ( regID2 != regID3 ) ) // every thing is different
                {
                    // let each direction point towards the center point

                    // calc the center
                    auto v1 = triangles->getVertex( t.x );
                    auto v2 = triangles->getVertex( t.y );
                    auto v3 = triangles->getVertex( t.z );
                    auto c = ( v1 + v2 + v3 ) / 3.0f;

                    // direction is the vector from the vertex to the center
                    auto d1 = glm::normalize( c - v1 );
                    auto d2 = glm::normalize( c - v2 );
                    auto d3 = glm::normalize( c - v3 );

                    // and merge with existing vectors
                    mergeVectors( borderDirection, t.x, d1 );
                    mergeVectors( borderDirection, t.y, d2 );
                    mergeVectors( borderDirection, t.z, d3 );
                }
                else // 2: two vertices are in one region
                {
                    // search the index of the one different region.
                    size_t differentI = 0;
                    size_t otherI1 = 0;
                    size_t otherI2 = 0;
                    if( ( regID1 != regID2 ) && ( regID1 != regID3 ) )
                    {
                        differentI = t.x;
                        otherI1 = t.y;
                        otherI2 = t.z;
                    }
                    else if( ( regID2 != regID1 ) && ( regID2 != regID3 ) )
                    {
                        differentI = t.y;
                        otherI1 = t.x;
                        otherI2 = t.z;
                    }
                    else if( ( regID3 != regID1 ) && ( regID3 != regID2 ) )
                    {
                        differentI = t.z;
                        otherI1 = t.x;
                        otherI2 = t.y;
                    }

                    auto vd = triangles->getVertex( differentI );
                    auto ve1 = triangles->getVertex( otherI1 );
                    auto ve2 = triangles->getVertex( otherI2 );

                    auto edge = glm::normalize( ve2 - ve1 );
                    auto triNormal = glm::normalize( glm::cross( edge, glm::normalize( vd - ve1 ) ) );
                    auto direction = glm::normalize( glm::cross( triNormal, edge ) );

                    // assign this direction to the involved vertices
                    mergeVectors( borderDirection, otherI1, direction );
                    mergeVectors( borderDirection, otherI2, direction );
                    mergeVectors( borderDirection, differentI, direction );
                }
            }
            */
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build a directed neighbourhood graph
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /*
            // Get the region with the most neighbours
            auto maxIter = std::max_element( regionNeighbours->begin(), regionNeighbours->end(),
                []( RegionNeighbourhood::value_type a, RegionNeighbourhood::value_type b )
                {
                    return a.size() < b.size();
                }
            );
            // the first region
            size_t initialRegion = maxIter - regionNeighbours->begin();

            // Debug output
            LogD << "Region with the most neighbours used as initial source region: " << initialRegion <<
                    ". Has " << maxIter->size() << " neighbours. Color is " <<
                    static_cast< unsigned short >( 255 * ( *regionColors )[ initialRegion ].r ) << ", " <<
                    static_cast< unsigned short >( 255 * ( *regionColors )[ initialRegion ].g ) << ", " <<
                    static_cast< unsigned short >( 255 * ( *regionColors )[ initialRegion ].b ) << "." << LogEnd;


            // DATA: Classical edge-list of the directed graph. Direction: from entry.first to entry.second
            LogD << "Propagating directions through " << numRegions << " regions. This might take a while." << LogEnd;
            auto directionGraph = std::make_shared< DirectedRegionNeighbourhood >();
            propagateDirectionBroad( initialRegion, directionGraph, regionNeighbours, regionLabels );
            */
            /*
            // Debug to DOT file:
            std::ofstream ofs( "/home/seth/dotfilebroad.dot", std::ofstream::out );

            ofs << "digraph connections" << std::endl << "{" << std::endl;
            for( size_t regID = 0; regID < regionColors->size(); ++regID )
            {
                ofs << "\"" << regID << "\"" << " [shape=circle, style=filled, fontcolor=\"white\", fontname=\"mono bold\", fillcolor = \"#" <<
                    std::hex << std::setw( 2 ) <<  std::setfill( '0' ) <<
                    static_cast< unsigned int >( 255 * ( *regionColors )[ regID ].r ) <<
                    std::hex << std::setw( 2 ) <<  std::setfill( '0' ) <<
                    static_cast< unsigned int >( 255 * ( *regionColors )[ regID ].g ) <<
                    std::hex << std::setw( 2 ) <<  std::setfill( '0' ) <<
                    static_cast< unsigned int >( 255 * ( *regionColors )[ regID ].b ) <<
                    std::dec <<
                    "\"]" << std::endl;
            }
            for( auto directions : *directionGraph )
            {
                for( auto direction : directions.second )
                {
                    ofs << "\"" << directions.first << "\" -> \"" << direction << "\"" << std::endl;
                }
            }
            ofs << "}" << std::endl;
            ofs.close();
            */

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build a continuous vector field
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /*
            // DATA: vector attribute for each vertex
            auto vectorAttribute = std::make_shared< di::Vec3Array >( triangles->getNumVertices() );
            float maxLength = 0.0;

            LogD << "Creating continuous vector field." << LogEnd;

            for( size_t vertexID = 0; vertexID < triangles->getNumVertices(); ++vertexID )
            {
                auto regionColor = attribute->at( vertexID );
                // Use region color as mask -> black is a "no-region"
                if( ( regionColor.r == 0.0f ) && ( regionColor.g == 0.0f ) && ( regionColor.b == 0.0f ) )
                {
                    continue;
                }

                // For the vertex, get the region:
                auto regID = vertexRegion[ vertexID ];
                if( regID < 0 )
                {
                    LogE << "Could not find region for vertex." << LogEnd;
                    continue;
                }

                auto currentVertex = triangles->getVertex( vertexID );
                float maxDistance = 0.0f;

                // As we want to weight by distance, we need a max distance
                for( auto neighbourRegion : regionNeighbours->operator[]( regID ) )
                {
                    auto connection = std::make_pair( std::min( static_cast< size_t >( regID ), neighbourRegion ),
                                                      std::max( static_cast< size_t >( regID ), neighbourRegion ) );
                                   // iterate all common border vertices of this neighbour
                    for( auto borderVertex : borderVertices[ connection ] )
                    {
                        auto vb = triangles->getVertex( borderVertex );
                        maxDistance = std::max( glm::length( vb - currentVertex ), maxDistance );
                    }
                }

                // Accumulate directions
                auto mergedDirection = glm::vec3( 0.0f );
                size_t numMerged = 0;

                // iterate all neighbour regions
                for( auto neighbourRegion : regionNeighbours->operator[]( regID ) )
                {
                    auto connection = std::make_pair( std::min( static_cast< size_t >( regID ), neighbourRegion ),
                                                      std::max( static_cast< size_t >( regID ), neighbourRegion ) );

                    // is this incoming or outgoing?
                    bool outgoing = ( directionGraph->operator[]( regID ).count( neighbourRegion ) != 0 );
                    bool incomoming = ( directionGraph->operator[]( neighbourRegion ).count( regID ) != 0 );
                    // Consistency test
                    if( !( outgoing || incomoming ) )
                    {
                        LogE << "No connection between regions: " << regID << "<->" << neighbourRegion << LogEnd;
                        continue;
                    }

                    // iterate all common border vertices of this neighbour
                    for( auto borderVertex : borderVertices[ connection ] )
                    {
                        // Consistency test
                        if( !borderDirection.count( borderVertex ) )
                        {
                            LogE << "No border direction found for vertex " << borderVertex << LogEnd;
                            continue;
                        }

                        // weight by distance -> calc distance
                        auto vb = triangles->getVertex( borderVertex );
                        auto distance = glm::length( vb - currentVertex );

                        // normalize direction
                        auto dot = glm::dot( glm::normalize( vb - currentVertex ), glm::normalize( borderDirection[ borderVertex ] ) );

                        float invert = 1.0;

                        // assume: dot < 0 -> the border vector points inwards -> for outgoing -> invert
                        if( ( dot < 0.0 ) &&  outgoing )
                        {
                            invert = -1.0;
                        }
                        if( ( dot > 0.0 ) && !outgoing )
                        {
                            invert = -1.0;
                        }

                        // merge
                        numMerged++;
                        mergedDirection += invert * ( 1.0f - ( distance / maxDistance ) ) * glm::normalize( borderDirection[ borderVertex ] );
                    }
                }

                // store the vector
                // But first, ensure the vector is parallel to the triangle
                auto onPlane = glm::normalize( glm::cross( glm::cross( triangles->getNormal( vertexID ),
                                                           mergedDirection ),
                                               triangles->getNormal( vertexID ) ) );
                vectorAttribute->operator[]( vertexID ) = onPlane * glm::length( mergedDirection / static_cast< float >( numMerged ) );
                maxLength = std::max( glm::length( mergedDirection ), maxLength );
            }

            // Normalize lengths against max-len?
            // for( size_t vID = 0; vID < vectorAttribute->size(); ++vID )
            // {
            //    vectorAttribute->operator[]( vID ) /= maxLength;
            // }

            LogD << "Done. Updating output." << LogEnd;
            m_vectorOutput->setData( std::make_shared< di::core::TriangleVectorField >( "Directionality", triangles, vectorAttribute ) );
            */

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // NOTE: the remaining code is not used but might be usable later -> keep it for now.
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build borders between regions
            //  - Find region borders and build index of lines splitting them
            //  - Also build a non-directed neighbourhood graph between regions
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /*
            // DATA: the border lines
            auto lines = std::make_shared< di::core::Lines >();
            // DATA: the colors of the lines
            auto colors = std::make_shared< di::RGBAArray >();
            */

            // DATA: the triangle mesh itself -> wireframe
            /*auto linesMesh = std::make_shared< di::core::Lines >();
            // DATA: the wireframe colors
            auto colorsMesh = std::make_shared< di::RGBAArray >();*/

            // Keep track of the regions that each line splits - the pair are the indices of the regions splitted. The vector is the list of lines
            // splitting those regions.
            /*std::map< std::pair< size_t, size_t >, std::vector< size_t > > regionSplitLines;
            std::map< size_t, glm::vec3 > lineNormals;
            std::map< size_t, glm::vec3 > lineBinormals;*/
            /*
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
                */
                /*auto v1I = linesMesh->addVertex( v1 );
                auto v2I = linesMesh->addVertex( v2 );
                auto v3I = linesMesh->addVertex( v3 );
                linesMesh->addLine( v1I, v2I );
                linesMesh->addLine( v1I, v3I );
                linesMesh->addLine( v2I, v3I );
                float a = 0.25;
                colorsMesh->push_back( glm::vec4( c1.r, c1.g, c1.b, a ) );
                colorsMesh->push_back( glm::vec4( c2.r, c2.g, c2.b, a ) );
                colorsMesh->push_back( glm::vec4( c3.r, c3.g, c3.b, a ) );*/
                /*
                // There are two cases now:
                //  1: c1,c2,c3 are the same -> triangle is not a border triangle -> do not add a border line
                if( ( c1 == c2 ) && ( c2 == c3 ) )
                {
                    continue;
                }
                else
                {
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

                        // find the new vertices of the border. It just is the middle of the edge
                        auto bv1 = vd + ( 0.5f * ( ve1 - vd ) );
                        auto bv2 = vd + ( 0.5f * ( ve2 - vd ) );

                        // use this as border line segment -> add to lines
                        auto li1 = lines->addVertex( bv1, true );
                        auto li2 = lines->addVertex( bv2, true );
                        //auto lineIdx =
                        lines->addLine( std::min( li1, li2 ), std::max( li1, li2 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 0.5 ) );
                        colors->push_back( glm::vec4( 1.0, 1.0, 1.0, 0.5 ) );
                        */
                        // Build the pair of regions, use min/max to ensure commutativity
                        /*auto nd = triangles->getNormal( differentI );
                        auto ne1 = triangles->getNormal( otherI1 );
                        auto ne2 = triangles->getNormal( otherI2 );

                        regionSplitLines[ std::make_pair( std::min( vertexRegion[ differentI ], vertexRegion[ otherI1 ] ),
                                                          std::max( vertexRegion[ differentI ], vertexRegion[ otherI1 ] )
                                                        )
                                        ].push_back( lineIdx ); // push the line index
                        lineNormals[ lineIdx ] = glm::normalize( nd + ne1 + ne2 );
                        lineBinormals[ lineIdx ] = glm::cross( lineNormals[ lineIdx ], glm::normalize( bv1 - bv2 ) );*/
                    /* }
                }
            }

            LogD << "Border Line Info: " << lines->getNumVertices() << " vertices and " << lines->getNumLines() << " lines." << LogEnd;
            */

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build a neighbourhood "arrow"
            //  - Find a suitable point for placing them and build some line data
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                /*
            auto neighbourLines = std::make_shared< di::core::Lines >();
            auto neighbourColors = std::make_shared< di::RGBAArray >();
            auto neighbourNormals = std::make_shared< di::NormalArray >();

            // regionSplitLines contains the info we need here: map two regions to the line indices splitting them. We now build a strip of lines for
            // the boders:
            for( auto regionPairToLineIndieces : regionSplitLines )
            {
                // auto regionPair = regionPairToLineIndieces.first;
                auto lineIndices = regionPairToLineIndieces.second;

                // NOTE: the lines use shared vertices.

                // build strip:
                std::deque< size_t > sortedLineVertices; // the line strip
                sortedLineVertices.push_back( lines->getLine( lineIndices[ 0 ] ).x );
                sortedLineVertices.push_back( lines->getLine( lineIndices[ 0 ] ).y );
                std::deque< size_t > sortedLines;
                sortedLines.push_back( lineIndices[ 0 ] );

                // Brute-force find the connected line strip. Quadratic runtime! Needs to be improved!
                //
                // NOTE: this is buggy. the resulting list contains always the same index ...
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
        */

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build a connectivity graph
            //  - The dual graph to the region neighbourhood
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /*
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
            */

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Update outputs
            //  - Most of them are for debugging
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /*
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
                                                                                                           */

            /*
            m_neighbourArrowOutput->setData(
                    std::make_shared< RenderIllustrativeLines::LineDataSetWithNormals >( "Region Neighbour Arrows", neighbourLines,
                                                                                                                    neighbourColors,
                                                                                                                    neighbourNormals ) );
                                                                                                                    */

            /*
            // Create line dataset and set output
            m_borderLinesOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Borders", lines, colors ) );
            m_regionMeshOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Mesh", linesMesh, colorsMesh ) );
            */
        }
    }
}


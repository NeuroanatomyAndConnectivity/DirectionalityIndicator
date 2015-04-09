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
                    "Region Mesh as Lines",
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

            m_dataLabelOrderingInput = addInput< di::io::RegionLabelReader::DataSetType >(
                    "Label Ordering",
                    "Label ordering to use for determining direction between regions."
            );


            m_enableDirectionSwitch = addParameter< bool >(
                    "Switch Directionality",
                    "If enabled, the directionality of the arrows will be inverted.",
                    true
            );
        }

        ExtractRegions::~ExtractRegions()
        {
            // nothing to clean up so far
        }

        template< typename CriterionFunctorType >
        void marchRegion( size_t vertexID,
                          std::vector< size_t >& connectedAndEqual,
                          std::vector< bool >& visited,
                          ConstSPtr< core::TriangleMesh >& source,
                          CriterionFunctorType criterion
                        )
        {
            auto directNeighbours = source->getNeighbourVertices( vertexID );
            for( auto n : directNeighbours )
            {
                if( !visited[ n ] && criterion( n, vertexID ) )
                {
                    connectedAndEqual.push_back( n );
                    visited[ n ] = true;
                    marchRegion( n, connectedAndEqual, visited, source, criterion );
                }
            }
        }

        void ExtractRegions::process()
        {
            // Get input data
            auto triangleDataSet = m_dataInput->getData();
            auto triangleLabelDataSet = m_dataLabelInput->getData();
            auto labelOrderDataSet = m_dataLabelOrderingInput->getData();
            if( !triangleDataSet || !triangleLabelDataSet || !labelOrderDataSet )
            {
                return;
            }

            auto triangles = triangleDataSet->getGrid();
            auto attribute = triangleDataSet->getAttributes< 0 >(); // the color in our case
            auto labels = triangleLabelDataSet->getAttributes< 0 >();
            auto labelOrders = labelOrderDataSet->getAttributes< 0 >();

            if( labels->size() != triangles->getNumVertices() )
            {
                LogE << "Number of labels needs to match the number of vertices in the triangle mesh." << LogEnd;
            }

            // Debug Code
            LogD << "Label ordering: ";
            for( auto l : *labelOrders )
            {
                LogContinue << l << ", ";
            }
            LogContinue << " - total: " << labelOrders->size() << LogEnd;


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
                    marchRegion( vertID, connectedAndEqual, visited, triangles,
                        [ & ]( size_t v1, size_t v2 )
                        {
                            return ( labels->at( v1 ) == labels->at( v2 ) );
                        }
                    );

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
            // auto numRegions = regionVertices.size();

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
            m_regionMeshOutput->setData( std::make_shared< di::core::LineDataSet >( "Region Mesh as Lines", linesMesh, colorsMesh ) );

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Build an directed neighbourhood between regions at the border vertices
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // DATA: Used to store the direction at each vertex
            auto vectorAttribute = std::make_shared< di::Vec3Array >( triangles->getNumVertices() );

            // DATA: vertices that are set to true here will be ignored during processing.
            auto vertexIgnore = std::vector< bool >( triangles->getNumVertices(), false );

            // DATA: Store if value is set for vertex ID
            auto vectorAttributeSet = std::vector< bool >( triangles->getNumVertices(), false );

            // Iterate all vertices, build ignore list
            for( size_t vertexID = 0; vertexID < triangles->getNumVertices(); ++vertexID )
            {
                // Get label of vertex
                auto label = labels->at( vertexID );

                // Ignoring a label when it is not in the label order list
                auto ignore = ( std::find( labelOrders->begin(), labelOrders->end(), label ) == labelOrders->end() );
                // NOT in list -> ignore
                if( ignore )
                {
                    vertexIgnore.at( vertexID ) = true;
                    vectorAttribute->at( vertexID ) = glm::vec3( 0.0f );
                    vectorAttributeSet.at( vertexID ) = true;
                }

            }
            LogD << "Masked all vertices that are ignored according to label order list." << LogEnd;

            // Iterate all vertices, decide for directionality if it is a border vertex
            for( size_t vertexID = 0; vertexID < triangles->getNumVertices(); ++vertexID )
            {
                if( vertexIgnore.at( vertexID ) )
                {
                    continue;
                }

                // Get vertex region and ignore unknown or "null" regions
                auto vertexRegionID = vertexRegion[ vertexID ];
                auto label = labels->at( vertexID );

                // Get all triangles sharing this vertex
                auto neighbourVertices = triangles->getNeighbourVertices( vertexID );

                // Collect directions of all borders
                std::vector< glm::vec3 > vertexBorderVectors;

                // Check each neighbour-region (if any different)
                for( auto neighbourID : neighbourVertices )
                {
                    // Get neighbour vertex region and ignore unknown or "null" regions
                    auto neighbourRegionID = vertexRegion[ neighbourID ];
                    if( vertexIgnore.at( neighbourID ) )
                    {
                        continue;
                    }

                    // If the regions are different, build a direction vector as the weighted sum of all directions to all affected border vertices.
                    if( neighbourRegionID != vertexRegionID ) // << there is a border between vertex and neighbour
                    {
                        // point from this vertex towards the neighbour.
                        auto vertexSource = triangles->getVertex( vertexID );
                        auto vertexNeighbour  = triangles->getVertex( neighbourID );

                        auto neighbourLabel = regionLabels->at( neighbourRegionID );
                        auto neighbourPos = std::find( labelOrders->begin(), labelOrders->end(), neighbourLabel );
                        auto vertexPos    = std::find( labelOrders->begin(), labelOrders->end(), label );

                        if( ( vertexPos == labelOrders->end() ) || ( neighbourPos == labelOrders->end() ) )
                        {
                            LogE << "ERROR: label not in orders list?" << LogEnd;
                        }

                        // Standard case: FROM the latter TO the earlier. The default direction is FROM vertexID TO neighbourID
                        float invert = ( vertexPos > neighbourPos ) ? -1.0f : 1.0f;

                        // But allow the user to change it again
                        invert *= m_enableDirectionSwitch->get() ? -1.0f : 1.0f;

                        // Finally, a direction. Add and go on to the next neighbour
                        auto direction = invert * glm::normalize( vertexNeighbour - vertexSource );
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
                }
            }

            LogD << "Done marching borders." << LogEnd;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Spread the field to have a direction for all vertices
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // This is an iterative process to spread the values in to each vertex by using its neighbours
            bool keepRunning = true;
            while( keepRunning )
            {
                auto nowSet = vectorAttributeSet;

                // Iterate all vertices
                for( size_t vertexID = 0; vertexID < triangles->getNumVertices(); ++vertexID )
                {
                    // already set?
                    // NOTE: this also includes 0-label vertices
                    if( vectorAttributeSet.at( vertexID ) )
                    {
                        continue;
                    }

                    // Get neighbours
                    auto neighbours = triangles->getNeighbourVertices( vertexID );

                    // We need to know how much neighbours already have a value and the longest distance between those neighbours
                    size_t includedNeighbours = 0;
                    float longestDistance = 0.0f;
                    for( auto neighbourID : neighbours )
                    {
                        // Well, the neighbour function includes the source vertex too. Skip the source vertex.
                        if( vertexID == neighbourID )
                        {
                            continue;
                        }

                        // If a neighbour is an ignored vertex, ignore it explicitly
                        if( vertexIgnore.at( neighbourID ) )
                        {
                            continue;
                        }

                        // Now, has this neighbour a value being set already?
                        if( vectorAttributeSet.at( neighbourID ) )
                        {
                            includedNeighbours++;

                            // for scaling the interpolation
                            longestDistance = std::max( longestDistance,
                                                        glm::distance( triangles->getVertex( vertexID ), triangles->getVertex( neighbourID ) )
                            );
                        }
                    }

                    // We want at least two vertices being set.
                    if( includedNeighbours < 2 )
                    {
                        continue;
                    }

                    // Repeat to go to each neighbour, this time merge the values using the distance we calculated earlier:
                    glm::vec3 meanVec = glm::vec3( 0.0f );
                    float factor = 0.0;
                    for( auto neighbourID : neighbours )
                    {
                        // Well, the neighbour function includes the source vertex too. Skip the source vertex.
                        if( vertexID == neighbourID )
                        {
                            continue;
                        }

                        // If a neighbour is a 0-label vertex, ignore it
                        if( vertexIgnore.at( neighbourID ) )
                        {
                            continue;
                        }

                        // If a value is defined ->
                        if( vectorAttributeSet.at( neighbourID ) )
                        {
                            // How far is it away?
                            auto dist = glm::distance( triangles->getVertex( vertexID ), triangles->getVertex( neighbourID ) );

                            // The value of the neighbour:
                            auto srcVec = vectorAttribute->at( neighbourID );
                            // To keep the vectors projected on the surface, we need the normal at our current vertex:
                            auto normal = glm::normalize( triangles->getNormal( vertexID ) );

                            // We need to project the vectors to the plane represented by this vertex's normal. To do this, the srcVec needs a minimal
                            // length and the angle to the normal should not be too small:
                            auto vec = glm::vec3( 0.0f );
                            auto cosAngle = std::abs( glm::dot( srcVec, normal ) );
                            if( ( glm::length( srcVec ) > 0.001 ) && ( cosAngle < 0.98 ) )
                            {
                                // The normal and the vector define a bi-normal (tangent)
                                auto biNormal = glm::normalize( glm::cross( normal, glm::normalize( srcVec ) ) );

                                // The binormal now allows a projection to the plane
                                vec = glm::normalize( glm::cross( biNormal, normal ) );

                                // Ensure the proper length
                                vec *= glm::length( srcVec );
                            }

                            // ensure length again
                            factor += ( dist / longestDistance );
                            meanVec += ( dist / longestDistance ) * vec;
                        }
                    }

                    vectorAttribute->at( vertexID ) = meanVec / factor;
                    nowSet.at( vertexID ) = true;
                }

                // As this is iterative and will converge -> go on until it converged
                /*vectorAttributeSet = nowSet;
                keepRunning = false;
                for( auto vSet : vectorAttributeSet )
                {
                    if( !vSet )
                    {
                        keepRunning = true;
                        break;
                    }
                }
                */

                // Iterate both sets and check for differences
                auto first1 = nowSet.begin();
                auto first2 = vectorAttributeSet.begin();
                auto last1 = nowSet.end();
                keepRunning = false;
                bool allEqual = true;
                for( ; first1 != last1; ++first1, ++first2 )
                {
                    if( !( *first1 == *first2 ) )
                    {
                        allEqual = false;
                    }

                    if( !*first1 )
                    {
                        keepRunning = true;
                    }
                }

                // Now there might be the case where all elements are equal but keepRunning is true -> break to avoid endless loops
                if( keepRunning && allEqual )
                {
                    LogW << "The data contains areas where propagation is stuck. Aborting those regions now." << LogEnd;
                    keepRunning = false;
                }

                vectorAttributeSet = nowSet;
            }

            LogD << "Done propagating directions." << LogEnd;

            // Update outputs
            LogD << "Done. Updating output." << LogEnd;
            m_vectorOutput->setData( std::make_shared< di::core::TriangleVectorField >( "Directionality", triangles, vectorAttribute ) );
        }
    }
}


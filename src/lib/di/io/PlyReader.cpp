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

#include <iostream>
#include <string>
#include <clocale>
#include <algorithm>
#include <functional>
#include <chrono>
#include <thread>

#define LogTag "io/PlyReader"
#include <di/core/Logger.h>
#include <di/core/Filesystem.h>
#include <di/core/StringUtils.h>
#include <di/core/data/TriangleMesh.h>
#include <di/core/data/TriangleDataSet.h>

#include <di/ext/rply/rply.h>

#include "PlyReader.h"

namespace di
{
    namespace io
    {
        PlyReader::PlyReader():
            Reader()
        {
        }

        PlyReader::~PlyReader()
        {
        }

        bool PlyReader::canLoad( const std::string& filename ) const
        {
            std::string ext = di::core::getFileExtension( filename );
            return ( di::core::toLower( ext ) == "ply" );
        }

        /**
         * Callback to handle rply vertices. Keep in mind that this is old-fashioned C code.
         *
         * \param argument the ply argument
         *
         * \return 1 of everything was fine.
         */
        int vertexCallback( p_ply_argument argument )
        {
            // Query the idata and pdata (user data allowed by rply)
            long index;
            void* meshPlain = nullptr;
            // do query
            ply_get_argument_user_data( argument, &meshPlain, &index );

            // the pointer was a mesh
            di::core::TriangleMesh* mesh = reinterpret_cast< di::core::TriangleMesh* >( meshPlain );

            // always use this vector. Static variable.
            static glm::vec3 vertex = { 0.0, 0.0, 0.0 };
            // Store value there
            vertex[ index ] = ply_get_argument_value( argument );

            // if this is the last index, add vertex
            if( index == 2 )
            {
                mesh->addVertex( vertex );
            }

            return 1;
        }

        /**
         * Callback to handle rply face indices. Keep in mind that this is old-fashioned C code.
         *
         * \param argument the ply argument
         *
         * \return 1 of everything was fine.
         */
        int faceCallback( p_ply_argument argument )
        {
            // get the indices of this face
            long length;
            long index;
            ply_get_argument_property( argument, NULL, &length, &index );
            if( ( index < 0 ) || ( index > 2 ) )
            {
                return 1;
            }

            // get the idata and pdata (user data allowed by rply)
            void* meshPlain = nullptr;
            // query
            ply_get_argument_user_data( argument, &meshPlain, NULL );
            // the pointer was a mesh
            di::core::TriangleMesh* mesh = reinterpret_cast< di::core::TriangleMesh* >( meshPlain );

            // always use this vector. Static variable.
            static glm::ivec3 face = { 0, 0, 0 };
            face[ index ] = ply_get_argument_value( argument );
            if( index == 2 )
            {
                mesh->addTriangle( face );
            }
            return 1;
        }

        /**
         * Callback to handle rply colors. Keep in mind that this is old-fashioned C code.
         *
         * \param argument the ply argument
         *
         * \return 1 of everything was fine.
         */
        int colorCallback( p_ply_argument argument )
        {
            // get the indices of this face
            long index;
            // get the idata and pdata (user data allowed by rply)
            void* colorsPlain = nullptr;
            // query
            ply_get_argument_user_data( argument, &colorsPlain, &index );
            // the pointer was a color array
            RGBAArray* colors = reinterpret_cast< RGBAArray* >( colorsPlain );

            // always use this vector. Static variable.
            static glm::vec4 color = { 1.0, 0.0, 0.0, 1.0 };
            color[ index ] = ( 1.0 / 255.0 ) * ply_get_argument_value( argument );  // we normalize here
            if( index == 2 )
            {
                // It is a std::vector
                colors->push_back( color );
            }
            return 1;
        }

        SPtr< di::core::DataSetBase > PlyReader::load( const std::string& filename ) const
        {
            // Use C style numeric locale to ensure that all loaders work properly.
            // Keep old locale
            const char* oldLocale = setlocale( LC_NUMERIC, NULL );
            setlocale( LC_NUMERIC, "C" );

            LogD << "Loading \"" << filename << "\"." << LogEnd;

            // store the mesh and a color array:
            SPtr< di::core::TriangleMesh > mesh( new di::core::TriangleMesh() );
            SPtr< RGBAArray > colors( new RGBAArray() );

            // to actually do the parsing, we use rply. It works with ASCII and binary PLY files.
            // ref: http://w3.impa.br/~diego/software/rply/
            long numVertices;
            long numColors;
            long numTriangles;

            // open the file
            p_ply ply = ply_open( filename.c_str(), NULL, 0, NULL );
            if( !ply )
            {
                LogE << "Failed to open PLY file " << filename << LogEnd;
                throw std::ios_base::failure( "Failed to open PLY file " + filename );
            }

            // read header
            if( !ply_read_header( ply ) )
            {
                LogE << "Failed to read header of PLY file " << filename << LogEnd;
                throw std::ios_base::failure( "Failed to read header of PLY file " + filename );
            }

            // load vertices using these callbacks:
            numVertices = ply_set_read_cb( ply, "vertex", "x", vertexCallback, mesh.get(), 0 );
                          ply_set_read_cb( ply, "vertex", "y", vertexCallback, mesh.get(), 1 );
                          ply_set_read_cb( ply, "vertex", "z", vertexCallback, mesh.get(), 2 );

            // also load colors
            numColors = ply_set_read_cb( ply, "vertex", "red", colorCallback, colors.get(), 0 );
                        ply_set_read_cb( ply, "vertex", "green", colorCallback, colors.get(), 1 );
                        ply_set_read_cb( ply, "vertex", "blue", colorCallback, colors.get(), 2 );

            // finally, read face indices
            numTriangles = ply_set_read_cb( ply, "face", "vertex_index", faceCallback, mesh.get(), 0 );

            LogD << "Going to load " << numTriangles << " triangles with " << numVertices << " vertices and " << numColors << " colors." << LogEnd;

            // go
            if( !ply_read( ply ) )
            {
                LogE << "Failed to read PLY file " << filename << LogEnd;
                throw std::ios_base::failure( "Failed to read PLY file " + filename );
            }

            // done. Close the file.
            ply_close( ply );

            // sanity check:
            if( !(
                    mesh->sanityCheck() &&
                  ( static_cast< size_t >( numTriangles ) == mesh->getNumTriangles() ) &&
                  ( static_cast< size_t >( numVertices ) == mesh->getNumVertices() )
                 )
              )
            {
                LogE << "Loaded mesh does not make sense. Sanity check failed for " + filename << LogEnd;
                throw std::ios_base::failure( "PLY file seems invalid. Sanity check failed for " + filename );
            }

            LogD << "Loading \"" << filename << "\" done." << LogEnd;

            // Restore locale.
            setlocale( LC_ALL, oldLocale );

            // Optimize mesh. As we did not load normals, create:
            mesh->calculateNormals();
            mesh->calculateInverseIndex();

            // construct the dataset
            return SPtr< di::core::TriangleDataSet >( new di::core::TriangleDataSet( filename, mesh, colors ) );
        }
    }
}


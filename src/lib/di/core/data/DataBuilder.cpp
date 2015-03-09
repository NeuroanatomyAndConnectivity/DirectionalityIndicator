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

#include "DataBuilder.h"

namespace di
{
    namespace core
    {
        SPtr< TriangleMesh > buildCuboid( const BoundingBox& bb )
        {
            auto cuboid = std::make_shared< TriangleMesh >();

            auto min = bb.getMin();
            auto max = bb.getMax();

            // Side 1:

            /*
            3-----2
            | \ / |
            |  X  |
            | / \ |
            0-----1
            */

            // Normals along positive axis
            glm::vec3 npx( 1.0, 0.0, 0.0 );
            glm::vec3 npy( 0.0, 1.0, 0.0 );
            glm::vec3 npz( 0.0, 0.0, 1.0 );
            // Normals along negative axis
            glm::vec3 nnx( -1.0,  0.0,  0.0 );
            glm::vec3 nny(  0.0, -1.0,  0.0 );
            glm::vec3 nnz(  0.0,  0.0, -1.0 );

            // Front
            auto vf0 = cuboid->addVertex( min.x, min.y, min.z );
            auto vf1 = cuboid->addVertex( max.x, min.y, min.z );
            auto vf2 = cuboid->addVertex( max.x, max.y, min.z );
            auto vf3 = cuboid->addVertex( min.x, max.y, min.z );
            // Each vert needs to have its own normal
            cuboid->addNormal( nnz );
            cuboid->addNormal( nnz );
            cuboid->addNormal( nnz );
            cuboid->addNormal( nnz );

            cuboid->addTriangle( vf0, vf1, vf2 );
            cuboid->addTriangle( vf2, vf3, vf0 );

            // Back
            auto vb0 = cuboid->addVertex( min.x, min.y, max.z );
            auto vb1 = cuboid->addVertex( max.x, min.y, max.z );
            auto vb2 = cuboid->addVertex( max.x, max.y, max.z );
            auto vb3 = cuboid->addVertex( min.x, max.y, max.z );
            // Each vert needs to have its own normal
            cuboid->addNormal( npz );
            cuboid->addNormal( npz );
            cuboid->addNormal( npz );
            cuboid->addNormal( npz );

            cuboid->addTriangle( vb0, vb1, vb2 );
            cuboid->addTriangle( vb2, vb3, vb0 );

            // Bottom
            auto vbot0 = cuboid->addVertex( min.x, min.y, min.z );
            auto vbot1 = cuboid->addVertex( max.x, min.y, min.z );
            auto vbot2 = cuboid->addVertex( max.x, min.y, max.z );
            auto vbot3 = cuboid->addVertex( min.x, min.y, max.z );
            // Each vert needs to have its own normal
            cuboid->addNormal( nny );
            cuboid->addNormal( nny );
            cuboid->addNormal( nny );
            cuboid->addNormal( nny );

            cuboid->addTriangle( vbot0, vbot1, vbot2 );
            cuboid->addTriangle( vbot2, vbot3, vbot0 );

            // Top
            auto vt0 = cuboid->addVertex( min.x, max.y, min.z );
            auto vt1 = cuboid->addVertex( max.x, max.y, min.z );
            auto vt2 = cuboid->addVertex( max.x, max.y, max.z );
            auto vt3 = cuboid->addVertex( min.x, max.y, max.z );
            // Each vert needs to have its own normal
            cuboid->addNormal( npy );
            cuboid->addNormal( npy );
            cuboid->addNormal( npy );
            cuboid->addNormal( npy );

            cuboid->addTriangle( vt0, vt1, vt2 );
            cuboid->addTriangle( vt2, vt3, vt0 );

            // Left
            auto vl0 = cuboid->addVertex( min.x, min.y, min.z );
            auto vl1 = cuboid->addVertex( min.x, max.y, min.z );
            auto vl2 = cuboid->addVertex( min.x, max.y, max.z );
            auto vl3 = cuboid->addVertex( min.x, min.y, max.z );
            // Each vert needs to have its own normal
            cuboid->addNormal( nnx );
            cuboid->addNormal( nnx );
            cuboid->addNormal( nnx );
            cuboid->addNormal( nnx );

            cuboid->addTriangle( vl0, vl1, vl2 );
            cuboid->addTriangle( vl2, vl3, vl0 );

            // Left
            auto vr0 = cuboid->addVertex( max.x, min.y, min.z );
            auto vr1 = cuboid->addVertex( max.x, max.y, min.z );
            auto vr2 = cuboid->addVertex( max.x, max.y, max.z );
            auto vr3 = cuboid->addVertex( max.x, min.y, max.z );
            // Each vert needs to have its own normal
            cuboid->addNormal( npx );
            cuboid->addNormal( npx );
            cuboid->addNormal( npx );
            cuboid->addNormal( npx );

            cuboid->addTriangle( vr0, vr1, vr2 );
            cuboid->addTriangle( vr2, vr3, vr0 );

            return cuboid;
        }
    }
}


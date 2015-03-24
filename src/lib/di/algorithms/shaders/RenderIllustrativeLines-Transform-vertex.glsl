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

#version 330

// Attribute data
in vec3 position;
in vec4 color;
in vec3 normal;
in vec3 vectors;

// Uniforms
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

// Varying out
flat out vec4 v_color;
out vec3 v_normal;
out vec4 v_posView;
out vec3 v_vector;
out float v_vectorLength;

void main()
{
    // v_color = vec4( abs(vectors.rgb), 1.0 );
    v_color = color;

    v_vector = ( u_ViewMatrix * vec4( vectors, 0.0 ) ).xyz;
    v_vectorLength = length( vectors );

    v_normal = ( u_ViewMatrix * vec4( normal, 0.0 ) ).xyz;
    v_posView = u_ViewMatrix * vec4( position, 1.0 );

    // Maybe switch normal. Point towards viewer
    vec3 toViewer = vec3( 0.0, 0.0, 1.0 );
    if( dot( toViewer, v_normal ) < 0.0 )
    {
        v_normal *= -1.0;
    }

    vec4 pos = u_ProjectionMatrix * v_posView;
    gl_Position = pos;
}


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

// Define input and output
layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

// Uniforms
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

uniform float u_width = 1.5;
uniform float u_height = 5.0;
uniform float u_dist = 2.0;

// Inputs
// NOTE: everything is in cam space
in vec4 v_pointColor[];
in vec4 v_pointPos[];
in vec4 v_pointVec[];
in vec4 v_pointNormal[];
// in vec4 v_position[]; // NOTE: implicit by gl_Position

// Outputs
out vec4 v_color;
out vec3 v_normal;
out vec2 v_surfaceUV;

void main()
{
    /////////////////////////////////////////////////////////////////////////////////////
    // Given:

    float width = u_width;
    float height = u_height;
    float dist = u_dist;

    float scale = clamp( 0.005 * 5.0 * smoothstep( 0.1, 0.9, 0.2 + v_pointVec[0].w ), 0.0025, 0.005 );
    float lscale = scale * height;
    float wscale = scale * width;

    vec3 p = v_pointPos[0].xyz;
    v_color = v_pointColor[0];
    v_normal = normalize( v_pointNormal[0].xyz );

    vec3 tangent = normalize( v_pointVec[0].xyz );
    vec3 binormal =  normalize( cross( tangent, v_normal ) );

    if( length( v_pointVec[0].xyz ) < 0.00001 )
    {
        return;
    }

    p += scale * v_normal * dist;

    vec3 lv1 = p - ( tangent * lscale )
                 - ( binormal * wscale );
    vec3 lv2 = p - ( tangent * lscale )
                 + ( binormal * wscale );
    vec3 lv3 = p + ( tangent * lscale )
                 - ( binormal * wscale );
    vec3 lv4 = p + ( tangent * lscale )
                 + ( binormal * wscale );

    // v1
    gl_Position = u_ProjectionMatrix * vec4( lv1, 1.0 );
    v_surfaceUV = vec2( -1.0, -1.0 );
    EmitVertex();

    // v2
    gl_Position = u_ProjectionMatrix * vec4( lv2, 1.0 );
    v_surfaceUV = vec2(  1.0, -1.0 );
    EmitVertex();

    // v3
    gl_Position = u_ProjectionMatrix * vec4( lv3, 1.0 );
    v_surfaceUV = vec2( -1.0, 1.0 );
    EmitVertex();

    // v4
    gl_Position = u_ProjectionMatrix * vec4( lv4, 1.0 );
    v_surfaceUV = vec2( 1.0, 1.0 );
    EmitVertex();

    EndPrimitive();
}


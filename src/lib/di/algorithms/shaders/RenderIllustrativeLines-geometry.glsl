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

    const float width = 2.0;
    const float length = 5.0;
    const float dist = 1.0;

    // Vector input
//    vec3 lv1 = v_pointPos[0].xyz + v_pointVec[0].xyz;
//    vec3 lv2 = v_pointPos[0].xyz - v_pointVec[0].xyz;

    float scale = 0.01;
    float z =  -0.5;
    vec3 lv1 = gl_in[0].gl_Position.xyz + vec3( -scale, -scale,  z );
    vec3 lv2 = gl_in[0].gl_Position.xyz + vec3( -scale,  scale, z );
    vec3 lv3 = gl_in[0].gl_Position.xyz + vec3(  scale, -scale, z );
    vec3 lv4 = gl_in[0].gl_Position.xyz + vec3(  scale,  scale, z );
/*
    vec3 lv1 = v_pointPos[0].xyz + scale * vec3( -1.0, -1.0, 0.0 );
    vec3 lv2 = v_pointPos[0].xyz + scale *  vec3( -1.0,  1.0, 0.0 );
    vec3 lv3 = v_pointPos[0].xyz + scale *  vec3(  1.0, -1.0, 0.0 );
    vec3 lv4 = v_pointPos[0].xyz + scale *  vec3(  1.0,  1.0, 0.0 );
*/


    // v1
    gl_Position = u_ProjectionMatrix * vec4( lv1, 1.0 );
    v_color = v_pointColor[0];
    v_normal = v_pointNormal[0].xyz;
    v_surfaceUV = vec2( -1.0, -1.0 );
    EmitVertex();

    // v2
    gl_Position = u_ProjectionMatrix * vec4( lv2, 1.0 );
    v_color = v_pointColor[0];
    v_normal = v_pointNormal[0].xyz;
    v_surfaceUV = vec2( -1.0, 1.0 );
    EmitVertex();

    // v3
    gl_Position = u_ProjectionMatrix * vec4( lv3, 1.0 );
    v_color = v_pointColor[0];
    v_normal = v_pointNormal[0].xyz;
    v_surfaceUV = vec2( 1.0, -1.0 );
    EmitVertex();

    // v4
    gl_Position = u_ProjectionMatrix * vec4( lv4, 1.0 );
    v_color = v_pointColor[0];
    v_normal = v_pointNormal[0].xyz;
    v_surfaceUV = vec2( 1.0, 1.0 );
    EmitVertex();

    EndPrimitive();





/*
    // Color and normal
    vec4 lc1 = v_pointColor[0];
    vec4 lc2 = v_pointColor[0];
    vec3 ln1 = v_pointNormal[0].xyz;
    vec3 ln2 = v_pointNormal[0].xyz;

    /////////////////////////////////////////////////////////////////////////////////////
    // Calculate plane and triangles:

    // Tangent -> given by the vertices
    vec3 tangent = normalize( lv2 - lv1 );

    // For the plane -> binormal
    vec3 biNormal1 = cross( ln1, tangent );
    vec3 biNormal2 = cross( ln2, tangent );

    // transfer normals to screenspace:
    vec3 viewN1 = ln1;
    vec3 viewN2 = ln2;

    // NOTE: the vertices should have a distance of 2 - two times a normalized binormal in the host code around the center point. So it NOW has a
    // length of 2

    // Offset each vertex, and use binormal to create a width
    vec3 tv1 = ( ( lv1 - tangent * 0.25 * length ) // move outside to create length, 0.25? Because we do this on two sides+the dist of lv1 and lv2 is
                                                   // 2
                 + dist * ln1 ) // lift the vertex a bit above the surface
               - biNormal1 * 0.5 * width; // move on surface to the side a bit

    vec3 tv2 = ( ( lv2 + tangent * 0.25 * length ) + dist * ln2 )
               - biNormal2 * 0.5 * width;

    vec3 tv3 = ( ( lv1 - tangent * 0.25 * length ) + dist * ln1 )
               + biNormal1 * 0.5 * width;

    vec3 tv4 = ( ( lv2 + tangent * 0.25 * length ) + dist * ln2 )
               + biNormal2 * 0.5 * width;

    gl_Position = u_ProjectionMatrix * vec4( tv1, 1.0 );
    v_color = lc1;
    v_normal = viewN1;

    // use tex coords to encode direction and center line. X is the width and Y the length
    v_surfaceUV = vec2( -1.0, -1.0 );
    EmitVertex();

    // v2
    gl_Position = u_ProjectionMatrix * vec4( tv2, 1.0 );
    v_color = lc2;
    v_normal = viewN2;
    v_surfaceUV = vec2( -1.0, 1.0 );
    EmitVertex();

    // v3
    gl_Position = u_ProjectionMatrix * vec4( tv3, 1.0 );
    v_color = lc1;
    v_normal = viewN1;
    v_surfaceUV = vec2( 1.0, -1.0 );
    EmitVertex();

    // v4
    gl_Position = u_ProjectionMatrix * vec4( tv4, 1.0 );
    v_color = lc2;
    v_normal = viewN2;
    v_surfaceUV = vec2( 1.0, 1.0 );
    EmitVertex();

    EndPrimitive();*/
}


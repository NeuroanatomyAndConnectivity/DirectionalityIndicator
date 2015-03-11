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

in vec4 v_color;
in vec3 v_normal;
in vec2 v_surfaceUV;

out vec4 fragColor;

// NOTE the following is LIB code. Load Shading.glsl on host side
float blinnPhongIlluminationIntensityFullDiffuse( in vec3 normal );

void main()
{
    // arrow :-)
    float shade = 1.0;

    // the head: above a threshold along y axis (longitudinal)
    if( v_surfaceUV.y >= 0.5 )
    {
        float arrowHeadY = 1.0 - 2.0 * ( v_surfaceUV.y - 0.5 );

        float leftEdge = -arrowHeadY;
        float rightEdge = arrowHeadY;

        float leftStep = 1.0 - smoothstep( leftEdge - 0.15, leftEdge, v_surfaceUV.x );
        float rightStep = smoothstep( rightEdge - 0.15, rightEdge, v_surfaceUV.x );

        shade = 1.0 - ( rightStep + leftStep );
    }
    else
    {
        float body = abs( v_surfaceUV.x );
        float skip = smoothstep( 0.50, 0.65, body );
        shade = 1.0 - skip;
    }

    // discard
    if( shade < 0.01 )
    {
       // discard;
    }

    // Light
    float light = blinnPhongIlluminationIntensityFullDiffuse( normalize( v_normal.xyz ) );

    // Write
    vec4 finalColor = vec4( light * v_color.rgb * shade, v_color.a );
    fragColor = finalColor;
    fragColor = v_color.rgba;

    // Small depth offset
    gl_FragDepth = gl_FragCoord.z;
}


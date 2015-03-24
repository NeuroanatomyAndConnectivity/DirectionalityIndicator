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

flat in vec4 v_color;
in vec3 v_normal;
in vec4 v_posView;
in vec3 v_vector;
in float v_vectorLength;

uniform mat4 u_ViewMatrix;

out vec4 fragColor;
out vec4 fragVec;
out vec4 fragNormal;
out vec4 fragPos;

// NOTE the following is LIB code. Load Shading.glsl on host side
float blinnPhongIlluminationIntensityFullDiffuse( in vec3 normal );

void main()
{
    float light = blinnPhongIlluminationIntensityFullDiffuse( normalize( v_normal.rgb ) );

    // Write
    fragColor = vec4( light * v_color.xyz, 1.0 );
    fragVec = vec4( v_vector.xyz, v_vectorLength );
    fragPos = v_posView;
    fragNormal = vec4( normalize( v_normal ), 1.0 );
}


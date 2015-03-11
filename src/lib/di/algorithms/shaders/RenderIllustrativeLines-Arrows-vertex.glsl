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

// Textures
uniform sampler2D u_colorSampler;
uniform sampler2D u_vecSampler;
uniform sampler2D u_normalSampler;
uniform sampler2D u_posSampler;

// Uniforms
uniform vec2 u_viewportScale = vec2( 1.0 );

// Attribute data
in vec3 position;

// Varying out
out vec4 v_pointColor;
out vec4 v_pointPos;
out vec4 v_pointVec;
out vec4 v_pointNormal;

void main()
{
    // IMPORTANT: works only with GL_NEAREST filtering
    vec2 texCoord = u_viewportScale * position.xy;

    v_pointColor  = texture( u_colorSampler, texCoord.xy );
    v_pointPos    = texture( u_posSampler, texCoord.xy );
    v_pointVec    = texture( u_vecSampler, texCoord.xy );
    v_pointNormal = texture( u_normalSampler, texCoord.xy );

    gl_Position = vec4( position, 1.0 );
}


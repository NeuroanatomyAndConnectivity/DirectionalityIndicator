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

uniform sampler2D u_colorSampler;
uniform sampler2D u_noiseSampler;
uniform sampler2D u_depthSampler;

in vec2 v_texCoord;

out vec4 fragColor;

void main()
{
    vec4 color = texture( u_colorSampler,  v_texCoord ).rgba;
    float noise = texture( u_noiseSampler, v_texCoord ).r;
    float depth = texture( u_depthSampler, v_texCoord ).r;
    fragColor = vec4( color.rgb, color.a );
    gl_FragDepth = depth;
}


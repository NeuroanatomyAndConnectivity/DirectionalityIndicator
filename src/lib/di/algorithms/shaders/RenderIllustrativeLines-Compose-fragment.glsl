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

// Uniforms
uniform sampler2D u_meshColorSampler;
uniform sampler2D u_arrowColorSampler;
uniform sampler2D u_meshDepthSampler;
uniform sampler2D u_arrowDepthSampler;

// Varyings
in vec2 v_texCoord;

// Outputs
out vec4 fragColor;

void main()
{
    vec4 meshColor = texture( u_meshColorSampler,  v_texCoord ).rgba;
    float meshDepth = texture( u_meshDepthSampler, v_texCoord ).r;
    vec4 arrowColor = texture( u_arrowColorSampler,  v_texCoord ).rgba;
    float arrowDepth = texture( u_arrowDepthSampler, v_texCoord ).r;

    if( arrowDepth > meshDepth )
    {
        fragColor = meshColor;
    }
    else
    {
        fragColor = arrowColor;
    }
}


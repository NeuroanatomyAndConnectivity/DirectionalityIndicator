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
uniform sampler2D u_colorSampler;
uniform sampler2D u_vecSampler;
uniform sampler2D u_depthSampler;
uniform sampler2D u_edgeSampler;
uniform sampler2D u_noiseSampler;
uniform sampler2D u_advectSampler;

uniform bool u_useHighContrast = true;

// Varyings
in vec2 v_texCoord;

// Outputs
out vec4 fragColor;

void main()
{
    vec4 color = texture( u_colorSampler,  v_texCoord ).rgba;
    vec3 vec = texture( u_vecSampler, v_texCoord ).rgb;
    float depth = texture( u_depthSampler, v_texCoord ).r;
    float edge = texture( u_edgeSampler, v_texCoord ).r;
    float noise = texture( u_noiseSampler, v_texCoord ).r;
    float advect = texture( u_advectSampler, v_texCoord ).r;

    // Nearly trivial Depth based halo ... looks rather ugly -> replace by SSAO
    float depthLodMost  = textureLod( u_depthSampler, v_texCoord, 5.0 ).r;
    float depthLodMore  = textureLod( u_depthSampler, v_texCoord, 3.0 ).r;
    float depthLodLess  = texture( u_depthSampler, v_texCoord ).r;
    float depthHalo1 = ( 5.0 * ( ( depthLodLess - depthLodMore ) ) );

    depthHalo1 = 1. - depthHalo1;
    depthHalo1 *= depthHalo1 * depthHalo1 * depthHalo1 * depthHalo1;
    depthHalo1 = min( 1.0, depthHalo1 );
    float depthHalo2 = ( 5.0 * ( ( depthLodLess - depthLodMost ) ) );

    depthHalo2 = 1. - depthHalo2;
    depthHalo2 *= depthHalo2 * depthHalo2 * depthHalo2 * depthHalo2;
    depthHalo2 = min( 1.0, depthHalo2 );

    float depthHalo = smoothstep( 0.2, 1.0, depthHalo2) * smoothstep( 0.5, 1.0,  depthHalo2 );

    float u_contrastingS = u_useHighContrast ? 9.0 : 2.5;
    float u_contrastingP = u_useHighContrast ? 4 : 2.5;
    vec3 plainColor = mix( color.rgb, vec3( u_contrastingS * pow( advect, u_contrastingP ) ), 0.4 );

    vec4 col = vec4(
        depthHalo *
            mix( plainColor.rgb, vec3( 0.5 ), 1.0 * edge ),
        color.a
    );

    fragColor = col;
    gl_FragDepth = depth;
}


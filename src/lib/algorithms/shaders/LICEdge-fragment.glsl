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
uniform sampler2D u_depthSampler;
uniform vec2 u_viewportSize;

// Varyings
in vec2 v_texCoord; // normalized texel coords!

// Outputs
out vec4 fragEdge;

void main()
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    // GETTING TEXELS
    //
    // Get surrounding texels; needed for ALL filters
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Get physical texel coordinate
    ivec2 texel = ivec2( int( u_viewportSize.x * v_texCoord.x ),
                         int( u_viewportSize.y * v_texCoord.y ) );

    // get data of surrounding textels
    float c  = texelFetch( u_depthSampler, texel, 0 ).r;
    float bl = texelFetch( u_depthSampler, texel + ivec2( -1, -1 ), 0 ).r;
    float l  = texelFetch( u_depthSampler, texel + ivec2( -1,  0 ), 0 ).r;
    float tl = texelFetch( u_depthSampler, texel + ivec2( -1,  1 ), 0 ).r;
    float t  = texelFetch( u_depthSampler, texel + ivec2(  0,  1 ), 0 ).r;
    float tr = texelFetch( u_depthSampler, texel + ivec2(  1,  1 ), 0 ).r;
    float r  = texelFetch( u_depthSampler, texel + ivec2(  1,  0 ), 0 ).r;
    float br = texelFetch( u_depthSampler, texel + ivec2(  1,  1 ), 0 ).r;
    float b  = texelFetch( u_depthSampler, texel + ivec2(  0, -1 ), 0 ).r;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // LAPLACE
    //
    // apply a standart laplace filter kernel
    /////////////////////////////////////////////////////////////////////////////////////////////

    // laplace filter kernel
    fragEdge = vec4(
        10.0 * abs(
            0.0 * tl +  1.0 * t + 0.0 * tr +
            1.0 * l  + -4.0 * c + 1.0 * r  +
            0.0 * bl +  1.0 * b + 0.0 * br
        ), c, 0.0, 1.0 );    // also store noise in this texture
}


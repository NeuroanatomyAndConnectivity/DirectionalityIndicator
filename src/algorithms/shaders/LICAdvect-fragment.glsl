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
uniform sampler2D u_noiseSampler;
uniform sampler2D u_vecSampler;
uniform sampler2D u_edgeSampler;

uniform vec2 u_viewportSize;
uniform vec2 u_viewportScale;

// Varyings
in vec2 v_texCoord; // normalized texel coords!

// Outputs
out vec4 fragAdvect;

/**
 * The blending ratio between noise and advected noise
 */
uniform float u_noiseRatio = 0.0;

/**
 * Number of iterations per frame.
 */
uniform int u_numIter = 30;

/**
 * Returns the vector at the given point.
 *
 * \param pos the position to retrieve the vector for
 *
 * \return the unscaled vector in [-1, 1]
 */
vec2 getVec( in vec2 pos )
{
    vec2 vec = texture( u_vecSampler, pos ).rg;
    return ( 2.0 * ( vec - vec2( 0.5, 0.5 ) ) );
}

/**
 * Returns noise for the given position.
 *
 * \param pos the position
 *
 * \return noise
 */
float getNoise( in vec2 pos )
{
    return texture( u_noiseSampler, pos ).r;
}

/**
 * Returns depth for the given position.
 *
 * \param pos the position
 *
 * \return depth
 */
float getDepth( in vec2 pos )
{
    return texture( u_depthSampler, pos  ).r;
}

/**
 * Returns edge for the given position.
 *
 * \param pos the position
 *
 * \return edge
 */
float getEdge( in vec2 pos )
{
    return texture( u_edgeSampler, pos ).r;
}

void main()
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    // Advection loop. Simple Euler integration.
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Get physical texel coordinate
    vec2 texel = v_texCoord;
    float offsetX = u_viewportScale.x * ( 1.0 / u_viewportSize.x );
    float offsetY = u_viewportScale.y * ( 1.0 / u_viewportSize.y );

    // get some needed values
    float noise = getNoise( texel );
    vec2 vec    = getVec( texel );

    // simply iterate along the line using the vector at each point
    vec2 lastVec1 = vec;
    vec2 lastPos1 = texel;
    vec2 lastVec2 = vec;
    vec2 lastPos2 = texel;
    float sum = 0.0;
    int m = 2 * u_numIter;
    float scaler1 = 1.0;
    float scaler2 = 1.0;
    for( int i = 0; i < u_numIter; ++i )
    {
        vec2 newPos1 = lastPos1 + vec2( lastVec1.x * offsetX, lastVec1.y * offsetY );
        vec2 newPos2 = lastPos2 - vec2( lastVec2.x * offsetX, lastVec2.y * offsetY );
        vec2 newVec1 = getVec( newPos1 );
        vec2 newVec2 = -getVec( newPos2 );

        // if( ( length( newVec1 ) < 0.01 ) || ( length( newVec2 )  < 0.01 ) )
        // {
        //     m = 2 * i;
        //     break;
        // }

        // it is also possible to scale using a Geometric progression: float( u_numIter - i ) / u_numIter * texture2D
        sum += scaler1 * getNoise( newPos1 );
        sum += scaler2 * getNoise( newPos2 );

        if( getEdge( newPos1 ) > 0.5 )
        {
            scaler1 = 0.0;
        }

        if( getEdge( newPos2 ) > 0.5 )
        {
            scaler2 = 0.0;
        }

        lastPos1 = newPos1;
        lastVec1 = newVec1;
        lastPos2 = newPos2;
        lastVec2 = newVec2;
    }

    // the sum needs to be scaled to [0,1] again
    float n = sum / float( m );
    /*if( depth > 0.99 )
    {
        n = noise;
    }*/

    // laplace filter kernel
    fragAdvect = vec4( vec3( ( n * ( 1.0 - u_noiseRatio ) ) + ( noise * u_noiseRatio ) ), 1.0 );
}


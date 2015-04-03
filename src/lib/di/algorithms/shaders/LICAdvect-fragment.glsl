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
uniform int u_numIter =  50;

/**
 * Returns the vector at the given point.
 *
 * \param pos the position to retrieve the vector for
 *
 * \return the unscaled vector in [-1, 1]
 */
vec2 getVec( in vec2 pos, out float len )
{
    vec2 vec = texture( u_vecSampler, pos ).rg;
    len = length( vec );
    return vec / len;
}

/**
 * Check if the position is somewhere outside the rendered area
 *
 * \param pos position to check
 *
 * \return 1.0 if outaide. 0.0 otherwise
 */
float outside( in vec2 pos )
{
    return 1.0 - texture( u_vecSampler, pos ).a;
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
    float vecLen = 0.0;
    vec2 vec    = getVec( texel, vecLen );
    if( vecLen < 0.0001 )
    {
        fragAdvect = vec4( vec3( 0.0 ), 1.0 );
        return;
    }

    // simply iterate along the line using the vector at each point
    vec2 lastVec1 = vec;
    vec2 lastPos1 = texel;
    vec2 lastVec2 = vec;
    vec2 lastPos2 = texel;
    float sum = 0.0;
    int m = 0;//2 * u_numIter;
    float scaler1 = 1.0;
    float scaler2 = 1.0;
    for( int i = 0; i < u_numIter; ++i )
    {
        vec2 newPos1 = lastPos1 + vec2( lastVec1.x * offsetX, lastVec1.y * offsetY );
        vec2 newPos2 = lastPos2 - vec2( lastVec2.x * offsetX, lastVec2.y * offsetY );
        float vecLen1 = 0.0;
        float vecLen2 = 0.0;
        vec2 newVec1 = getVec( newPos1, vecLen1 );
        vec2 newVec2 = -getVec( newPos2, vecLen2 );

        if( vecLen1 < 0.0001 )
        {
            break;
        }
        if( vecLen2 < 0.0001 )
        {
            break;
        }

        // it is also possible to scale using a Geometric progression: float( u_numIter - i ) / u_numIter * texture2D
        sum += scaler1 * getNoise( newPos1 );
        sum += scaler2 * getNoise( newPos2 );

        m += int( scaler1 ) + int( scaler2 );

        if( outside( newPos1 ) > 0.0 )
        {
            scaler1 = 0.0;
        }

        if( outside( newPos2 ) > 0.0 )
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

    // laplace filter kernel
    fragAdvect = vec4( vec3( ( n * ( 1.0 - u_noiseRatio ) ) + ( noise * u_noiseRatio ) ), 1.0 );
}


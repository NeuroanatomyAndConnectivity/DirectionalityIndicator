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
uniform sampler2D u_meshNormalSampler;
uniform sampler2D u_noiseSampler;


uniform vec2 u_viewportScale = vec2( 1.0 );

// Varyings
in vec2 v_texCoord;
in float v_zoom;

// Outputs
out vec4 fragColor;
out vec4 fragAO;

vec4 getNormal( vec2 where, float lod )
{
    return texture( u_meshNormalSampler, where );
}

float getDepth( vec2 where, float lod )
{
    //return textureLod( u_meshDepthSampler, where, lod ).r;
    return min( textureLod( u_meshDepthSampler, where, lod ).r,
                textureLod( u_arrowDepthSampler, where, lod ).r - 0.005 );
}

vec3 getNoiseAsVector( vec2 where )
{
    return texture( u_noiseSampler, where ).rgb * 2.0 - vec3( 1.0 );
}

float getZoom( vec2 where )
{
    return 2.0 * v_zoom;
}

float getInfluence( vec2 where )
{
    float d = texture( u_arrowDepthSampler, where ).r;
    if( d < 0.99 )
    {
        return 2.0;
    }
    return 1.0;
}


// Shading.glsl:
float getLineAO( vec2 where, vec2 px2tx );

void main()
{
    ivec2 texSize = textureSize( u_meshColorSampler, 0 );
    vec2 px2tx = vec2( 1.0 / texSize.x, 1.0 / texSize.y );

    vec4 meshColor = texture( u_meshColorSampler,  v_texCoord ).rgba;
    float meshDepth = texture( u_meshDepthSampler, v_texCoord ).r;
    vec4 arrowColor = texture( u_arrowColorSampler,  v_texCoord ).rgba;
    float arrowDepth = texture( u_arrowDepthSampler, v_texCoord ).r;

    float ssao = getLineAO( v_texCoord, px2tx );

    vec4 finalColor = mix( arrowColor, meshColor, 0.0 );//arrowColor;
    float finalDepth = arrowDepth;
    // Manual depth test.
    if( arrowDepth > meshDepth )
    {
        finalColor = meshColor;
        finalDepth = meshDepth;
    }

    fragColor = vec4( finalColor );
    fragAO = vec4( ssao, 0.0, 0.0, 1.0 );
    gl_FragDepth = finalDepth;
}


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

// Define input and output
layout( points ) in;
#ifdef d_curvatureEnable
layout( triangle_strip, max_vertices = d_curvatureNumVerts ) out;
#else
layout( triangle_strip, max_vertices = 4 ) out;
#endif

// Uniforms
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform vec2 u_viewportScale = vec2( 1.0 );

uniform float u_width = 1.5;
uniform float u_height = 5.0;
uniform float u_dist = 2.0;

// Textures
uniform sampler2D u_colorSampler;
uniform sampler2D u_vecSampler;
uniform sampler2D u_normalSampler;
uniform sampler2D u_posSampler;

// Outputs
out vec4 v_color;
out vec3 v_normal;
out vec2 v_surfaceUV;

struct PointInfo
{
    vec4 pointColor;
    vec4 pointPos;
    vec4 pointVec;
    vec4 pointNormal;
};

PointInfo getPointInfo( vec2 where )
{
    PointInfo result;

    // IMPORTANT: works only with GL_NEAREST filtering
    vec2 texCoord = u_viewportScale * where;

    result.pointColor  = texture( u_colorSampler, texCoord.xy );
    result.pointPos    = texture( u_posSampler, texCoord.xy );
    result.pointVec    = texture( u_vecSampler, texCoord.xy );
    result.pointNormal = texture( u_normalSampler, texCoord.xy );

    return result;
}

#ifdef d_curvatureEnable

void main()
{
    /////////////////////////////////////////////////////////////////////////////////////
    // Given:

    float width = u_width;
    float height = u_height;
    float dist = u_dist;

    // Initial position
    vec2 pos = gl_in[0].gl_Position.xy;

    // Iterate and add segments
    for( int segment = 0; segment < d_curvatureNumSegments; ++segment )
    {
        PointInfo pinfo = getPointInfo( pos );

        // Parameterize along the surface
        float longitudinalParam = float( segment ) / float( d_curvatureNumSegments - 1.0 );

        // float scale = clamp( 0.005 * pinfo.pointVec.w, 0.0001, 0.005 );

        // NOTE: magic number: it represents the scaling between texture space and actual transformed world space the arrows reside in ....
        float scale = 0.005;
        float lscale = scale * 2.0 * height / float( d_curvatureNumSegments );
        float wscale = scale * width;

        vec3 p = pinfo.pointPos.xyz;
        v_color = pinfo.pointColor;
        v_normal = normalize( pinfo.pointNormal.xyz );

        vec3 tangent = normalize( pinfo.pointVec.xyz );
        vec3 binormal =  normalize( cross( tangent, v_normal ) );

        if( length( pinfo.pointVec.xyz ) < 0.00001 )
        {
            break;
        }

        p += scale * v_normal * dist;

        vec3 lv1 = p - ( binormal * wscale );
        vec3 lv2 = p + ( binormal * wscale );

        // v1
        gl_Position = u_ProjectionMatrix * vec4( lv1, 1.0 );
        v_surfaceUV = vec2( -1.0, 2.0 * longitudinalParam - 1.0 );
        EmitVertex();

        // v2
        gl_Position = u_ProjectionMatrix * vec4( lv2, 1.0 );
        v_surfaceUV = vec2(  1.0, 2.0 * longitudinalParam - 1.0 );
        EmitVertex();

        // Follow the tangential direction
        pos += tangent.xy * lscale;

        if( length( tangent.xy ) < 0.001 )
        {
            break;
        }
    }

    EndPrimitive();
}

#else

void main()
{
    PointInfo pinfo = getPointInfo( gl_in[0].gl_Position.xy );

    /////////////////////////////////////////////////////////////////////////////////////
    // Given:

    float width = u_width;
    float height = u_height;
    float dist = u_dist;

    // float scale = 0.005;
    // float scale = clamp( 0.005 * 5.0 * smoothstep( 0.1, 0.9, 0.2 + v_pointVec[0].w ), 0.0025, 0.005 );
    float scale = clamp( 0.005 * pinfo.pointVec.w, 0.0001, 0.005 );
    float lscale = scale * 2.0 * height;
    float wscale = scale * width;

    vec3 p = pinfo.pointPos.xyz;
    v_color = pinfo.pointColor;
    v_normal = normalize( pinfo.pointNormal.xyz );

    vec3 tangent = normalize( pinfo.pointVec.xyz );
    vec3 binormal =  normalize( cross( tangent, v_normal ) );

    if( length( pinfo.pointVec.xyz ) < 0.00001 )
    {
        return;
    }

    p += scale * v_normal * dist;

    vec3 lv1 = p - ( tangent * 0.0 )
                 - ( binormal * wscale );
    vec3 lv2 = p - ( tangent * 0.0 )
                 + ( binormal * wscale );
    vec3 lv3 = p + ( tangent * lscale )
                 - ( binormal * wscale );
    vec3 lv4 = p + ( tangent * lscale )
                 + ( binormal * wscale );

    // v1
    gl_Position = u_ProjectionMatrix * vec4( lv1, 1.0 );
    v_surfaceUV = vec2( -1.0, -1.0 );
    EmitVertex();

    // v2
    gl_Position = u_ProjectionMatrix * vec4( lv2, 1.0 );
    v_surfaceUV = vec2(  1.0, -1.0 );
    EmitVertex();

    // v3
    gl_Position = u_ProjectionMatrix * vec4( lv3, 1.0 );
    v_surfaceUV = vec2( -1.0, 1.0 );
    EmitVertex();

    // v4
    gl_Position = u_ProjectionMatrix * vec4( lv4, 1.0 );
    v_surfaceUV = vec2( 1.0, 1.0 );
    EmitVertex();

    EndPrimitive();
}

#endif


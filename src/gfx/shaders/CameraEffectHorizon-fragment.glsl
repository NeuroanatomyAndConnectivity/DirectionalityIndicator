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

in float vert;

vec4 u_bottom1Color = vec4( 1.0 );
vec4 u_bottom2Color = vec4( vec3( 0.82 ), 1.0 );

vec4 u_top1Color = vec4( vec3( 0.85 ), 1.0 );
vec4 u_top2Color = vec4( vec3( 0.95 ), 1.0 );

float u_horizon = 33;

out vec4 fragColor;

void main()
{
    vec4 color = vec4( 1.0 );
    float horizonLine = u_horizon / 100.0;
    float bottom = float( vert <= horizonLine );
    float horizon = horizonLine + 0.0001;
    float topScale = ( vert - horizon ) / ( 1.0 - horizon );
    color = bottom           * mix( u_bottom1Color, u_bottom2Color, vert / horizon ) +
           ( 1.0 - bottom ) * mix( u_top1Color, u_top2Color, topScale );
    fragColor = vec4( color.rgb, 1.0 );
}


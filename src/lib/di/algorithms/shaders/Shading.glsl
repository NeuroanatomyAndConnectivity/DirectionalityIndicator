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

/**
 * A struct containing the needed light and material parameters commonly used in most shaders.
 *
 * \note This is for evaluating the phong equation for 1 channel only.
 */
struct LightIntensityParameter
{
    // These 4 parameters are similar to those in gl_MaterialParameters
    float materialAmbient;  //!< Material ambient intensity.
    float materialDiffuse;  //!< Material diffuse intensity.
    float materialSpecular; //!< Material Specular intensity.
    float materialShinines; //!< Material shinines factor

    // These 4 parametes are a stripped down version of gl_LightSourceParameters
    float lightDiffuse;     //!< Light diffuse intensity.
    float lightAmbient;     //!< Light ambient intensity.
};

/**
 * This variable contains the OpenWalnut default light. You should definitely use this for your lighting to obtain an identical look for all
 * rendered images.
 */
LightIntensityParameter DefaultLightIntensity = LightIntensityParameter(
    0.04,                            // material ambient
    0.75,                            // material diffuse
    1.0,                             // material specular
    250.0,                           // material shininess
    1.0,                             // light diffuse
    1.0                              // light ambient
);

/**
 * This variable contains the OpenWalnut default light. You should definitely use this for your lighting to obtain an identical look for all
 * rendered images. This version looks has no ambient factor and provides full diffuse colors.
 */
LightIntensityParameter DefaultLightIntensityFullDiffuse = LightIntensityParameter(
    0.0,                             // material ambient
    1.0,                             // material diffuse
    1.0,                             // material specular
    250.0,                           // material shininess
    1.0,                             // light diffuse
    1.0                              // light ambient
);

/**
 * This variable contains the OpenWalnut default light. You should definitely use this for your lighting to obtain an identical look for all
 * rendered images. This version looks a little bit more metallic.
 */
LightIntensityParameter DefaultLightIntensityLessDiffuse = LightIntensityParameter(
    0.0,                             // material ambient
    0.35,                            // material diffuse
    1.0,                             // material specular
    250.0,                           // material shininess
    1.0,                             // light diffuse
    1.0                              // light ambient
);

/**
 * Function to calculate lighting based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 *
 * \param ambient   materials ambient color
 * \param diffuse   materials diffuse color
 * \param specular  materials specular color
 * \param shininess material shininess
 * \param lightColor  the light color
 * \param ambientLight the ambient light color
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the color.
 */
vec4 blinnPhongIllumination( vec3 ambient, vec3 diffuse, vec3 specular, float shininess,
                             vec3 lightColor, vec3 ambientLight,
                             vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    normalDir *= sign( dot( normalDir, viewDir ) );

    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    vec3 ambientV = ambient * ambientLight;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
    vec3 diffuseV = diffuse * diffuseLight;

    // compute specular term
    float specularLight = pow( max( dot( H, normalDir ), 0.0 ), shininess );
    if( diffuseLight <= 0.) specularLight = 0.;
    vec3 specularV = specular * specularLight;

    return vec4( ambientV + ( diffuseV + specularV ) * lightColor, 1.0 );
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * \param ambient   materials ambient intensity
 * \param diffuse   materials diffuse intensity
 * \param specular  materials specular intensity
 * \param shininess material shininess
 * \param lightIntensity  the light intensity
 * \param ambientIntensity the ambient light intensity
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the light intensity.
 */
float blinnPhongIlluminationIntensity( float ambient, float diffuse, float specular, float shininess,
                                       float lightIntensity, float ambientIntensity,
                                       vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    normalDir *= sign( dot( normalDir, viewDir ) );

    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    float ambientV = ambient * ambientIntensity;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
    float diffuseV = diffuse * diffuseLight;

    // compute specular term
    float specularLight = pow( max( dot( H, normalDir ), 0.0 ), shininess );
    if( diffuseLight <= 0.) specularLight = 0.;
    float specularV = specular * specularLight;

    return ambientV + ( diffuseV + specularV ) * lightIntensity;
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * \param parameter the LightIntensityParameter defining material and light
 * \param normal the normal. Needs to be normalized.
 *
 * \return lighting intensity.
 */
float blinnPhongIlluminationIntensity( in LightIntensityParameter parameter, in vec3 normal )
{
    return blinnPhongIlluminationIntensity(
        parameter.materialAmbient,
        parameter.materialDiffuse,
        parameter.materialSpecular,
        parameter.materialShinines,
        parameter.lightDiffuse,
        parameter.lightAmbient,
        normal,
        vec3( 0.0, 0.0, 1.0 ),
        vec3( 0.0, 0.0, 1.0 )
        );
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity. This uses the DefaultLightIntensity.
 *
 * \param normal the normal. Must be normalized beforehand
 *
 * \return the light intensity
 */
float blinnPhongIlluminationIntensity( in vec3 normal )
{
    return blinnPhongIlluminationIntensity( DefaultLightIntensity, normal );
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity. This uses the DefaultLightIntensityFullDiffuse.
 *
 * \param normal the normal. Must be normalized beforehand
 *
 * \return the light intensity
 */
float blinnPhongIlluminationIntensityFullDiffuse( in vec3 normal )
{
    return blinnPhongIlluminationIntensity( DefaultLightIntensityFullDiffuse, normal );
}

/**
 * Function to calculate lighting intensity based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity. This uses the DefaultLightIntensityFullDiffuse.
 *
 * \param normal the normal. Must be normalized beforehand
 *
 * \return the light intensity
 */
float blinnPhongIlluminationIntensityFullDiffuse( in vec3 normal, in float specularity )
{
    LightIntensityParameter params = DefaultLightIntensityFullDiffuse;
    params.materialSpecular *= specularity;
    return blinnPhongIlluminationIntensity( params, normal );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This is based on the LineAO algorithm from http://doi.ieeecomputersociety.org/10.1109/TVCG.2012.142
//
//  - This implementation matches the paper in most cases. We made some additional improvements and
//    simplifications here. This simply is due to the time-lag between first submission and final acceptance.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * LineAO parameter collection
 */
struct LineAOParameter
{
    /**
     * The total influence of SSAO.
     */
    float lineaoTotalStrength;
    /**
     * The radius of the hemispshere in screen-space which gets scaled.
     */
    float lineaoRadiusSS;          // Should be the same default as in WGEPostprocessorLineAO.cpp
    /**
     * The strength of the occluder influence in relation to the geometry density. The higher the value, the larger the influence. Low values remove
     * the drop-shadow effect.
     */
    float lineaoDensityWeight;     // Should be the same default as in WGEPostprocessorLineAO.cpp
};

/**
 * Some defaults for LineAO.
 */
LineAOParameter DefaultLineAO = LineAOParameter(
    1.0,
    1.0,
    1.0
);

// Forwards:
vec4 getNormal( vec2 where, float lod );
float getDepth( vec2 where, float lod );
float getZoom( vec2 where );
float getInfluence( vec2 where );
vec3 getNoiseAsVector( vec2 where );

uniform int u_samples = 16;

/**
 * Calculate the screen-space ambient occlusion LineAO for the given pixel.
 */
float getLineAO( vec2 where, vec2 px2tx, LineAOParameter params )
{
    #define SCALERS 2  // how much hemispheres to sample?
    float invSamples = 1.0 / float( u_samples );

    // Fall-off for SSAO per occluder. This should be zero (or nearly zero) since it defines what is counted as before, or behind.
    float falloff = 0.001;

    // grab a random normal for reflecting the sample rays later on
    vec3 randNormal = normalize( getNoiseAsVector( where * 10.0 ) );

    // grab the current pixel's normal and depth
    vec3 currentPixelSample = getNormal( where, 0 ).xyz;
    float currentPixelDepth = getDepth( where, 0 );

    // current fragment coords in screen space
    vec3 ep = vec3( where.xy, currentPixelDepth );

    // get the normal of current fragment
    vec3 normal = currentPixelSample.xyz;

    // the radius of the sphere is, in screen-space, half a pixel. So the hemisphere covers nearly one pixel. Scaling by depth somehow makes it
    // more invariant for zooming
    float radius = ( getZoom( where ) * max( px2tx.x, px2tx.y ) * params.lineaoRadiusSS );// / ( 1.0 - currentPixelDepth );

    // some temporaries needed inside the loop
    vec3 ray;                     // the current ray inside the sphere
    vec3 hemispherePoint;         // the point on the hemisphere

    float occluderDepth;          // the depth of the potential occluder on the hemisphere
    float depthDifference;        // the depth difference between the current point and the occluder (point on hemisphere)

    // accumulated occlusion
    float occlusion = 0.0;
    float radiusScaler = 0.0;     // we sample with multiple radii, so use a scaling factor here

    // sample for different radii
    for( int l = 0; l < SCALERS; ++l )
    {
        float occlusionStep = 0.0;  // this variable accumulates the occlusion for the current radius

        // this allows an adaptive radius
        // NOTE: we do not exactly use the linear scaling from the paper here. Although they look very similar, the non-linear radius, which
        // increases non-linearly with the level l, improves visual quality a bit. The drawback of this method is that increasing SCALERS causes
        // larger structures to become more important than local structures.
        #define radScaleMin 1.5

        if( l == 0 )
        {
            radiusScaler = 5.0;
            falloff = 0.001;
        }
        if( l == 1 )
        {
            falloff = 0.01;
            radiusScaler = 125.0;
        }

        // Get SAMPLES-times samples on the hemisphere and check for occluders
        int numSamplesAdded = 0;    // used to count how many samples really got added to the occlusion term
        for( int i = 0; i < u_samples; ++i )
        {
            // grab a rand normal from the noise texture
            vec3 randSphereNormal = getNoiseAsVector( vec2( float( i ) / float( u_samples ),
                                                            float( l + 1 ) / float( SCALERS ) ) );

            // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
            vec3 hemisphereVector = reflect( randSphereNormal, randNormal );
            ray = radiusScaler * radius * hemisphereVector;
            ray = sign( dot( ray, normal ) ) * ray;

            // get the point in texture space on the hemisphere
            hemispherePoint = ray + ep;

            // we need to handle the case where a hemisphere point is outside the texture space. The correct solution would be to ensure that
            // no hemisphere point ever gets outside the texture space. This can be achieved by rendering the scene to a larger texture and than
            // process only the required viewport in this texture. Unfortunately OpenWalnut does not provide an easy to use mechanism for this.
            // So, we skip hemispherePoints outside the texture. This yields nearly no or only small artifacts at the borders if zoomed in.
            if( ( hemispherePoint.x < 0.0 ) || ( hemispherePoint.x > 1.0 ) ||
                ( hemispherePoint.y < 0.0 ) || ( hemispherePoint.y > 1.0 )
              )
            {
                continue;
            }
            // Count the samples we really use.
            numSamplesAdded++;

            float lod = 0.0;
            //#define POSTPROCESSOR_LINEAO_USEGAUSSPYRAMID
            #ifdef POSTPROCESSOR_LINEAO_USEGAUSSPYRAMID
            // select the Level in the gaussian pyramid of the normal, depth and tangent map. When NOT using this, you retain more local detail
            // at more far away occluders which creates crispier images.
            lod = float( l );
            #endif

            // get the depth of the occluder fragment
            occluderDepth = getDepth( hemispherePoint.xy, lod );

            // if depthDifference is negative = occluder is behind the current fragment -> occluding this fragment
            depthDifference = currentPixelDepth - occluderDepth;
            occlusionStep += step( falloff, depthDifference );
        }

        // for this radius, add to total occlusion. Keep in mind to normalize the occlusion term according to the number of samples taken
        occlusion += ( 1.0 / float( numSamplesAdded ) ) * occlusionStep;
    }

    // output the result
    return clamp( 1.0 - ( params.lineaoTotalStrength * occlusion ), 0, 1 );
}

float getLineAO( vec2 where, vec2 px2tx )
{
    return getLineAO( where, px2tx, DefaultLineAO );
}


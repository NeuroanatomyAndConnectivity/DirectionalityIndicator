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



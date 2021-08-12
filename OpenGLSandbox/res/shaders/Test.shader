#shader vertex
#version 330 core

layout( location = 0 ) in vec4 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 uv;

uniform mat4 u_ModelToClip; // Model View Projection matrix

out vec2 _uv;
out vec3 _normal;
out vec3 _modelPos;

void main() {
    // Transform vertex position to clip space
    v_ClipPos = position * u_ModelToClip; // in hlsl mul(position, u_ModelToClip)
    // Pass inputs without modifications
    _normal = normal;
    _uv = uv;
    _modelPos = position;

    gl_Position = v_ClipPos;
}

#shader fragment
#version 330 core

// layout( location = 0 ) out vec4 color; // MUST HAVE ! Pixel shader output color

// Values from vertex shader
in vec2 _uv;
in vec3 _normal;
in vec3 _modelPos;

// A global constant RGB and opacity
uniform vec4 constantColor;
// Omni light position, in MODEL SPACE
uniform vec3 omniPos;
// Reciprocal of omni light radius. ( The light will falloff
// linearly to zero at this radius ). Note that it's common to truck
// this into the w component of the position, to reduce the number of
// constants, since each constant usually takes a full 4d vector slot
uniform float invOmniRad;
// Unattenuated omni light color
uniform vec3 omniColor;
// View position, in MODEL SPACE
uniform vec3 viewPos;
// Constant ambient light color
uniform vec3 ambientLightColor;
// Material glossiness (phong exponent)
uniform float specExponent;
// Diffuse and specular map samplers. Note we assume that diffuse
// and spec maps use the same UV coords
uniform sampler2D u_diffuseMap;
uniform sampler2D u_specularMap;

void main() {
    // color = texture( u_Texture, v_TexCoord ) * vec4( v_Color, 1.0f ); // texture sampling

    // Fetch the texels to get the material colors
    vec4 matDiff = texture( u_diffuseMap, _uv );
    vec4 matSpec = texture( u_specularMap, _uv );
    // Normalize interpolated vertex normal
    vec3 N = normalize( _normal );
    // Compute vector to light
    vec3 L = omniPos - _modelpos;
    // Normalize it, and save off distance to use later
    // for attenuation
    float dist = length( L );
    L /= dist;
    // Compute view vector and halfway vector
    vec3 V = normalize( viewPos - modelPos );
    vec3 H = normalize( V + L );
    // Compute attenuated light color
    vec3 lightColor = omniColor * max( 1 - dist * invOmniRad, 0 );
    // Compute diffuse and specular factors
    float diffFactor = max( dot( N, L ), 0 );
    float specFactor = pow( max( dot( N, H ), 0 ), specExponent );
    // Compute effecttive light colors
    vec3 diffColor = lightColor* diffFactor* ambientLightColor;
    vec3 specColor = lightColor* specFactor;
    // Sum up colors 
    vec4 result = matDiff; // RGB and opacity from diffuse map
    result.rgb *= diffColor; // modulate by diffuse + ambient lightning
    result.rgb += matSpec.rgb * specColor; // add spec ignoring map alpha
    // Modulate it by constant and outpuut
    gl_FragColor = result * constantColor;
}
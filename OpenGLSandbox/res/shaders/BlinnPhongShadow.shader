#shader vertex
#version 330 core
layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoords;

out vec2 TexCoords;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main() {
    vs_out.FragPos = vec3( u_Model * vec4( aPos, 1.0 ) );
    vs_out.Normal = transpose( inverse( mat3( u_Model ) ) ) * aNormal;
    vs_out.TexCoords = aTexCoords;

    // ( Light projection and view ) * ( world fragment position )
    vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4( vs_out.FragPos, 1.0 );
    
    gl_Position = u_ProjectionView * u_Model * vec4( aPos, 1.0 );
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D   u_DiffuseTexture;
uniform sampler2D   u_ShadowMap;
uniform vec3        u_LightPos;
uniform vec3        u_ViewPos;
uniform vec3        u_LightColor;

// Zero or one value
float CalculateShadow( vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, sampler2D shadowMap ) {
    // perform perspective divide
    // When using an orthographic projection matrix the w component of a vertex remains untouched
    // so this step is actually quite meaningless.
    // However, it is necessary when using perspective projection so keeping this line ensures it works with both projection matrices.
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // from NDC [ -1, 1 ] range transform to [ 0, 1 ] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture( shadowMap, projCoords.xy ).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;


    // A shadow bias of 0.005 solves the issues of our scene by a large extent,
    // but you can imagine the bias value is highly dependent on the angle between the light source and the surface.
    // If the surface would have a steep angle to the light source, the shadows may still display shadow acne.
    // A more solid approach would be to change the amount of bias based on the surface angle towards the light:
    // something we can solve with the dot product
    float bias = max( 0.05 * ( 1.0 - dot( normal, lightDir ) ), 0.005 );

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize( shadowMap, 0 );
    for( int x = -1; x <= 1; ++x )     {
        for( int y = -1; y <= 1; ++y )         {
            float pcfDepth = texture( shadowMap, projCoords.xy + vec2( x, y ) * texelSize ).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if( projCoords.z > 1.0 ) {
        shadow = 0.0;
    }

    // check whether current frag pos is in shadow
    return shadow;
}

void main() {
    vec3 color = texture( u_DiffuseTexture, fs_in.TexCoords ).rgb;
    vec3 normal = normalize( fs_in.Normal );

    // ambient
    vec3 ambient = 0.1 * u_LightColor;
    
    // diffuse
    vec3 lightDir = normalize( u_LightPos - fs_in.FragPos );
    float diff = max( dot( lightDir, normal ), 0.0 );
    vec3 diffuse = diff * u_LightColor;
    
    // specular
    vec3 viewDir = normalize( u_ViewPos - fs_in.FragPos );
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = 0.0;
    vec3 halfwayDir = normalize( lightDir + viewDir );
    spec = pow( max( dot( normal, halfwayDir ), 0.0 ), 64.0 );
    vec3 specular = spec * u_LightColor;
    
    // calculate shadow
    float shadow = CalculateShadow( fs_in.FragPosLightSpace, normal, lightDir, u_ShadowMap );
    vec3 lighting = ( ambient + ( 1.0 - shadow ) * ( diffuse + specular ) ) * color;

    FragColor = vec4( lighting, 1.0 );
}
#shader vertex
#version 330 core
layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

out     vec2 TexCoords;
uniform mat4 u_PVM;
uniform mat4 u_Model;
uniform bool u_ReverseNormals;

void main() {
    vs_out.FragPos = vec3( u_Model * vec4( aPos, 1.0 ) );
    
    // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
    if( u_ReverseNormals ) {
        vs_out.Normal = transpose( inverse( mat3( u_Model ) ) ) * ( -1.0 * aNormal );
    } else {
        vs_out.Normal = transpose( inverse( mat3( u_Model ) ) ) * aNormal;
    }

    vs_out.TexCoords = aTexCoords;
    gl_Position = u_PVM * vec4( aPos, 1.0 );
}




#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D   u_DiffuseTexture;
uniform samplerCube u_DepthMap;
uniform vec3        u_LightPos;
uniform vec3        u_ViewPos;
uniform float       u_FarPlane;
uniform bool        u_CalculateShadows;


float ShadowCalculation( vec3 fragPos ) {
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - u_LightPos;
    
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length( fragToLight );

    // PCF
    float   shadow  = 0.0;
    float   bias    = 0.15;
    int     samples = 20;

    float viewDistance  = length( u_ViewPos - fragPos );
    float diskRadius    = ( 1.0 + ( viewDistance / u_FarPlane ) ) / 25.0;
    
    // array of offset direction for sampling
    vec3 gridSamplingDisk[20] = vec3[](
        vec3( 1, 1, 1 ), vec3( 1, -1, 1 ), vec3( -1, -1, 1 ), vec3( -1, 1, 1 ),
        vec3( 1, 1, -1 ), vec3( 1, -1, -1 ), vec3( -1, -1, -1 ), vec3( -1, 1, -1 ),
        vec3( 1, 1, 0 ), vec3( 1, -1, 0 ), vec3( -1, -1, 0 ), vec3( -1, 1, 0 ),
        vec3( 1, 0, 1 ), vec3( -1, 0, 1 ), vec3( 1, 0, -1 ), vec3( -1, 0, -1 ),
        vec3( 0, 1, 1 ), vec3( 0, -1, 1 ), vec3( 0, -1, -1 ), vec3( 0, 1, -1 )
    );

    for( int i = 0; i < samples; ++i ) {
        float closestDepth = texture( u_DepthMap, fragToLight + gridSamplingDisk[i] * diskRadius ).r;
        closestDepth *= u_FarPlane;   // undo mapping [0;1]
        if( currentDepth - bias > closestDepth ) {
            shadow += 1.0;
        }
    }
    shadow /= float( samples );
    return shadow;
}

void main() {
    vec3 color = texture( u_DiffuseTexture, fs_in.TexCoords ).rgb;
    vec3 normal = normalize( fs_in.Normal );
    vec3 lightColor = vec3( 0.3 );

    // ambient
    vec3 ambient = 0.3 * lightColor;

    // diffuse
    vec3 lightDir = normalize( u_LightPos - fs_in.FragPos );
    float diff = max( dot( lightDir, normal ), 0.0 );
    vec3 diffuse = diff * lightColor;

    // specular
    float spec = 0.0;
    vec3 viewDir = normalize( u_ViewPos - fs_in.FragPos );
    vec3 reflectDir = reflect( -lightDir, normal );
    vec3 halfwayDir = normalize( lightDir + viewDir );
    spec = pow( max( dot( normal, halfwayDir ), 0.0 ), 64.0 );
    vec3 specular = spec * lightColor;

    // calculate shadow
    float shadow = u_CalculateShadows ? ShadowCalculation( fs_in.FragPos ) : 0.0;
    vec3 lighting = ( ambient + ( 1.0 - shadow ) * ( diffuse + specular ) ) * color;

    FragColor = vec4( lighting, 1.0 );
}
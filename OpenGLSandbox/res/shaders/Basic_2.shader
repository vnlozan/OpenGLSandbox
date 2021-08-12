#shader vertex
#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 texCoord;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 u_Model;

// Model View Projection matrix
uniform mat4 u_MVP;

void main() {
    FragPos = vec3( u_Model * vec4( position, 1.0 ) );
    // IS ESSENTIAL if non-uniform scale is done
    // expensive operation on GPU, for every vertex
    // from model to world space, using normal matrix
    // should be done on cpu side for better performance
    Normal = mat3( transpose( inverse( u_Model ) ) ) * normal;

    gl_Position = u_MVP * vec4( position, 1.0f );
    TexCoord = texCoord;
}



#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D u_Texture;
uniform vec3 u_LightColor;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
// PHONG LIGHTNING
void main() {
    // AMBIENT
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor;

    // DIFFUSE 
    vec3 norm = normalize( Normal );
    vec3 lightDir = normalize( u_LightPos - FragPos );
    float diff = max( dot( norm, lightDir ), 0.0 );
    vec3 diffuse = diff * u_LightColor;

    // SPECULAR
    float specularStrength = 0.5;
    vec3 viewDir = normalize( u_ViewPos - FragPos );
    // To calculate reflect vector we should do r = 2 * dot( Normal, lightDir ) * Normal - lightDir;
    // For a given incident vector I and surface normal
    // N reflect returns the reflection direction calculated as I - 2.0 * dot(N, I) * N.
    vec3 reflectDir = reflect( -lightDir, norm );
    // The higher the shininess value of an object, the more it properly reflects the light 
    // instead of scattering it all around and thus the smaller the highlight becomes. 
    int shininess = 32;
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), shininess );
    vec3 specular = specularStrength * spec * u_LightColor;


    vec4 objectColor = texture( u_Texture, TexCoord );
    vec3 result = ( ambient + diffuse + specular ) * objectColor.rgb;
    FragColor = vec4( result, 1.0 );
}
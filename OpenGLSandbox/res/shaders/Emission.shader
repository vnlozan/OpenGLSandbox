#shader vertex
#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 texCoord;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

// as example
layout( std140 ) uniform ub_Matrices{
    mat4 u_Projection;
    mat4 u_View;
};
uniform mat4 u_Model;

void main() {
    FragPos = vec3( u_Model * vec4( position, 1.0 ) );
    // IS ESSENTIAL if non-uniform scale is done
    // expensive operation on GPU, for every vertex
    // from model to world space, using normal matrix
    // should be done on cpu side for better performance
    Normal = mat3( transpose( inverse( u_Model ) ) ) * normal;

    gl_Position = u_Projection * u_View * u_Model * vec4( position, 1.0f );
    TexCoord = texCoord;
}

#shader fragment
#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    // The higher the shininess value of an object, the more it properly reflects the light 
    // instead of scattering it all around and thus the smaller the highlight becomes. 
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 u_ViewPos;
uniform float u_Time;
uniform Material u_Material;
uniform Light u_Light;

out vec4 FragColor;

vec3 calculate_emission() {
    // float step(float edge, float x)  
    // step() generates a step function by comparing x to edge.
    // For element i of the return value, 0.0 is returned if x[i] < edge[i], and 1.0 is returned otherwise.
    vec3 show = step( vec3( 1.0 ), vec3( 1.0 ) - texture( u_Material.specular, TexCoord ).rgb );
    if( show == vec3( 0.0 ) ) {
        return show;
    }

    // SIMPLE TEXTURE
    //vec3 emission = texture( u_Material.emission, TexCoord ).rgb;

    // MOVING TEXTURE
    vec3 emission = texture( u_Material.emission, TexCoord + vec2( 0.0, u_Time ) ).rgb;

    // FADING TEXTURE
    //emission *= ( sin( u_Time ) * 0.5 + 0.5 ) * 2.0;

    return emission;
}
// PHONG LIGHTNING
void main() {
    // Set materials
    vec3 matDiffuse = vec3( texture( u_Material.diffuse, TexCoord ) );
    vec3 matSpecular = vec3( texture( u_Material.specular, TexCoord ) );

    // AMBIENT
    vec3 ambient = u_Light.ambient * matDiffuse;

    // DIFFUSE
    vec3 norm = normalize( Normal );
    vec3 lightDir = normalize( u_Light.position - FragPos );
    float diff = max( dot( norm, lightDir ), 0.0 );
    vec3 diffuse = u_Light.diffuse * diff * matDiffuse;

    // SPECULAR
    vec3 viewDir = normalize( u_ViewPos - FragPos );
    // To calculate reflect vector we should do r = 2 * dot( Normal, lightDir ) * Normal - lightDir;
    // For a given incident vector I and surface normal
    // N reflect returns the reflection direction calculated as I - 2.0 * dot(N, I) * N.
    vec3 reflectDir = reflect( -lightDir, norm );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), u_Material.shininess );
    vec3 specular = u_Light.specular * spec * matSpecular;
    // vec3 specular = light.specular * spec * ( vec3( 1.0 ) - matSpecular ); // INVERSE

    // EMISSION
    vec3 emission = calculate_emission();

    // RESULT
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4( result, 1.0 );
}
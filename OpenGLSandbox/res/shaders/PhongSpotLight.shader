#shader vertex
#version 330 core
layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_MVP;

void main() {
    FragPos = vec3( u_Model * vec4( aPos, 1.0 ) );
    Normal = mat3( transpose( inverse( u_Model ) ) ) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = u_MVP * vec4( aPos, 1.0f );
}

#shader fragment
#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform Light u_Light;

void main() {

    // ambient
    vec3 ambient = u_Light.ambient * texture( u_Material.diffuse, TexCoords ).rgb;

    // diffuse 
    vec3 norm = normalize( Normal );
    vec3 lightDir = normalize( u_Light.position - FragPos );
    float diff = max( dot( norm, lightDir ), 0.0 );
    vec3 diffuse = u_Light.diffuse * diff * texture( u_Material.diffuse, TexCoords ).rgb;

    // specular
    vec3 viewDir = normalize( u_ViewPos - FragPos );
    vec3 reflectDir = reflect( -lightDir, norm );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), u_Material.shininess );
    vec3 specular = u_Light.specular * spec * texture( u_Material.specular, TexCoords ).rgb;

    // spotlight (soft edges)
    // check if lighting is inside the spotlight cone
    float theta = dot( lightDir, normalize( -u_Light.direction ) );
    float epsilon = ( u_Light.cutOff - u_Light.outerCutOff );
    float intensity = clamp( ( theta - u_Light.outerCutOff ) / epsilon, 0.0, 1.0 );
    diffuse *= intensity;
    specular *= intensity;

    // attenuation
    float distance = length( u_Light.position - FragPos );
    float attenuation = 1.0 / ( u_Light.constant + u_Light.linear * distance + u_Light.quadratic * ( distance * distance ) );
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4( result, 1.0 );
}
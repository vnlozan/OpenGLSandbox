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

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct DirectionLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define NR_POINT_LIGHTS 4

uniform vec3            u_ViewPos;
uniform DirectionLight  u_DirectionLight;
uniform PointLight      u_PointLights[NR_POINT_LIGHTS];
uniform SpotLight       u_SpotLight;
uniform Material        u_Material;

vec3 ProcessDirectionLight( DirectionLight light, vec3 normal, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular ) {
    vec3 lightDir = normalize( -light.direction );
    // diffuse
    float diff = max( dot( normal, lightDir ), 0.0 );
    // specular
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), u_Material.shininess );

    vec3 ambient = light.ambient * matDiffuse;
    vec3 diffuse = light.diffuse * diff * matDiffuse;
    vec3 specular = light.specular * spec * matSpecular;
    return ( ambient + diffuse + specular );
}
vec3 ProcessPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular ) {
    vec3 lightDir = normalize( light.position - fragPos );
    // diffuse
    float diff = max( dot( normal, lightDir ), 0.0 );
    // specular
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), u_Material.shininess );
    // attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0 / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );

    vec3 ambient = light.ambient * matDiffuse;
    vec3 diffuse = light.diffuse * diff * matDiffuse;
    vec3 specular = light.specular * spec * matSpecular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ( ambient + diffuse + specular );
}
vec3 ProcessSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular ) {
    vec3 lightDir = normalize( light.position - fragPos );
    // diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    // specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), u_Material.shininess );
    // attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0 / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    // spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );
    // combine results
    vec3 ambient = light.ambient * matDiffuse;
    vec3 diffuse = light.diffuse * diff * matDiffuse;
    vec3 specular = light.specular * spec * matSpecular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return ( ambient + diffuse + specular );
}

void main() {
    vec3 norm = normalize( Normal );
    vec3 viewDir = normalize( u_ViewPos - FragPos );

    vec3 matDiffuse = vec3( texture( u_Material.diffuse, TexCoords ) );
    vec3 matSpecular = vec3( texture( u_Material.specular, TexCoords ) );

    vec3 result = ProcessDirectionLight( u_DirectionLight, norm, viewDir, matDiffuse, matSpecular );

    for( int i = 0; i < NR_POINT_LIGHTS; i++ ) {
        result += ProcessPointLight( u_PointLights[i], norm, FragPos, viewDir, matDiffuse, matSpecular );
    }

    result += ProcessSpotLight( u_SpotLight, norm, FragPos, viewDir, matDiffuse, matSpecular );

    FragColor = vec4( result, 1.0 );
}
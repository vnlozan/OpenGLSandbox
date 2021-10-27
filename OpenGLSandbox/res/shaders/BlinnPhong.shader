#shader vertex
#version 330 core
layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoords;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 u_VP;

void main() {
    vs_out.FragPos = aPos;
    vs_out.Normal = aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = u_VP * vec4( aPos, 1.0 );
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D u_Texture;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform bool u_Blinn;

void main() {
    vec3 color = texture( u_Texture, fs_in.TexCoords ).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize( u_LightPos - fs_in.FragPos );
    vec3 normal = normalize( fs_in.Normal );
    float diff = max( dot( lightDir, normal ), 0.0 );
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize( u_ViewPos - fs_in.FragPos );
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = 0.0;
    if( u_Blinn )     {
        vec3 halfwayDir = normalize( lightDir + viewDir );
        spec = pow( max( dot( normal, halfwayDir ), 0.0 ), 32.0 );
    }     else     {
        vec3 reflectDir = reflect( -lightDir, normal );
        spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), 8.0 );
    }
    vec3 specular = vec3( 0.3 ) * spec; // assuming bright white light color
    FragColor = vec4( ambient + diffuse + specular, 1.0 );
}
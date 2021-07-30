#shader vertex
#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP; // Model View Projection matrix

void main() {
    gl_Position = u_MVP * vec4( position, 1.0f );
    v_TexCoord = texCoord;
}



#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec3 u_LightColor;

void main() {
    FragColor = texture( u_Texture, v_TexCoord );
    FragColor.rgb *= u_LightColor;
}
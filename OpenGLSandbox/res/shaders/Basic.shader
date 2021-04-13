#shader vertex
#version 330 core

layout( location = 0 ) in vec4 position;
layout( location = 1 ) in vec2 texCoord;
layout( location = 2 ) in vec3 specColor;

out vec2 v_TexCoord;
out vec3 v_Color;

uniform mat4 u_MVP; // Model View Projection matrix

void main() {
    gl_Position = u_MVP * position;
    v_TexCoord  = texCoord;
    v_Color     = specColor;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Color;

//uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main() {
    color = texture( u_Texture, v_TexCoord ) * vec4( v_Color, 1.0f ); // texture sampling
}
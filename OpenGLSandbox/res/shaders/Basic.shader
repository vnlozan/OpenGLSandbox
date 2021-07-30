#shader vertex
#version 330 core

layout( location = 0 ) in vec4 position;
layout( location = 1 ) in vec2 texCoord;

uniform mat4 u_MVP; // Model View Projection matrix

void main() {
    gl_Position = u_MVP * position;
}

#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec3 u_Color;

void main() {
    FragColor = vec4( u_Color, 1.0f ); // texture sampling
}
#shader vertex
#version 330 core

layout( location = 0 ) in vec3 position;

uniform mat4 u_MVP; // Model View Projection matrix

void main() {
    gl_Position = u_MVP * vec4( position, 1.0f );
}

#shader fragment
#version 330 core

out vec4 FragColor;
uniform vec3 u_Color;

void main() {
    FragColor = vec4( u_Color, 1.0f );
}
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

void main() {
    FragColor = vec4( vec3( gl_FragCoord.z ), 1.0f );
}
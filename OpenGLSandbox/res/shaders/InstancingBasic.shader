#shader vertex
#version 330 core
layout( location = 0 ) in vec2 aPos;
layout( location = 1 ) in vec3 aColor;

out vec3 fColor;

uniform vec2 u_Offsets[100];

void main() {
    vec2 offset = u_Offsets[gl_InstanceID];
    gl_Position = vec4( aPos + offset, 0.0, 1.0 );
    fColor = aColor;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 fColor;

void main() {
    FragColor = vec4( fColor, 1.0 );
}
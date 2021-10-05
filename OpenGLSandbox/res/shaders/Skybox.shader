#shader vertex
#version 330 core

layout( location = 0 ) in vec3 position;

out vec3 TexCoords;

uniform mat4 u_VP; // View Projection

void main() {
    TexCoords = position;
    vec4 pos = u_VP * vec4( position, 1.0 );
    // z as w => 1/1 = 1 which is maximum depth value, Z value, the most distant textures
    gl_Position = pos.xyww;
}

#shader fragment
#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube u_Skybox;

void main() {
    FragColor = texture( u_Skybox, TexCoords ); // position as texture coordinate
}
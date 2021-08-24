#shader vertex
#version 330 core

layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoords;
layout( location = 3 ) in vec3 aTan;
layout( location = 4 ) in vec3 aBitan;

out vec2 TexCoords;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4( aPos, 1.0f );
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_TextureDiffuse1;


void main() {
    FragColor = texture( u_TextureDiffuse1, TexCoords );
    //FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}
#shader vertex
#version 330 core

layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoords;
layout( location = 3 ) in vec3 aTan;
layout( location = 4 ) in vec3 aBitan;

out vec2 TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 u_Rotation;
uniform float u_Angle;

mat4 rotationMatrix( vec3 axis, float angle ) {
    axis = normalize( axis );
    float s = sin( angle );
    float c = cos( angle );
    float oc = 1.0 - c;

    return mat4( oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
        oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
        oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
        0.0, 0.0, 0.0, 1.0 );
}

void main() {
    mat4 rotation = rotationMatrix( vec3( 0.0f, 1.0f, 0.0f ), u_Angle );
    //gl_Position = u_Projection * u_View * u_Model * u_Rotation * vec4( aPos, 1.0f );
    gl_Position = u_Projection * u_View * u_Model * rotation * vec4( aPos, 1.0f );
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_TextureSpecular1;
uniform sampler2D u_TextureDiffuse1;
uniform sampler2D u_TextureHeight1;

void main() {

    FragColor = texture( u_TextureDiffuse1, TexCoords );
    //FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}
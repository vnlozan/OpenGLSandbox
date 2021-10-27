#shader vertex
#version 330 core
layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec2 aTexCoords;
layout( location = 5 ) in mat4 aInstanceMatrix; // 5,6,7,8

out vec2 TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform float u_Angle;

/* Rotate around an arbitrary 3D axis we can combine all 3 them by first rotating around the X-axis, then Y and then Z */
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
    TexCoords = aTexCoords;
    mat4 rotMat = rotationMatrix( vec3( 0.0f, 1.0f, 0.0f ), u_Angle ); // object rotation in world coords
    mat4 rotMat2 = rotationMatrix( vec3( 0.0f, 0.0f, 1.0f ), u_Angle ); // object rotation in self coords
    gl_Position = u_Projection * u_View * rotMat * aInstanceMatrix * rotMat2 * vec4( aPos, 1.0f ); // Multiplication from the left
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
}
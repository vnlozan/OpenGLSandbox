#shader vertex
#version 330 core
layout( location = 0 ) in vec3 aPos;

uniform mat4 u_Model;

void main() {
    gl_Position = u_Model * vec4( aPos, 1.0 );
}



#shader geometry
#version 330 core
layout( triangles ) in;
layout( triangle_strip, max_vertices = 18 ) out;

uniform mat4 u_ShadowMatrices[6];
out     vec4 FragPos; 

void main() {
    for( int face = 0; face < 6; ++face ) {

        // Specify output face to which we render
        // This only works when we have a cubemap texture attached to the active framebuffer.
        gl_Layer = face;

        for( int i = 0; i < 3; ++i ) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = u_ShadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}



#shader fragment
#version 330 core

in      vec4    FragPos;
uniform vec3    u_LightPos;
uniform float   u_FarPlane;

void main() {
    float lightDistance = length( FragPos.xyz - u_LightPos );

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / u_FarPlane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
#shader vertex
#version 330 core
layout( location = 0 ) in vec3 aPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out VS_OUT{
    mat4 projection;
    mat4 view;
} vs_out;

void main() {
    
    vs_out.projection = u_Projection;
    vs_out.view = u_View;

    gl_Position = u_Model * vec4( aPos, 1.0 );
}

#shader geometry
#version 330 core
layout( triangles ) in;
layout( line_strip, max_vertices = 2 ) out;

const float MAGNITUDE = 0.2;

in VS_OUT{
    mat4 projection;
    mat4 view;
} gs_in[];

vec3 GetTriangleCenter(vec3 p1, vec3 p2, vec3 p3) {
    float l1 = length( p1 );
    float l2 = length( p2 );
    float l3 = length( p3 );
    return ( l1 * p1 + l2 * p2 + l3 * p3 ) / ( l1 + l2 + l3 );
}
vec3 GetNormalVector( vec3 p1, vec3 p2, vec3 p3 ) {
    vec3 a = p3 - p1;
    vec3 b = p2 - p1;
    return normalize( cross( a, b ) );
}

void main() {
    vec3 normal = GetNormalVector( vec3(gl_in[0].gl_Position ), vec3( gl_in[1].gl_Position ), vec3( gl_in[2].gl_Position ) );
    vec3 startPoint = GetTriangleCenter( vec3( gl_in[0].gl_Position ), vec3( gl_in[1].gl_Position ), vec3( gl_in[2].gl_Position ) );
    vec3 endPoint = startPoint + normal * MAGNITUDE;

    gl_Position = gs_in[0].projection * gs_in[0].view * vec4( startPoint, 1.0f );
    EmitVertex();

    gl_Position = gs_in[0].projection * gs_in[0].view * vec4( endPoint, 1.0f );
    EmitVertex();

    EndPrimitive();
}

#shader fragment
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4( 1.0, 1.0, 0.0, 1.0 );
}

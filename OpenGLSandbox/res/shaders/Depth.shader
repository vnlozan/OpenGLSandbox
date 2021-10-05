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

float near = 0.1;
float far = 100.0;

float LinearizeDepth( float depth ) {
    float z = depth * 2.0 - 1.0; // back to normalized device coordinates (NDC) [-1,1]
    return ( 2.0 * near * far ) / ( far + near - z * ( far - near ) );
}

void main() {

    //FragColor = vec4( vec3( gl_FragCoord.z ), 1.0f );

    float depth = LinearizeDepth( gl_FragCoord.z ) / far; // divide by far for demonstration
    FragColor = vec4( vec3( depth ), 1.0 );
}
#shader vertex
#version 330 core
layout( location = 0 ) in vec3 l_Pos;
layout( location = 1 ) in vec3 l_Normal;

out vec3 Normal;
out vec3 Position;

uniform mat4 u_Model;
uniform mat4 u_MVP;

void main() {
    Normal = mat3( transpose( inverse( u_Model ) ) ) * l_Normal;
    Position = vec3( u_Model * vec4( l_Pos, 1.0 ) );
    gl_Position = u_MVP * vec4( l_Pos, 1.0 );
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 u_CameraPos;
uniform samplerCube u_Skybox;
uniform int u_Mode;

vec4 skyboxReflectionColor( vec3 pos, vec3 cameraPos, vec3 normal, samplerCube skybox ) {
    // View direction vector I
    vec3 I = normalize( pos - cameraPos );
    // The reflection vector, vector R is used as a direction vector to index/sample the cubemap,
    // returning color value of the environment
    // The resulting effect is that the object seems to reflect the skybox
    vec3 R = reflect( I, normalize( normal ) );
    return vec4( texture( skybox, R ).rgb, 1.0 );
}
vec4 skyboxRefractionColor( vec3 pos, vec3 cameraPos, vec3 normal, samplerCube skybox ) {
    // Mat:     Refractive index:
    // Air      1.00
    // Water    1.33
    // Ice      1.309
    // Glass    1.52
    // Diamond  2.42
    float ratio = 1.00 / 1.52; // ratio between both materials' refractive indices
    vec3 I = normalize( pos - cameraPos );
    vec3 R = refract( I, normalize( normal ), ratio );
    return vec4( texture( skybox, R ).rgb, 1.0 );
}

void main() {
    if( u_Mode == 1 ) {
        FragColor = skyboxReflectionColor( Position, u_CameraPos, Normal, u_Skybox );
    } else if( u_Mode == 2 ) {
        FragColor = skyboxRefractionColor( Position, u_CameraPos, Normal, u_Skybox );
    } else {
        FragColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
    }
}
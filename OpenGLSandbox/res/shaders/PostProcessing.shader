#shader vertex
#version 330 core
layout( location = 0 ) in vec2 position;
layout( location = 1 ) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4( position.x, position.y, 0.0, 1.0 );
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform int u_Mode;
uniform sampler2D u_ScreenTexture;

vec3 Inversion( sampler2D map, vec2 texCoords ) {
    return vec3( 1.0 - texture( map, texCoords ).rgb );
}
vec3 Grayscale( sampler2D map, vec2 texCoords ) {
    vec3 _map = texture( map, texCoords ).rgb;
    //float average = ( _map.r + _map.g + _map.b ) / 3.0;
    // human eye tends to be more sensitive to green colors and the least to blue
    float average = 0.2126 * _map.r + 0.7152 * _map.g + 0.0722 * _map.b;
    return vec3( average, average, average );
}
vec3 SharpenKernel( sampler2D map, vec2 texCoords ) {
    const float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2( -offset,  offset ),   // top-left
        vec2( 0.0f,     offset ),   // top-center
        vec2( offset,   offset ),   // top-right
        vec2( -offset,  0.0f ),     // center-left
        vec2( 0.0f,     0.0f ),     // center-center
        vec2( offset,   0.0f ),     // center-right
        vec2( -offset,  -offset ),  // bottom-left
        vec2( 0.0f,     -offset ),  // bottom-center
        vec2( offset,   -offset )   // bottom-right    
    );
    float kernel[9] = float[](
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for( int i = 0; i < 9; i++ ) {
        sampleTex[i] = vec3( texture( map, texCoords.st + offsets[i] ) );
    }
    vec3 col = vec3( 0.0 );
    for( int i = 0; i < 9; i++ ) {
        col += sampleTex[i] * kernel[i];
    }
    return col;
}
vec3 Blur( sampler2D map, vec2 texCoords ) {
    const float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2( -offset, offset ),   // top-left
        vec2( 0.0f, offset ),   // top-center
        vec2( offset, offset ),   // top-right
        vec2( -offset, 0.0f ),     // center-left
        vec2( 0.0f, 0.0f ),     // center-center
        vec2( offset, 0.0f ),     // center-right
        vec2( -offset, -offset ),  // bottom-left
        vec2( 0.0f, -offset ),  // bottom-center
        vec2( offset, -offset )   // bottom-right    
    );
    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );
    vec3 sampleTex[9];
    for( int i = 0; i < 9; i++ ) {
        sampleTex[i] = vec3( texture( map, texCoords.st + offsets[i] ) );
    }
    vec3 col = vec3( 0.0 );
    for( int i = 0; i < 9; i++ ) {
        col += sampleTex[i] * kernel[i];
    }
    return col;
}
vec3 EdgeDetection( sampler2D map, vec2 texCoords ) {
    const float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2( -offset, offset ),   // top-left
        vec2( 0.0f, offset ),   // top-center
        vec2( offset, offset ),   // top-right
        vec2( -offset, 0.0f ),     // center-left
        vec2( 0.0f, 0.0f ),     // center-center
        vec2( offset, 0.0f ),     // center-right
        vec2( -offset, -offset ),  // bottom-left
        vec2( 0.0f, -offset ),  // bottom-center
        vec2( offset, -offset )   // bottom-right    
    );
    float kernel[9] = float[](
        1.0, 1.0, 1.0,
        1.0, -8.0, 1.0,
        1.0, 1.0, 1.0
    );

    vec3 sampleTex[9];
    for( int i = 0; i < 9; i++ ) {
        sampleTex[i] = vec3( texture( map, texCoords.st + offsets[i] ) );
    }
    vec3 col = vec3( 0.0 );
    for( int i = 0; i < 9; i++ ) {
        col += sampleTex[i] * kernel[i];
    }
    return col;
}

void main() {
    switch( u_Mode ) {
        case 1: FragColor = vec4( Inversion( u_ScreenTexture, TexCoords ), 1.0 ); break;
        case 2: FragColor = vec4( Grayscale( u_ScreenTexture, TexCoords ), 1.0 ); break;
        case 3: FragColor = vec4( SharpenKernel( u_ScreenTexture, TexCoords ), 1.0 ); break;
        case 4: FragColor = vec4( Blur( u_ScreenTexture, TexCoords ), 1.0 ); break;
        default: FragColor = vec4( texture( u_ScreenTexture, TexCoords ).rgb, 1.0 ); break;
    }
}
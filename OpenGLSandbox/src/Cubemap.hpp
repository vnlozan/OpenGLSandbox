#pragma once
#include "Renderer.h"
#include <iostream>
#include "stb_image/stb_image.h"

class Cubemap {
public:
	Cubemap( std::vector<std::string>& texturePaths ) : m_Files( texturePaths ), m_RendererId{ 0 } {
        stbi_set_flip_vertically_on_load( false );

        GLCall( glGenTextures( 1, &m_RendererId ) );
        Bind();

        int width, height, nrChannels;
        for( unsigned int i = 0; i < texturePaths.size(); i++ ) {
            unsigned char* data = stbi_load( texturePaths[i].c_str(), &width, &height, &nrChannels, 0 );
            if( data ) {
                glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
                stbi_image_free( data );
            } else {
                std::cout << "Cubemap tex failed to load at path: " << texturePaths[i] << std::endl;
                stbi_image_free( data );
            }
        }
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
        
        //Unbind();
	}
    void ActivateTexture( unsigned int slot /* = 0 */ ) const {
        GLCall( glActiveTexture( GL_TEXTURE0 + slot ) );
    }
    void Bind() {
        GLCall( glBindTexture( GL_TEXTURE_CUBE_MAP, m_RendererId ) );
    }
    void Unbind() {
        GLCall( glBindTexture( GL_TEXTURE_CUBE_MAP, 0 ) );
    }
    ~Cubemap() {

    }
public:
	std::vector<std::string> m_Files;
	unsigned int m_RendererId;
};
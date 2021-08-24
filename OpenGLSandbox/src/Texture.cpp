#include <iostream>

#include "stb_image/stb_image.h"

#include "Texture.h"

Texture::Texture( const char* path, Texture::TYPE type )
	: m_RendererID{ 0 }, type{ type }, m_FilePath{ path }, m_LocalBuffer{ nullptr }, m_Height{ 0 }, m_Width{ 0 } {

	stbi_set_flip_vertically_on_load( true );

	GLCall( glGenTextures( 1, &m_RendererID ) );
	
	m_LocalBuffer = stbi_load( path, &m_Width, &m_Height, &m_BPP, 0 );
	if( m_LocalBuffer ) {
		GLenum format;
		switch( m_BPP ) {
			case 1	: format = GL_RED;		break;
			case 3	: format = GL_RGB;		break;
			case 4	: format = GL_RGBA;		break;
			default : format = GL_RGBA8;	break;
		}
		GLCall( glBindTexture( GL_TEXTURE_2D, m_RendererID ) );
		// 2. mipmap level
		// 3. rgb format to store
		// 5. deprecated. ALWAYS 0
		GLCall( glTexImage2D( GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer ) );
		GLCall( glGenerateMipmap( GL_TEXTURE_2D ) );
		GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ) );
		GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ) );
		GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ) );
		GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
		stbi_image_free( m_LocalBuffer );
	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free( m_LocalBuffer );
	}
}
Texture::Texture( const Texture& t )
	: type{ t.type }, m_FilePath{ t.m_FilePath }, m_RendererID{ t.m_RendererID }, m_Width{ t.m_Width }, m_Height{ t.m_Height }, m_BPP{ t.m_BPP } {
	std::cout << "COPY CONSTRUCTOR" << std::endl;
	m_LocalBuffer = t.m_LocalBuffer;
}
Texture::Texture( Texture&& t ) noexcept
	: type{ t.type }, m_FilePath{ t.m_FilePath }, m_RendererID{ t.m_RendererID }, m_Width{ t.m_Width }, m_Height{ t.m_Height }, m_BPP{ t.m_BPP } {
	std::cout << "MOVE CONSTRUCTOR" << std::endl;
	m_LocalBuffer = t.m_LocalBuffer;
	t.m_LocalBuffer = nullptr;
}
Texture::~Texture() {
	std::cout << "Texture deleted" << std::endl;
	//GLCall( glDeleteTextures( 1, &m_RendererID ) );
}
void Texture::ActivateTexture( unsigned int slot /* = 0 */ ) const {
	GLCall( glActiveTexture( GL_TEXTURE0 + slot ) );
}
void Texture::Bind() const {
	GLCall( glBindTexture( GL_TEXTURE_2D, m_RendererID ) );
}
void Texture::Unbind() const {
	GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
}

Texture& Texture::operator=( const Texture& rhs ) {
	if( this == &rhs ) {
		return *this;
	}
	type = rhs.type;
	m_FilePath = rhs.m_FilePath;
	m_RendererID = rhs.m_RendererID;
	m_Width = rhs.m_Width;
	m_Height = rhs.m_Height;
	m_BPP = rhs.m_BPP;
	*m_LocalBuffer = *rhs.m_LocalBuffer;
	return *this;
}
Texture& Texture::operator=( Texture&& rhs ) noexcept {
	if( this == &rhs ) {
		return *this;
	}
	type = rhs.type;
	m_FilePath = rhs.m_FilePath;
	m_RendererID = rhs.m_RendererID;
	m_Width = rhs.m_Width;
	m_Height = rhs.m_Height;
	m_BPP = rhs.m_BPP;
	*m_LocalBuffer = *rhs.m_LocalBuffer;
	rhs.m_LocalBuffer = nullptr;
	return *this;
}
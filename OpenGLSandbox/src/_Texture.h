#pragma once

#include <vector>
#include <string>

#include "stb_image/stb_image.h"

#include "Renderer.h"

class _Texture {
public:
	_Texture( int width, int height )
		: m_Width{ width }, m_Height{ height }, m_RendererId{ 0 }, m_DeleteOnDestruct{ false } { glGenTextures( 1, &m_RendererId ); }
	_Texture( const _Texture& t )
		: m_RendererId{ t.m_RendererId }, m_Width{ t.m_Width }, m_Height{ t.m_Height }, m_DeleteOnDestruct{ false } {}
	_Texture( _Texture&& t ) noexcept
		: m_RendererId{ t.m_RendererId }, m_Width{ t.m_Width }, m_Height{ t.m_Height }, m_DeleteOnDestruct{ false } {}
	
	virtual ~_Texture() {
		if( m_DeleteOnDestruct ) {
			GLCall( glDeleteTextures( 1, &m_RendererId ) );
		}
	}
	void SetParameterfv( GLenum target, GLenum pname, const GLfloat* params ) {
		Bind();
		glTexParameterfv( target, pname, params );
		Unbind();
	}
	void SetParameteri( GLenum target, GLenum pname, GLint param ) {
		Bind();
		glTexParameteri( target, pname, param );
		Unbind();
	}
	void SetDeleteOnDestruct( bool deleteOnDestruct=true ) {
		m_DeleteOnDestruct = deleteOnDestruct;
	}

	inline int GetRendererId()	const { return m_RendererId; }
	inline int GetWidth()		const { return m_Width; }
	inline int GetHeight()		const { return m_Height; }
	
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	_Texture& operator=( const _Texture& rhs ) {
		//std::cout << "COPY CONSTRUCTOR=" << std::endl;
		if( this == &rhs ) {
			return *this;
		}
		m_RendererId = rhs.m_RendererId;
		m_Width = rhs.m_Width;
		m_Height = rhs.m_Height;
		return *this;
	}
	_Texture& operator=( _Texture&& rhs ) noexcept {
		//std::cout << "MOVE CONSTRUCTOR=" << std::endl;
		if( this == &rhs ) {
			return *this;
		}
		m_RendererId = rhs.m_RendererId;
		m_Width = rhs.m_Width;
		m_Height = rhs.m_Height;
		return *this;
	}
protected:
	bool			m_DeleteOnDestruct;
	unsigned int	m_RendererId;
	int				m_Width;
	int				m_Height;
};

class _TextureCubemap: public _Texture {
public:
	_TextureCubemap( std::vector<std::string>& texturePaths, bool flipVOnLoad = false )
		: _Texture{ 0, 0 }, m_TexturePaths{ texturePaths } {

		stbi_set_flip_vertically_on_load( flipVOnLoad );

		Bind();

		int width, height, nrChannels;
		for( unsigned int i = 0; i < texturePaths.size(); i++ ) {
			unsigned char* data = stbi_load( texturePaths[i].c_str(), &width, &height, &nrChannels, 0 );
			if( data ) {
				glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
				stbi_image_free( data );
			} else {
				//std::cout << "Cubemap tex failed to load at path: " << texturePaths[i] << std::endl;
				stbi_image_free( data );
			}
		}
		SetParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		SetParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		SetParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		SetParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		SetParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

		Unbind();
	}
	void Bind() const override {
		GLCall( glBindTexture( GL_TEXTURE_CUBE_MAP, m_RendererId ) );
	}
	void Unbind() const override {
		GLCall( glBindTexture( GL_TEXTURE_CUBE_MAP, 0 ) );
	}
	void ActivateTexture( unsigned int slot = 0 ) const {
		GLCall( glActiveTexture( GL_TEXTURE0 + slot ) );
	}
private:
	std::vector<std::string> m_TexturePaths;
};

class _TextureMultisample: public _Texture {
public:
	_TextureMultisample( int width, int height, int format = GL_RGB ) : _Texture{ width, height } {
		Bind();
		glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 4, format, m_Width, m_Height, GL_TRUE );
		Unbind();
	}
	void Bind() const override {
		GLCall( glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, m_RendererId ) );
	}
	void Unbind() const override {
		GLCall( glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, 0 ) );
	}
};

class _Texture2D: public _Texture {
public:
	enum class TYPE {
		BUFFER		= 1,
		DIFFUSE		= 2,
		SPECULAR	= 3,
		NORMAL		= 4,
		HEIGHT		= 5,
		AMBIENT		= 6
	};

	/* Create from image */
	_Texture2D( const char* path, TYPE type = _Texture2D::TYPE::DIFFUSE, bool flipVOnLoad = true )
		: _Texture{ 0, 0 }, type{ type }, m_FilePath{ path }, m_LocalBuffer{ nullptr } {
		
		stbi_set_flip_vertically_on_load( flipVOnLoad );
		
		Bind();

		m_LocalBuffer = stbi_load( path, &m_Width, &m_Height, &m_NrChannels, 0 );
		if( m_LocalBuffer ) {
			GLenum format;
			switch( m_NrChannels ) {
				case 1: format	= GL_RED;	break;
				case 3: format	= GL_RGB;	break;
				case 4: format	= GL_RGBA;	break;
				default: format = GL_RGBA8;	break;
			}
			GLCall( glTexImage2D( GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer ) );
			GLCall( glGenerateMipmap( GL_TEXTURE_2D ) );
			stbi_image_free( m_LocalBuffer );
		} else {
			stbi_image_free( m_LocalBuffer );
		}
		Unbind();
	}
	_Texture2D( const char* path, TYPE type, GLint wrap=GL_REPEAT )
		: _Texture{ 0, 0 }, type{ type }, m_FilePath{ path }, m_LocalBuffer{ nullptr } {

		stbi_set_flip_vertically_on_load( true );

		Bind();

		m_LocalBuffer = stbi_load( path, &m_Width, &m_Height, &m_NrChannels, 0 );
		if( m_LocalBuffer ) {
			GLenum format;
			switch( m_NrChannels ) {
				case 1: format = GL_RED;	break;
				case 3: format = GL_RGB;	break;
				case 4:	format = GL_RGBA;	break;
				default: format = GL_RGBA8;	break;
			}
			GLCall( glTexImage2D( GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer ) );
			GLCall( glGenerateMipmap( GL_TEXTURE_2D ) );
			GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ) );
			GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ) );
			GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ) );
			GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
			stbi_image_free( m_LocalBuffer );
		} else {
			stbi_image_free( m_LocalBuffer );
		}
		Unbind();
	}
	/* Create as buffer */
	_Texture2D( int width, int height, int format=GL_RGB ) : _Texture{ width, height } {
		Bind();
		glTexImage2D( GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_FLOAT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		Unbind();
	}

	_Texture2D( const _Texture2D& t ) : _Texture{ t }, type{ t.type }, m_FilePath{ t.m_FilePath }, m_NrChannels{ t.m_NrChannels } {
		//std::cout << "COPY CONSTRUCTOR" << std::endl;
		m_LocalBuffer = t.m_LocalBuffer;
	}
	_Texture2D( _Texture2D&& t ) noexcept : _Texture{ t }, type{ t.type }, m_FilePath{ t.m_FilePath }, m_NrChannels{ t.m_NrChannels } {
		//std::cout << "MOVE CONSTRUCTOR" << std::endl;
		m_LocalBuffer = t.m_LocalBuffer;
		t.m_LocalBuffer = nullptr;
	}

	~_Texture2D() override {
		if( m_DeleteOnDestruct ) {
			delete m_LocalBuffer;
		}
	}

	void ActivateTexture( unsigned int slot = 0 ) const {
		GLCall( glActiveTexture( GL_TEXTURE0 + slot ) );
	}
	
	void Bind() const override {
		GLCall( glBindTexture( GL_TEXTURE_2D, m_RendererId ) );
	}
	void Unbind() const override {
		GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
	}

	_Texture2D& operator=( const _Texture2D& rhs ) {
		//std::cout << "COPY CONSTRUCTOR=" << std::endl;
		if( this == &rhs ) {
			return *this;
		}

		_Texture::operator=( rhs );

		*m_LocalBuffer	= *rhs.m_LocalBuffer;
		m_FilePath		= rhs.m_FilePath;
		type			= rhs.type;
		m_NrChannels	= rhs.m_NrChannels;

		return *this;
	}
	_Texture2D& operator=( _Texture2D&& rhs ) noexcept {
		//std::cout << "MOVE CONSTRUCTOR=" << std::endl;
		if( this == &rhs ) {
			return *this;
		}
		_Texture::operator=( rhs );

		m_FilePath		= rhs.m_FilePath;
		*m_LocalBuffer	= *rhs.m_LocalBuffer;
		type			= rhs.type;
		m_NrChannels	= rhs.m_NrChannels;

		rhs.m_LocalBuffer = nullptr;
		
		return *this;
	}
public:
	_Texture2D::TYPE	type;
	std::string			m_FilePath;
private:
	unsigned char*		m_LocalBuffer;
	int					m_NrChannels;
};
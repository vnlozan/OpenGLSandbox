#pragma once
#include <string>
#include "Renderer.h"

class Texture {
public:
	enum class TYPE {
		//CUBEMAP = 0,
		BUFFER = 1,
		DIFFUSE = 2,
		SPECULAR = 3,
		NORMAL = 4,
		HEIGHT = 5,
		AMBIENT = 6
	};
	
	Texture( const char* path, Texture::TYPE type = Texture::TYPE::DIFFUSE, int params = GL_REPEAT );
	Texture( int width, int height );
	Texture( const Texture& texture );
	Texture( Texture&& texture ) noexcept;
	~Texture();

	void ActivateTexture( unsigned int slot = 0 ) const;
	void Bind() const;
	void Unbind() const;

	Texture& operator=( const Texture& rhs );
	Texture& operator=( Texture&& rhs ) noexcept;

	inline int	GetWidth() const { return m_Width; }
	inline int	GetHeight()	const { return m_Height; }
	inline unsigned int GetRendererId() { return m_RendererId; }
public:
	Texture::TYPE type;
	std::string m_FilePath;
	unsigned int m_RendererId;
private:
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
};
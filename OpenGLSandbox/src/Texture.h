#pragma once
#include "Renderer.h"

class Texture {
public:
	enum class TYPE {
		DIFFUSE = 0,
		SPECULAR = 1,
		NORMAL = 2,
		HEIGHT = 3,
		AMBIENT = 4
	};
	
	Texture( const char* path, Texture::TYPE type = Texture::TYPE::DIFFUSE, int params = GL_REPEAT );
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
public:
	Texture::TYPE type;
	std::string m_FilePath;
	unsigned int m_RendererID;
private:
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
};
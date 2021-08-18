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
	
	Texture( const char* path, Texture::TYPE type = Texture::TYPE::DIFFUSE );
	~Texture();

	void Bind( unsigned int slot = 0 ) const;
	void Unbind() const;

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
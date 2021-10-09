#pragma once
#include <vector>
#include "Renderer.h"

struct LayoutElement {
	unsigned int glType;				// GL_FLOAT,GL_UNSIGNED_INT
	unsigned int count = 0;				// element count
	unsigned int offset = 0;			// pointer offset
	unsigned int normalized = GL_FALSE;	// GL_FALSE, GL_TRUE

	bool isInstanced = false;
	unsigned int instanceDivisor = 1;
};

class VertexBuffer {
public:
	VertexBuffer( const void* data, unsigned int size );
	~VertexBuffer();
	void Bind() const;
	void Unbind() const;
	void AddLayoutElement( unsigned int glType, unsigned int count, unsigned int normalized = GL_FALSE, bool isInstanced = false, unsigned int divisor = 1 );
	inline std::vector<LayoutElement> GetElements() const { return m_LayoutElements; }
	inline unsigned int GetStride() const { return m_Stride; }
private:
	unsigned int m_RendererId;
	unsigned int m_Stride;
	std::vector<LayoutElement> m_LayoutElements;
};
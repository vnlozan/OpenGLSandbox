#pragma once
#include "Renderer.h"

class UniformBuffer {
private:
	unsigned int m_RendererId;
	unsigned int m_Size;
	unsigned int m_BindingPoint;
public:
	UniformBuffer( unsigned int size, unsigned int bindingPoint )
		: m_Size{ size }, m_BindingPoint{ bindingPoint } {
		GLCall( glGenBuffers( 1, &m_RendererId ) );
		Bind();
		// Allocate memory
		GLCall( glBufferData( GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW ) );
		// Define the range of the buffer that links to a uniform binding point
		GLCall( glBindBufferRange( GL_UNIFORM_BUFFER, bindingPoint, m_RendererId, 0, size ) );
		Unbind();
	}
	~UniformBuffer() {
		//GLCall( glDeleteBuffers( 1, &m_RendererId ) );
	}
	void Bind() const {
		GLCall( glBindBuffer( GL_UNIFORM_BUFFER, m_RendererId ) );
	}
	void BufferSubData( void* ptr, unsigned int size, unsigned int offset = 0 ) const {
		Bind();
		GLCall( glBufferSubData( GL_UNIFORM_BUFFER, offset, size, ptr ) );
		Unbind();
	}
	void Unbind() const {
		GLCall( glBindBuffer( GL_UNIFORM_BUFFER, 0 ) );
	}
};
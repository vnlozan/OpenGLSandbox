#pragma once
#include "Renderer.h"
#include "_Buffer.h"

class _VertexArray {
public:
	_VertexArray(): m_LastAttribPointer{ 0 } {
		GLCall( glGenVertexArrays( 1, &m_RendererId ) );
	}
	~_VertexArray() {
		GLCall( glDeleteVertexArrays( 1, &m_RendererId ) );
	}

	void Bind() const {
		GLCall( glBindVertexArray( m_RendererId ) );
	}
	void Unbind() const {
		GLCall( glBindVertexArray( 0 ) );
	}
	
	void AddBuffer( const _VertexBuffer& vb ) {
		Bind();
		vb.Bind();
		unsigned int offset = 0;
		const auto& elements = vb.GetElements();
		for( const auto& element : elements ) {
			GLCall( glEnableVertexAttribArray( m_LastAttribPointer ) );
			GLCall( glVertexAttribPointer( m_LastAttribPointer, element.count, element.glType, element.normalized, vb.GetStride(), ( void* ) element.offset ) );
			if( element.isInstanced ) {
				GLCall( glVertexAttribDivisor( m_LastAttribPointer, element.instanceDivisor ) );
			}
			m_LastAttribPointer++;
		}
		vb.Unbind();
		Unbind();
	}
private:
	unsigned int m_RendererId;
	unsigned int m_LastAttribPointer;
};
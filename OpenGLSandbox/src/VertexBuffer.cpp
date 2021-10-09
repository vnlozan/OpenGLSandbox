#include "VertexBuffer.h"

VertexBuffer::VertexBuffer( const void* data, unsigned int size ): m_Stride{ 0 } {
	GLCall( glGenBuffers( 1, &m_RendererId ) );
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, m_RendererId ) );
	GLCall( glBufferData( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW ) );
}
VertexBuffer::~VertexBuffer() {
	GLCall( glDeleteBuffers( 1, &m_RendererId ) );
}
void VertexBuffer::Bind() const {
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, m_RendererId ) );
}
void VertexBuffer::Unbind() const {
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
}
void VertexBuffer::AddLayoutElement( unsigned int glType, unsigned int count, unsigned int normalized, bool isInstanced, unsigned int divisor ) {
	unsigned int offset = 0;
	if( !m_LayoutElements.empty() ) {
		offset = m_LayoutElements.back().offset + m_LayoutElements.back().count * sizeof( m_LayoutElements.back().glType );
	}
	m_LayoutElements.push_back( { glType, count, offset, normalized, isInstanced, divisor } );
	m_Stride += count * sizeof( glType );
}
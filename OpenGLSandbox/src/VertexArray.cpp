#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include <iostream>
VertexArray::VertexArray(): m_LastAttribPointer{ 0 } {
	GLCall( glGenVertexArrays( 1, &m_RendererId ) );
}
VertexArray::~VertexArray() {
	GLCall( glDeleteVertexArrays( 1, &m_RendererId ) );
}
void VertexArray::Bind() const {
	GLCall( glBindVertexArray( m_RendererId ) );
}
void VertexArray::Unbind() const {
	GLCall( glBindVertexArray( 0 ) );
}
void VertexArray::AddBuffer( const VertexBuffer& vb ) {
	std::cout << "Start!" << std::endl;

	Bind();
	vb.Bind();
	unsigned int offset = 0;
	const auto& elements = vb.GetElements();
	for( const auto& element : elements ) {
		std::cout << "Attribute : " << m_LastAttribPointer << std::endl;
		GLCall( glEnableVertexAttribArray( m_LastAttribPointer ) );
		GLCall( glVertexAttribPointer( m_LastAttribPointer, element.count, element.glType, element.normalized, vb.GetStride(), ( void* )element.offset ) );
		if( element.isInstanced ) {
			GLCall( glVertexAttribDivisor( m_LastAttribPointer, element.instanceDivisor ) );
		}
		m_LastAttribPointer++;
	}

	std::cout << "End!" << std::endl;
}
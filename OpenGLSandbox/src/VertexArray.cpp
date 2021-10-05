#include "VertexArray.h"
#include "VertexBufferLayout.hpp"
#include "Renderer.h"

VertexArray::VertexArray() {
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

void VertexArray::AddBuffer( const VertexBuffer& vb, const VertexBufferLayout& layout ) {
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++ ) {
		const auto& element = elements[i];
		GLCall( glEnableVertexAttribArray( i ) );
		// (void*)offsetof(Vertex, Normal), offsetof - preprocessor for struct objects
		GLCall( glVertexAttribPointer( i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)(( __int64 )offset) ) );
		offset += element.count * VertexBufferElement::GetSizeOfType( element.type );
	}
}

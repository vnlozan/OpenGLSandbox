#include "Renderer.h"
#include <iostream>
void GLClearError() { while (glGetError() != GL_NO_ERROR); }
bool GLLogCall( const char* function, const char* file, int line ) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] " << error << " " << function << " " << file << " : " << line << std::endl;
		return false;
	}
	return true;
}

Renderer::Renderer() {
	glEnable( GL_DEPTH_TEST );
}

void Renderer::Clear() const {
	GLCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
}

void Renderer::DrawElements( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	// TYPE, ELEMENTS_COUNT, ELEMENTS_COUNT, ELEMENT_TYPE
	// The last argument allows us to specify an offset in the EBO
	// (or pass in an index array, but that is when you're not using element buffer objects), but we're just going to leave this at 0.
	GLCall( glDrawElements( GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr ) );
}

void Renderer::DrawArrays( const VertexArray& va, const unsigned int bufferCount, const Shader& shader ) const {
	shader.Bind();
	va.Bind();
	// TYPE, START_INDEX, INDEX_COUNT from ARRAY
	GLCall( glDrawArrays( GL_TRIANGLES, 0, bufferCount ) );
}

#include <iostream>
#include "Log.h"
#include "Renderer.h"

void GLClearError() {
	while ( glGetError() != GL_NO_ERROR );
}
bool GLLogCall( const char* function, const char* file, int line ) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] " << error << " " << function << " " << file << " : " << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::EnableDepthTest() const {
	GLCall( glEnable( GL_DEPTH_TEST ) );
	//glDepthMask( GL_FALSE );
	//glDepthFunc( GL_LESS ); // Passes if the fragment's depth value is less than the stored depth value
}
void Renderer::EnableStencilTest() const {
	GLCall( glEnable( GL_STENCIL_TEST ) );
	//glStencilMask( 0xFF ); // each bit is written to the stencil buffer as is
	//glStencilMask( 0x00 ); // each bit ends up as 0 in the stencil buffer (disabling writes)
	//glStencilFunc( GL_EQUAL, 1, 0xFF )
}
void Renderer::EnableBlend( int srcp, int dstp ) const {
	GLCall( glEnable( GL_BLEND ) );
	GLCall( glBlendFunc( srcp, dstp ) );
	GLCall( glBlendEquation( GL_FUNC_ADD ) );
}
void Renderer::EnableFaceCull( int cullface, int ordering ) const {
	GLCall( glEnable( GL_CULL_FACE ) );
	GLCall( glCullFace( cullface ) );
	GLCall( glFrontFace( ordering ) );
}
void Renderer::Clear() const {
	GLCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT ) );
}
void Renderer::DrawElements( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	// TYPE, ELEMENTS_COUNT, ELEMENTS_COUNT, ELEMENT_TYPE
	// The last argument allows us to specify an offset in the EBO
	// (or pass in an index array, but that is when you're not using element buffer objects), but we're just going to leave this at 0.
	GLCall( glDrawElements( GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr ) );
	ib.Unbind();
	va.Unbind();
}
void Renderer::DrawArrays( const VertexArray& va, const unsigned int bufferCount, const Shader& shader ) const {
	shader.Bind();
	va.Bind();
	// TYPE, START_INDEX, INDEX_COUNT from ARRAY
	GLCall( glDrawArrays( GL_TRIANGLES, 0, bufferCount ) );
}
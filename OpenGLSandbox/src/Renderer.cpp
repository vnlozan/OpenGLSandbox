#include "Renderer.h"

#include "Log.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"

void GLClearError() {
	while ( glGetError() != GL_NO_ERROR );
}
bool GLLogCall( const char* function, const char* file, int line ) {
	while ( GLenum errorCode = glGetError() ) {
		std::string error;
		switch( errorCode ) {
			case GL_INVALID_ENUM:                  error = "An unacceptable value is specified for an enumerated argument.\r\nThe offending command is ignored and has no other side effect than to set the error flag."; break;
			case GL_INVALID_VALUE:                 error = "A numeric argument is out of range.\r\nThe offending command is ignored and has no other side effect than to set the error flag."; break;
			case GL_INVALID_OPERATION:             error = "The specified operation is not allowed in the current state.\r\nThe offending command is ignored and has no other side effect than to set the error flag."; break;
			case GL_STACK_OVERFLOW:                error = "An attempt has been made to perform an operation that would cause an internal stack to overflow."; break;
			case GL_STACK_UNDERFLOW:               error = "An attempt has been made to perform an operation that would cause an internal stack to underflow."; break;
			case GL_OUT_OF_MEMORY:                 error = "There is not enough memory left to execute the command.\r\nThe state of the GL is undefined, except for the state of the error flags, after this error is recorded."; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "The framebuffer object is not complete.\r\nThe offending command is ignored and has no other side effect than to set the error flag."; break;
		}
		LOG_ERROR( (std::string)"Renderer:: " + error + " " + function + " " + file + " : " + std::to_string( line ) );
		return false;
	}
	return true;
}

void Renderer::SetPolygonMode( int face, int mode ) const {
	// GL_POINT
	// GL_LINE
	// GL_FILL
	GLCall( glPolygonMode( face, mode ) );
}

void Renderer::EnableDepthTest( bool enable, unsigned int comparisonFunc ) const {
	if( !enable ) {
		GLCall( glDisable( GL_DEPTH_TEST ) );
		return;
	}
	GLCall( glEnable( GL_DEPTH_TEST ) );
	glDepthFunc( comparisonFunc ); // Passes if the fragment's depth value is less than the stored depth value
	//glDepthMask( GL_FALSE );
}
void Renderer::EnableMSAA( bool enable ) const {
	if( !enable ) {
		GLCall( glDisable( GL_MULTISAMPLE ) );
		return;
	}
	glEnable( GL_MULTISAMPLE );
}
void Renderer::EnableGammaCorrection( bool enable ) const {
	/* pow(fragColor.rgb, vec3(1.0/gamma)) */

	if( !enable ) {
		GLCall( glDisable( GL_FRAMEBUFFER_SRGB ) );
		return;
	}
	glEnable( GL_FRAMEBUFFER_SRGB );
}
void Renderer::EnablePointSize( bool enable ) const {
	if( !enable ) {
		GLCall( glDisable( GL_PROGRAM_POINT_SIZE ) );
		return;
	}
	GLCall( glEnable( GL_PROGRAM_POINT_SIZE ) );
}
void Renderer::EnableBlend( bool enable, int srcp, int dstp ) const {
	if( !enable ) {
		GLCall( glDisable( GL_BLEND ) );
		return;
	}
	GLCall( glEnable( GL_BLEND ) );
	GLCall( glBlendFunc( srcp, dstp ) );
	GLCall( glBlendEquation( GL_FUNC_ADD ) );
}
void Renderer::EnableFaceCull( bool enable, int cullface, int ordering ) const {
	if( !enable ) {
		GLCall( glDisable( GL_CULL_FACE ) );
		return;
	}
	GLCall( glEnable( GL_CULL_FACE ) );
	GLCall( glCullFace( cullface ) );
	GLCall( glFrontFace( ordering ) );
}

void Renderer::EnableStencilTest( bool enable ) const {
	if( !enable ) {
		GLCall( glDisable( GL_STENCIL_TEST ) );
		return;
	}
	GLCall( glEnable( GL_STENCIL_TEST ) );
	//glStencilMask( 0xFF ); // each bit is written to the stencil buffer as is
	//glStencilMask( 0x00 ); // each bit ends up as 0 in the stencil buffer (disabling writes)
	//glStencilFunc( GL_EQUAL, 1, 0xFF )
}
void Renderer::WriteStencilBuffer( bool enable ) const {
	if( !enable ) {
		glStencilMask( 0x00 );// disable writing to the stencil buffer
		return;
	}
	glStencilMask( 0xFF ); // enable writing to the stencil buffer
}
void Renderer::SetStencilOp( GLenum sfail, GLenum dpfail, GLenum dppass ) const {
	glStencilOp( sfail, dpfail, dppass );
}
void Renderer::SetStencilFunc( GLenum func, GLint ref, GLuint mask ) const {
	glStencilFunc( func, ref, mask );
}

void Renderer::SetViewport( GLint x, GLint y, GLsizei width, GLsizei height ) const {
	GLCall( glViewport( x, y, width, height) );
}
void Renderer::Clear() const {
	GLCall( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT ) );
}
void Renderer::Clear( GLbitfield mask ) const {
	GLCall( glClear( mask ) );
}
void Renderer::ClearColor( float r, float g, float b, float alpha ) const {
	GLCall( glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
}

void Renderer::DrawArrays( const VertexArray& va, const unsigned int bufferCount, const Shader& shader, unsigned int type ) const {
	shader.Bind();
	va.Bind();
	// TYPE, START_INDEX, INDEX_COUNT from ARRAY
	GLCall( glDrawArrays( type, 0, bufferCount ) );
	va.Unbind();
	shader.Unbind();
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
	shader.Unbind();
}
void Renderer::DrawElementsInstanced( const VertexArray& va, const IndexBuffer& ib, const Shader& shader, int instancesCount ) {
	shader.Bind();
	va.Bind();
	ib.Bind();
	glDrawElementsInstanced( GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0, instancesCount );
	ib.Unbind();
	va.Unbind();
	shader.Unbind();
}
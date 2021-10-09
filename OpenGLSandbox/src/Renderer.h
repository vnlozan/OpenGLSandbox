#pragma once
#include <Glad/glad.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT( GLLogCall( #x, __FILE__, __LINE__ ) );

void GLClearError();
bool GLLogCall( const char* function, const char* file, int line );

class VertexArray;
class Shader;
class IndexBuffer;

class Renderer {
public:
	Renderer() {}

	void SetPolygonMode( int face = GL_FRONT_AND_BACK, int mode = GL_FILL ) const;
	void EnableDepthTest( bool enable = true, unsigned int comparisonFunc = GL_LESS ) const;

	void EnableStencilTest( bool enable = true ) const;
	void WriteStencilBuffer( bool enable = true ) const;
	void SetStencilOp( GLenum sfail=GL_KEEP, GLenum dpfail=GL_KEEP, GLenum dppass=GL_KEEP ) const;
	void SetStencilFunc( GLenum func, GLint ref, GLuint mask ) const;

	void EnablePointSize( bool enable = true ) const;
	void EnableBlend( int srcp=GL_SRC_ALPHA, int dstp=GL_ONE_MINUS_SRC_ALPHA ) const;
	void EnableFaceCull( bool enable=true, int cullface=GL_BACK, int ordering=GL_CCW ) const;
	void ClearColor( float r = 0.0f, float g = 0.0f, float b = 0.0f, float alpha = 1.0f ) const;
	void Clear() const;

	void DrawElements( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const;
	void DrawElementsInstanced( const VertexArray& va, const IndexBuffer& ib, const Shader& shader, int instancesCount );
	void DrawArrays( const VertexArray& va, const unsigned int bufferCount, const Shader& shader, unsigned int type = GL_TRIANGLES ) const;
	void DrawArraysInstanced( const VertexArray& va, const Shader& shader, unsigned int type = GL_TRIANGLES, int first = 0, int count = 0, int instancesCount = 2 ) const;
};
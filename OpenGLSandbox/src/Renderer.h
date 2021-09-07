#pragma once
#include <Glad/glad.h>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT( GLLogCall( #x, __FILE__, __LINE__ ) );

void GLClearError();
bool GLLogCall( const char* function, const char* file, int line );

class Renderer {
public:
	Renderer() {}
	void EnableDepthTest() const;
	void EnableStencilTest() const;
	void EnableBlend( int srcp=GL_SRC_ALPHA, int dstp=GL_ONE_MINUS_SRC_ALPHA ) const;
	void EnableFaceCull( int cullface=GL_BACK, int ordering=GL_CCW ) const;
	void Clear() const;
	void DrawElements( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const;
	void DrawArrays( const VertexArray& va, const unsigned int bufferCount, const Shader& shader ) const;
private:
	//bool blendEnabled;
	//bool depthEnabled;
	//bool stencilEnabled;
};
#pragma once
#include <GL/glew.h>
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
	Renderer();
	void Clear() const;
	void DrawElements( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const;
	void DrawArrays( const VertexArray& va, const unsigned int bufferCount, const Shader& shader ) const;
private:

};
#pragma once
#include <GL/glew.h>
#include <functional>
#include "Renderer.h"
namespace tests {
	class Test {
	public:
		Test( GLuint width, GLuint height ) : m_Width{ width }, m_Height{ height }  {}
		virtual ~Test() {}
		virtual void OnStart() {}
		virtual void OnUpdate( float deltaTime ) {}
		virtual void OnRender( Renderer& renderer ) {}
		virtual void OnImGuiRender() {}
	public:
		GLuint m_Width;
		GLuint m_Height;
	};
}
#pragma once
#include <functional>
#include "Renderer.h"

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"

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
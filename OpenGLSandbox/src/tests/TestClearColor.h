#pragma once
#include "Test.h"
#include <imgui_1.79/imgui.h>

namespace tests {
	class TestClearColor : public tests::Test {
	public:
		TestClearColor( GLuint width, GLuint height ) : Test{ width, height }, m_Color { 0.2f, 0.3f, 0.8f, 1.0f } {}
		~TestClearColor() {}
		void OnUpdate( float deltatime ) override {}
		void OnRender( Renderer& renderer ) override {
			renderer.Clear();
			GLCall( glClearColor( m_Color[0], m_Color[1], m_Color[2], m_Color[3] ) );
			GLCall( glClear( GL_COLOR_BUFFER_BIT ) );
		}
		void OnImGuiRender() override {
			{
				ImGui::Begin( "Settings" );
				ImGui::ColorEdit4( "Clear Color", m_Color );
				ImGui::End();
			}
		}
	private:
		float m_Color[4];
	};
}
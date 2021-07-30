#pragma once
#include <functional>
#include <vector>
#include <string>
#include "Test.h"

namespace tests {
	class TestMenu : public Test {
	public:
		TestMenu( GLuint width, GLuint height, GLFWwindow*& window ): Test{ width, height }, m_CurrentTest{ nullptr }, window{ window } {}
		~TestMenu() {
			delete m_CurrentTest;
		}
		template<typename T>
		void RegisterTest( const std::string& name ) {
			std::cout << "Registering test " << name << std::endl;
			m_Tests.push_back( std::make_pair( name, [ this ]() {
				T* t = new T( m_Width, m_Height, window );
				t->OnStart();
				return t; 
			} ) );
		}
		void StartImGui() {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); ( void )io;
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL( window, true );
			ImGui_ImplOpenGL3_Init( ( char* )glGetString( GL_NUM_SHADING_LANGUAGE_VERSIONS ) );
		}
		void ShutDownImGui() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		void OnUpdate( float deltaTime ) override {
			if (m_CurrentTest) {
				m_CurrentTest->OnUpdate( deltaTime );
			}
		}
		void OnRender( Renderer& renderer ) override {
			if (m_CurrentTest) {
				m_CurrentTest->OnRender( renderer );
			}
		}
		void OnImGuiRender() override {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin( "Menu" );
			if ( m_CurrentTest ) {
				if ( ImGui::Button( "[<<]" ) ) {
					delete m_CurrentTest;
					m_CurrentTest = nullptr;
				}

			} else {
				for (auto& test : m_Tests) {
					if (ImGui::Button( test.first.c_str() )) {
						m_CurrentTest = test.second();
					}
				}
			}
			if (m_CurrentTest) {
				m_CurrentTest->OnImGuiRender();
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
		}
	private:
		Test* m_CurrentTest;
		std::vector < std::pair<std::string, std::function<Test* ( )>>> m_Tests;
		GLFWwindow* window;
	};
}
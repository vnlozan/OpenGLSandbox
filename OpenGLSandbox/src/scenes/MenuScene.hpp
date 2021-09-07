#include "Scene.hpp"

namespace Scenes {

	class MenuScene: public Scene {
	public:
		MenuScene( GLuint width, GLuint height, GLFWwindow* window,
			Scene** currentScene, std::vector < std::pair<std::string, std::function<Scene* ( )>>>* scenes
		) : Scene{ width, height, window }, m_CurrentScene{ currentScene }, m_Scenes{ scenes } {}
		virtual ~MenuScene() override {	}
		virtual void OnStart( Renderer& renderer ) override {
			glfwSetWindowUserPointer( m_Window, NULL );
			glfwSetCursorPosCallback( m_Window, NULL );
			glfwSetScrollCallback( m_Window, NULL );
			glfwSetKeyCallback( m_Window, NULL );
		}
		virtual void OnImGuiRender() override {
			ImGui::Begin( "Menu" );
			for( auto& test : *m_Scenes ) {
				if( ImGui::Button( test.first.c_str() ) ) {

					*m_CurrentScene = test.second();
					delete this;
				}
			}
			if( ImGui::Button( "Exit" ) ) {
				glfwSetWindowShouldClose( m_Window, GL_TRUE );
			}
			ImGui::End();
		}
	private:
		std::vector < std::pair<std::string, std::function<Scene* ( )>>>* m_Scenes;
		Scene** m_CurrentScene;
	};
}
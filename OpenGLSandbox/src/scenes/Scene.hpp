#pragma once
#include <vector>
#include <map>
#include <queue>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"

#include "Renderer.h"
#include "Camera.h"

namespace Scenes {

	struct KeyEvent {
		KeyEvent( int key, int code, int action, int modifiers ): key{ key }, code{ code }, action{ action }, modifiers{ modifiers } {}
		KeyEvent( const KeyEvent& o ): key{ o.key }, code{ o.code }, action{ o.action }, modifiers{ o.modifiers } {}
		//KeyEvent( KeyEvent&& o ) noexcept: key( std::exchange( o.key, 0 ) ), code( std::exchange( o.code, 0 ) ), action( std::exchange( o.action, 0 ) ), modifiers( std::exchange( o.modifiers, 0 ) ) {}
		int key;
		int code;
		int action;
		int modifiers;
	};

	class Scene {
	public:
		Scene( GLuint width, GLuint height, GLFWwindow* window )
			: m_Width{ width }, m_Height{ height }, m_Window{ window }, m_FirstMouse{ true } {

			m_LastX = m_Width / 2.0f;
			m_LastY = m_Height / 2.0f;
		}
		virtual ~Scene() {
			m_KeyFunctions.clear();
		}
		/* Is called by Application object. All initialization should be here. */
		virtual void OnStart( Renderer& renderer ) {
			m_Camera.Position = glm::vec3( 0.0f, 0.0f, 5.0f );
			glfwSetWindowUserPointer( m_Window, ( void* ) this );
			glfwSetCursorPosCallback( m_Window, [] ( GLFWwindow* window, double xpos, double ypos ) {
				Scene* scene = ( Scene* ) glfwGetWindowUserPointer( window );
				if( scene->m_ControlsEnabled ) {
					if( scene->m_FirstMouse ) {
						scene->m_LastX = xpos;
						scene->m_LastY = ypos;
						scene->m_FirstMouse = false;
					}
					float xoffset = xpos - scene->m_LastX;
					float yoffset = scene->m_LastY - ypos; // reversed since y-coordinates go from bottom to top
					scene->m_LastX = xpos;
					scene->m_LastY = ypos;
					scene->m_Camera.ProcessMouseMovement( xoffset, yoffset );
				}
			} );
			glfwSetScrollCallback( m_Window, [] ( GLFWwindow* window, double xoffset, double yoffset ) {
				Scene* scene = ( Scene* ) glfwGetWindowUserPointer( window );
				scene->m_Camera.ProcessMouseScroll( yoffset );
			} );
			glfwSetKeyCallback( m_Window, [] ( GLFWwindow* window, int key, int scancode, int action, int modes ) {
				Scene* scene = ( Scene* ) glfwGetWindowUserPointer( window );
				scene->m_UnhandledKeys.emplace( key, scancode, action, modes );
			} );
		}
		/* Is called by Application object. All the physics should be here. */
		virtual void OnUpdate( float deltaTime ) {
			ProcessControls( deltaTime );
		}
		/* Is called by Application object. All the rendering should be here. */
		virtual void OnRender( Renderer& renderer ) {	}
		/* Is called by Application object. All the ImGui components should be here. */
		virtual void OnImGuiRender() {
			ImGui::Begin( "Menu" );
			if( ImGui::Button( "Back" ) ) {
				this->m_menuBackFunc();
			}
			ImGui::End();
		}
		virtual void ProcessControls( float deltaTime ) {
			while( !m_UnhandledKeys.empty() ) {
				KeyEvent event = m_UnhandledKeys.front();
				m_UnhandledKeys.pop();
				m_Keys[event.key] = event.action == GLFW_PRESS || event.action == GLFW_REPEAT;
				if( m_Keys[event.key] && m_KeyFunctions.find( event.key ) != m_KeyFunctions.end() ) {
					m_KeyFunctions[event.key]();
				}
			}
			if( m_ControlsEnabled ) {
				if( m_Keys[GLFW_KEY_W] ) {
					if( m_Keys[GLFW_KEY_LEFT_SHIFT] ) {
						m_Camera.ProcessKeyboard( Camera::Movement::FORWARD, 2 * deltaTime );
					} else {
						m_Camera.ProcessKeyboard( Camera::Movement::FORWARD, deltaTime );
					}
				}
				if( m_Keys[GLFW_KEY_A] ) {
					if( m_Keys[GLFW_KEY_LEFT_SHIFT] ) {
						m_Camera.ProcessKeyboard( Camera::Movement::LEFT, 2 * deltaTime );
					} else {
						m_Camera.ProcessKeyboard( Camera::Movement::LEFT, deltaTime );
					}
				}
				if( m_Keys[GLFW_KEY_S] ) {
					if( m_Keys[GLFW_KEY_LEFT_SHIFT] ) {
						m_Camera.ProcessKeyboard( Camera::Movement::BACKWARD, 2 * deltaTime );
					} else {
						m_Camera.ProcessKeyboard( Camera::Movement::BACKWARD, deltaTime );
					}
				}
				if( m_Keys[GLFW_KEY_D] ) {
					if( m_Keys[GLFW_KEY_LEFT_SHIFT] ) {
						m_Camera.ProcessKeyboard( Camera::Movement::RIGHT, 2 * deltaTime );
					} else {
						m_Camera.ProcessKeyboard( Camera::Movement::RIGHT, deltaTime );
					}
				}
			}
		}
		void SetMenuBackFunction( std::function<void( void )> menuBackFunc ) {
			this->m_menuBackFunc = menuBackFunc;
		}
	protected:
	private:
	public:
		bool m_ControlsEnabled = false;

		float m_LastX;
		float m_LastY;
		bool m_FirstMouse;
		Camera m_Camera;

		GLFWwindow* m_Window;

		GLuint m_Width;
		GLuint m_Height;

		std::function<void()> m_menuBackFunc;
		std::queue<KeyEvent> m_UnhandledKeys;
		std::map<int, std::function<void()>> m_KeyFunctions;
		std::map<int, bool> m_Keys;
	};
}
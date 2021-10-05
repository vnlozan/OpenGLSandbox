#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"
#include "_VertexArray.h"
#include "_VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/glm/gtc/type_ptr.hpp>

namespace Scenes {
	class InstancingScene: public Scene {
	public:
		InstancingScene( GLuint width, GLuint height, GLFWwindow* window ): Scene{ width, height, window }, m_InstanceType{ 0 } {
			int index = 0;
			float offset = 0.1f;
			for( int y = -10; y < 10; y += 2 ) {
				for( int x = -10; x < 10; x += 2 ) {
					glm::vec2 translation;
					translation.x = ( float ) x / 10.0f + offset;
					translation.y = ( float ) y / 10.0f + offset;
					m_Translations[index++] = translation;
				}
			}
		}
		virtual ~InstancingScene() override {}
		virtual void OnStart( Renderer& renderer ) override {
			Scene::OnStart( renderer );

			glfwSetInputMode( m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
			m_KeyFunctions[GLFW_KEY_SPACE] = [this] {
				m_ControlsEnabled = !m_ControlsEnabled;
				if( m_ControlsEnabled ) {
					glfwSetInputMode( m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
				} else {
					m_FirstMouse = true;
					m_LastX = m_Width / 2.0f;
					m_LastY = m_Height / 2.0f;
					glfwSetInputMode( m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
				}
			};

			float quadVertices[] = {
				// positions		// colors
				-0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
				0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
				-0.05f, -0.05f, 0.0f, 0.0f, 1.0f,

				-0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
				0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
				0.05f, 0.05f, 0.0f, 1.0f, 1.0f
			};

			m_VBO_PosColor = std::make_unique<_VertexBuffer>( quadVertices, sizeof( quadVertices ) );
			m_VBO_PosColor->AddLayoutElement( GL_FLOAT, 2 ); // add position
			m_VBO_PosColor->AddLayoutElement( GL_FLOAT, 3 ); // add color

			ConfigureSettings();
		}
		virtual void OnImGuiRender() override {
			Scene::OnImGuiRender();
			{
				ImGui::Begin( "Info" );
				ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
				ImGui::End();
			}
			{
				ImGui::Begin( "Settings" );
				ImGui::Text( "Instancing" );
				int state = m_InstanceType;
				ImGui::RadioButton( "Using uniform buffer", &m_InstanceType, 0 ); ImGui::SameLine();
				ImGui::RadioButton( "Using array buffer", &m_InstanceType, 1 ); ImGui::SameLine();
				if( state != m_InstanceType ) {
					ConfigureSettings();
				}
				ImGui::End();
			}
		}
		virtual void OnRender( Renderer& renderer ) override {
			renderer.DrawArraysInstanced( *m_VAO, *m_Shader, GL_TRIANGLES, 0, 6, 100 );
		}
	private:
		void ConfigureSettings() {
			if( m_InstanceType == 0 ) {
				m_VAO = std::make_unique<_VertexArray>();
				m_VAO->AddBuffer( *m_VBO_PosColor );
				
				m_Shader = std::make_unique<Shader>( "res/shaders/InstancingBasic.shader" );
				m_Shader->Bind();
				for( unsigned int i = 0; i < 100; i++ ) {
					m_Shader->SetUniform2f( ( "u_Offsets[" + std::to_string( i ) + "]" ), m_Translations[i] );
				}
				return;
			}
			m_VAO = std::make_unique<_VertexArray>();
			m_VAO->AddBuffer( *m_VBO_PosColor );
			m_VBO_Offset = std::make_unique<_VertexBuffer>( m_Translations, sizeof( m_Translations ) );
			m_VBO_Offset->AddLayoutElement( GL_FLOAT, 2, GL_FALSE, true, 1 ); // add offset
			m_VAO->AddBuffer( *m_VBO_Offset );

			m_Shader = std::make_unique<Shader>( "res/shaders/Instancing.shader" );
			m_Shader->Bind();
		}
	private:
		glm::vec2 m_Translations[100];
		std::unique_ptr<_VertexBuffer> m_VBO_PosColor;
		std::unique_ptr<_VertexBuffer> m_VBO_Offset;
		std::unique_ptr<_VertexArray> m_VAO;
		std::unique_ptr<Shader> m_Shader;
		int m_InstanceType;
	};
}
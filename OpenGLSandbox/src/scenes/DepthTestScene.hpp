#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "scenes/Scene.hpp"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Scenes {
	class DepthTestScene: public Scene {
	public:
		DepthTestScene( GLuint width, GLuint height, GLFWwindow* window ) : Scene{ width, height, window } {}
		virtual ~DepthTestScene() override {}
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
			
			float cubeVertices[] = {
				// positions          // texture Coords
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
				-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

				-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
				-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
				-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

				0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
				0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

				-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
				0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
				-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
			};
			float planeVertices[] = {
				// positions       // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
				5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
				-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
				-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

				5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
				-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
				5.0f, -0.5f, -5.0f, 2.0f, 2.0f
			};

			m_CubeVBO = std::make_unique<VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
			m_CubeVBO->AddLayoutElement( GL_FLOAT, 3 );
			m_CubeVBO->AddLayoutElement( GL_FLOAT, 2 );
			m_CubeVAO = std::make_unique<VertexArray>();
			m_CubeVAO->AddBuffer( *m_CubeVBO );

			m_PlaneVBO = std::make_unique<VertexBuffer>( planeVertices, sizeof( planeVertices ) );
			m_PlaneVBO->AddLayoutElement( GL_FLOAT, 3 );
			m_PlaneVBO->AddLayoutElement( GL_FLOAT, 2 );
			m_PlaneVAO = std::make_unique<VertexArray>();
			m_PlaneVAO->AddBuffer( *m_PlaneVBO );
			
			m_Shader = std::make_unique<Shader>( "res/shaders/Depth.shader" );
			m_Shader->Bind();
		}
		virtual void OnImGuiRender() override {
			Scene::OnImGuiRender();
			{
				ImGui::Begin( "Info" );
				ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
				ImGui::End();
			}
		}
		virtual void OnRender( Renderer& renderer ) override {
			m_Shader->Bind();

			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();

			glm::mat4 model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			glm::mat4 mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_CubeVAO, 36, *m_Shader );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_CubeVAO, 36, *m_Shader );

			model = glm::mat4( 1.0f );
			mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_PlaneVAO, 36, *m_Shader );
		}
	private:
		std::unique_ptr<VertexArray> m_CubeVAO;
		std::unique_ptr<VertexArray> m_PlaneVAO;

		std::unique_ptr<VertexBuffer> m_CubeVBO;
		std::unique_ptr<VertexBuffer> m_PlaneVBO;

		std::unique_ptr<Shader> m_Shader;
	};
}
#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "scenes/Scene.hpp"
#include "Model.h"

namespace Scenes {

	class ModelScene: public Scene {
	public:
		ModelScene( GLuint width, GLuint height, GLFWwindow* window ) : Scene{ width, height, window } {}
		virtual ~ModelScene() override {}
		virtual void OnStart( Renderer& renderer ) override {
			Scene::OnStart( renderer );
			renderer.EnableFaceCull();
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
			m_Model = std::make_unique<Model>( "res/models/backpack/backpack.obj" );

			//m_ShaderVertexNormal = std::make_unique<Shader>( "res/shaders/VertexNormal.shader" );
			//m_ShaderPrimitiveNormal = std::make_unique<Shader>( "res/shaders/PrimitiveNormal.shader" );
			m_Shader = std::make_unique<Shader>( "res/shaders/Model.shader" );
			//m_Shader = std::make_unique<Shader>( "res/shaders/Explosion_g.shader" );

		}
		virtual void OnImGuiRender() override {
			Scene::OnImGuiRender();
			{
				ImGui::Begin( "Info" );
				ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
				ImGui::Text( "Camera Position : %.3f / %.3f / %.3f", m_Camera.Position.r, m_Camera.Position.g, m_Camera.Position.b );
				ImGui::End();
			}
		}
		virtual void OnRender( Renderer& renderer ) override {
			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();
			glm::mat4 model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 0.0f, 0.0f, 0.0f ) ); // translate it down so it's at the center of the scene
			model = glm::scale( model, glm::vec3( 1.0f, 1.0f, 1.0f ) );	// it's a bit too big for our scene, so scale it down
			glm::mat4 mvp = projection * view * model;

			m_Shader->Bind();
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			//m_Shader->SetUniform1f( "u_Time", glfwGetTime() );
			m_Model->Draw( renderer, *m_Shader );

			//m_ShaderVertexNormal->Bind();
			//m_ShaderVertexNormal->SetUniformMat4f( "u_MV", view * model );
			//m_ShaderVertexNormal->SetUniformMat4f( "u_Projection", projection );
			//m_Model->Draw( renderer, *m_ShaderVertexNormal );
			//m_ShaderPrimitiveNormal->Bind();
			//m_ShaderPrimitiveNormal->SetUniformMat4f( "u_MVP", mvp );
			//m_Model->Draw( renderer, *m_ShaderPrimitiveNormal );
		}
		void SetWireframeMode( bool enable = true ) {
			if( enable ) {
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				return;
			}
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	private:
		std::unique_ptr<Model> m_Model;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderVertexNormal;
		std::unique_ptr<Shader> m_ShaderTriangleNormal;
	};
}
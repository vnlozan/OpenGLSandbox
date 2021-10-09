#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "scenes/Scene.hpp"
#include "Model.h"

namespace Scenes {

	class ModelScene: public Scene {
	public:
		ModelScene( GLuint width, GLuint height, GLFWwindow* window )
			: Scene{ width, height, window }, m_DrawVerticesNormals{ false }, m_DrawTriangleNormals{ false }, m_WireframeModeEnabled{ false } {}
		virtual ~ModelScene() override {}
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
			m_Model = std::make_unique<Model>( "res/models/backpack/backpack.obj" );

			m_ShaderVertexNormal = std::make_unique<Shader>( "res/shaders/VertexNormal.shader" );
			m_ShaderTriangleNormal = std::make_unique<Shader>( "res/shaders/TriangleNormal.shader" );
			m_ShaderTexture = std::make_unique<Shader>( "res/shaders/Model.shader" );

			renderer.EnableFaceCull();
		}
		virtual void OnImGuiRender() override {
			Scene::OnImGuiRender();
			{
				ImGui::Begin( "Info" );
				ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
				ImGui::Text( "Camera Position : %.3f / %.3f / %.3f", m_Camera.Position.r, m_Camera.Position.g, m_Camera.Position.b );
				ImGui::End();
			}
			{
				ImGui::Begin( "Settings" );
				ImGui::Text( "Rendering" );
				bool mode = m_WireframeModeEnabled;
				ImGui::Checkbox( "Wireframe mode", &m_WireframeModeEnabled );
				if( mode != m_WireframeModeEnabled ) {
					if( m_WireframeModeEnabled ) {
						glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
					} else {
						glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					}
				}
				ImGui::Checkbox( "Draw primitives normals", &m_DrawTriangleNormals ); //ImGui::SameLine();
				ImGui::Checkbox( "Draw vertices normals", &m_DrawVerticesNormals ); //ImGui::SameLine();
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

			m_ShaderTexture->Bind();
			m_ShaderTexture->SetUniformMat4f( "u_MVP", mvp );
			m_Model->Draw( renderer, *m_ShaderTexture );

			if( m_DrawTriangleNormals ) {
				m_ShaderTriangleNormal->Bind();
				m_ShaderTriangleNormal->SetUniformMat4f( "u_Model", model );
				m_ShaderTriangleNormal->SetUniformMat4f( "u_View", view );
				m_ShaderTriangleNormal->SetUniformMat4f( "u_Projection", projection );
				m_Model->Draw( renderer, *m_ShaderTriangleNormal );
			}
			if( m_DrawVerticesNormals ) {
				m_ShaderVertexNormal->Bind();
				m_ShaderVertexNormal->SetUniformMat4f( "u_MV", view * model );
				m_ShaderVertexNormal->SetUniformMat4f( "u_Projection", projection );
				m_Model->Draw( renderer, *m_ShaderVertexNormal );
			}
		}
	private:
		std::unique_ptr<Model> m_Model;

		std::unique_ptr<Shader> m_ShaderTexture;
		std::unique_ptr<Shader> m_ShaderVertexNormal;
		std::unique_ptr<Shader> m_ShaderTriangleNormal;

		bool m_DrawTriangleNormals;
		bool m_DrawVerticesNormals;
		bool m_WireframeModeEnabled;
	};
}
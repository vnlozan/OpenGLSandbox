#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"

#include "Shader.h"

#include "_VertexArray.h"
#include "_Texture.h"
#include "_Buffer.h"

namespace Scenes {
	class BlinnPhongBasic: public Scene {
	public:
		BlinnPhongBasic( GLuint width, GLuint height, GLFWwindow* window )
			: Scene{ width, height, window }, m_LightPos{ glm::vec3( 1.2f, 1.0f, 2.0f ) }, m_BlinnEnabled{ false } {}
		virtual ~BlinnPhongBasic() override {}
		virtual void OnStart( Renderer& renderer ) override {
			Scene::OnStart( renderer );

			// tell GLFW to capture our mouse
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
			float planeVertices[] = {
				// positions            // normals			// texcoords
				10.0f, -0.5f, 10.0f,	0.0f, 1.0f, 0.0f,	10.0f, 0.0f,
				-10.0f, -0.5f, 10.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
				-10.0f, -0.5f, -10.0f,	0.0f, 1.0f, 0.0f,	0.0f, 10.0f,

				10.0f, -0.5f, 10.0f,	0.0f, 1.0f, 0.0f,	10.0f, 0.0f,
				-10.0f, -0.5f, -10.0f,	0.0f, 1.0f, 0.0f,	0.0f, 10.0f,
				10.0f, -0.5f, -10.0f,	0.0f, 1.0f, 0.0f,	10.0f, 10.0f
			};
			
			m_VAO = std::make_unique<_VertexArray>();
			m_VBO = std::make_unique<_VertexBuffer>( planeVertices, sizeof( planeVertices ) );
			m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // normals
			m_VBO->AddLayoutElement( GL_FLOAT, 2 ); // tex coords
			m_VAO->AddBuffer( *m_VBO );

			m_TextureWood = std::make_unique<_Texture2D>( "res/textures/wood.png", _Texture2D::TYPE::DIFFUSE, GL_REPEAT );
			//m_TextureWood->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			//m_TextureWood->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			//m_TextureWood->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			//m_TextureWood->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			m_TextureWood->ActivateTexture( 0 );
			m_TextureWood->Bind();

			m_Shader = std::make_unique<Shader>( "res/shaders/BlinnPhong.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Blinn", m_BlinnEnabled );
			m_Shader->SetUniform1i( "u_Texture", 0 );
			m_Shader->SetUniform3f( "u_LightPos", glm::vec3( 0.0f, 0.0f, 0.0f ) );
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
				int state = m_BlinnEnabled;
				ImGui::RadioButton( "Phong", &m_BlinnEnabled, 0 ); ImGui::SameLine();
				ImGui::RadioButton( "Blinn-Phong", &m_BlinnEnabled, 1 ); ImGui::SameLine();
				if( state != m_BlinnEnabled ) {
					m_Shader->Bind();
					m_Shader->SetUniform1i( "u_Blinn", m_BlinnEnabled );
				}
				ImGui::End();
			}
		}
		virtual void OnRender( Renderer& renderer ) override {
			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();

			m_Shader->Bind();
			m_Shader->SetUniformMat4f( "u_VP", projection * view );
			m_Shader->SetUniform3f( "u_ViewPos", m_Camera.Position );
			renderer.DrawArrays( *m_VAO, 36, *m_Shader );
		}
	private:
		std::unique_ptr<_Texture2D> m_TextureWood;

		std::unique_ptr<_VertexBuffer> m_VBO;
		std::unique_ptr<_VertexArray> m_VAO;
		
		std::unique_ptr<Shader> m_Shader;
		
		glm::vec3 m_LightPos;
		
		int m_BlinnEnabled = 0;
	};
}
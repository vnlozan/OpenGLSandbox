#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"

#include "Shader.h"

#include "_VertexArray.h"
#include "_Buffer.h"
#include "_Texture.h"

namespace Scenes {

	class MultipleLightScene: public Scene {
	public:
		MultipleLightScene( GLuint width, GLuint height, GLFWwindow* window )
			: Scene{ width, height, window }, m_LightPos{ glm::vec3( 1.2f, 1.0f, 2.0f ) }, m_BlinnEnabled{ false } {}
		virtual ~MultipleLightScene() override {}
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
			
			float vertices[] = {
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
				0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// triangle 1 vertices + normals + texture coords
				0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
				-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// triangle 2 vertices + normals + texture coords

				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

				-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

				-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
				0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
			};

			m_CubePositions.emplace_back( glm::vec3( 0.0f, 0.0f, 0.0f ) );
			m_CubePositions.emplace_back( glm::vec3( 2.0f, 5.0f, -15.0f ) );
			m_CubePositions.emplace_back( glm::vec3( -1.5f, -2.2f, -2.5f ) );
			m_CubePositions.emplace_back( glm::vec3( -3.8f, -2.0f, -12.3f ) );
			m_CubePositions.emplace_back( glm::vec3( 2.4f, -0.4f, -3.5f ) );
			m_CubePositions.emplace_back( glm::vec3( -1.7f, 3.0f, -7.5f ) );
			m_CubePositions.emplace_back( glm::vec3( 1.3f, -2.0f, -2.5f ) );
			m_CubePositions.emplace_back( glm::vec3( 1.5f, 2.0f, -2.5f ) );
			m_CubePositions.emplace_back( glm::vec3( 1.5f, 0.2f, -1.5f ) );
			m_CubePositions.emplace_back( glm::vec3( -1.3f, 1.0f, -1.5f ) );

			m_PointLightPositions.emplace_back( glm::vec3( 0.7f, 0.2f, 2.0f ) );
			m_PointLightPositions.emplace_back( glm::vec3( 2.3f, -3.3f, -4.0f ) );
			m_PointLightPositions.emplace_back( glm::vec3( -4.0f, 2.0f, -12.0f ) );
			m_PointLightPositions.emplace_back( glm::vec3( 0.0f, 0.0f, -3.0f ) );

			m_VAO = std::make_unique<_VertexArray>();

			m_VBO = std::make_unique<_VertexBuffer>( vertices, sizeof( vertices ) );
			m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // normals
			m_VBO->AddLayoutElement( GL_FLOAT, 2 ); // tex coords

			m_VAO->AddBuffer( *m_VBO );

			m_TextureDiffuse = std::make_unique<_Texture2D>( "res/textures/container_diffuse.png", _Texture2D::TYPE::DIFFUSE, true );
			m_TextureSpecular = std::make_unique<_Texture2D>( "res/textures/container_specular.png", _Texture2D::TYPE::SPECULAR, true );

			m_Shader = std::make_unique<Shader>( "res/shaders/MultipleLight.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Blinn", m_BlinnEnabled );
			m_Shader->SetUniform1i( "u_Material.diffuse", 0 );
			m_Shader->SetUniform1i( "u_Material.specular", 1 );

			m_VAOLight = std::make_unique<_VertexArray>();
			m_VAOLight->AddBuffer( *m_VBO );

			m_ShaderLight = std::make_unique<Shader>( "res/shaders/Color.shader" );
			m_ShaderLight->Bind();
			m_ShaderLight->SetUniform3f( "u_Color", 1.0f, 0.0f, 0.0f );
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

			m_TextureDiffuse->ActivateTexture( 0 );
			m_TextureDiffuse->Bind();
			m_TextureSpecular->ActivateTexture( 1 );
			m_TextureSpecular->Bind();

			m_Shader->SetUniform3f( "u_ViewPos", m_Camera.Position );
			m_Shader->SetUniform1f( "u_Material.shininess", 32.0f );
			
			// Direction light
			m_Shader->SetUniform3f( "u_DirectionLight.direction", -0.2f, -1.0f, -0.3f );
			m_Shader->SetUniform3f( "u_DirectionLight.ambient", 0.0f, 0.05f, 0.0f );
			m_Shader->SetUniform3f( "u_DirectionLight.diffuse", 0.0f, 1.0f, 0.0f );
			m_Shader->SetUniform3f( "u_DirectionLight.specular", 0.0f, 0.5f, 0.0f );

			// Point light
			for( size_t i = 0; i < m_PointLightPositions.size(); i++ ) {
				std::string tmp = std::to_string( i );
				m_Shader->SetUniform3f( "u_PointLights[" + tmp + "].position", m_PointLightPositions[i] );
				m_Shader->SetUniform3f( "u_PointLights[" + tmp + "].ambient", 0.02f, 0.0f, 0.0f );
				m_Shader->SetUniform3f( "u_PointLights[" + tmp + "].diffuse", 1.0f, 0.0f, 0.0f );
				m_Shader->SetUniform3f( "u_PointLights[" + tmp + "].specular", 0.5f, 0.0f, 0.0f );
				m_Shader->SetUniform1f( "u_PointLights[" + tmp + "].constant", 1.0f );
				m_Shader->SetUniform1f( "u_PointLights[" + tmp + "].linear", 0.09 );
				m_Shader->SetUniform1f( "u_PointLights[" + tmp + "].quadratic", 0.032 );
			}
			// Spot light
			m_Shader->SetUniform3f( "u_SpotLight.position", m_Camera.Position );
			m_Shader->SetUniform3f( "u_SpotLight.direction", m_Camera.Front );
			m_Shader->SetUniform3f( "u_SpotLight.ambient", 0.0f, 0.0f, 0.0f );
			m_Shader->SetUniform3f( "u_SpotLight.diffuse", 1.0f, 1.0f, 1.0f );
			m_Shader->SetUniform3f( "u_SpotLight.specular", 1.0f, 1.0f, 1.0f );
			m_Shader->SetUniform1f( "u_SpotLight.constant", 1.0f );
			m_Shader->SetUniform1f( "u_SpotLight.linear", 0.09 );
			m_Shader->SetUniform1f( "u_SpotLight.quadratic", 0.032 );
			m_Shader->SetUniform1f( "u_SpotLight.cutOff", glm::cos( glm::radians( 12.5f ) ) );
			m_Shader->SetUniform1f( "u_SpotLight.outerCutOff", glm::cos( glm::radians( 15.0f ) ) );

			// Draw call for all the lightened cubes
			for( size_t i = 0; i < m_CubePositions.size(); i++ ) {

				glm::mat4 model = glm::mat4( 1.0f );
				model = glm::translate( model, m_CubePositions[i] );

				float angle = 20.0f * i;
				model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 0.3f, 0.5f ) );
				glm::mat4 mvp = projection * view * model;

				m_Shader->SetUniformMat4f( "u_Model", model );
				m_Shader->SetUniformMat4f( "u_MVP", mvp );
				renderer.DrawArrays( *m_VAO, 36, *m_Shader );
			}

			// Draw call for all the light sources
			m_ShaderLight->Bind();
			for( size_t i = 0; i < m_PointLightPositions.size(); i++ ) {
				glm::mat4 model = glm::mat4( 1.0f );
				model = glm::translate( model, m_PointLightPositions[i] );
				model = glm::scale( model, glm::vec3( 0.2f ) ); // Make it a smaller cube
				glm::mat4 mvp = projection * view * model; // THE RIGHT WAY pvm
				m_ShaderLight->SetUniformMat4f( "u_MVP", mvp );
				glDrawArrays( GL_TRIANGLES, 0, 36 );
				renderer.DrawArrays( *m_VAOLight, 36, *m_ShaderLight );
			}
		}
	private:
		std::unique_ptr<_VertexBuffer> m_VBO;

		std::unique_ptr<_VertexArray> m_VAOLight;
		std::unique_ptr<_VertexArray> m_VAO;
		
		std::unique_ptr<_Texture2D> m_TextureDiffuse;
		std::unique_ptr<_Texture2D> m_TextureSpecular;

		std::unique_ptr<Shader> m_ShaderLight;
		std::unique_ptr<Shader> m_Shader;

		glm::vec3 m_LightPos;

		std::vector<glm::vec3> m_CubePositions;
		std::vector<glm::vec3> m_PointLightPositions;

		int m_BlinnEnabled = 0;
	};
}
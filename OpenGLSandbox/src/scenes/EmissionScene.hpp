#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm/gtc/type_ptr.hpp>
#include "scenes/Scene.hpp"
#include "_VertexArray.h"
#include "_VertexBuffer.h"
#include "UniformBuffer.hpp"
#include "Shader.h"
#include "Texture.h"

namespace Scenes {
	class EmissionScene: public Scene {
	public:
		EmissionScene( GLuint width, GLuint height, GLFWwindow* window ) : Scene{ width, height, window }, m_LightPos{ glm::vec3( 1.2f, 1.0f, 2.0f ) } {}
		virtual ~EmissionScene() override {}
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
			
			m_TextureDiffuse = std::make_unique<Texture>( "res/textures/container_diffuse.png" );
			m_TextureSpecular = std::make_unique<Texture>( "res/textures/container_specular.png" );
			m_TextureEmissive = std::make_unique<Texture>( "res/textures/container_emission.jpg" );

			m_VBO = std::make_unique<_VertexBuffer>( vertices, sizeof( vertices ) );
			m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // position
			m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // normal
			m_VBO->AddLayoutElement( GL_FLOAT, 2 ); // tex coords

			m_VAO = std::make_unique<_VertexArray>();
			m_VAO->AddBuffer( *m_VBO );

			m_VAOLight = std::make_unique<_VertexArray>();
			m_VAOLight->AddBuffer( *m_VBO );

			m_Shader = std::make_unique<Shader>( "res/shaders/Emission.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Material.diffuse", 0 );
			m_Shader->SetUniform1i( "u_Material.specular", 1 );
			m_Shader->SetUniform1i( "u_Material.emission", 2 );
			m_Shader->SetUniform3f( "u_Light.position", m_LightPos.r, m_LightPos.g, m_LightPos.b );

			unsigned int matricesBindingPoint = 0;
			m_Shader->LinkUniformBlock( "ub_Matrices", matricesBindingPoint );
			m_UB = std::make_unique <UniformBuffer>( 2 * sizeof( glm::mat4 ), matricesBindingPoint );
			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			m_UB->BufferSubData( glm::value_ptr( projection ), sizeof( glm::mat4 ), 0 );

			m_ShaderLight = std::make_unique<Shader>( "res/shaders/Color.shader" );
			m_ShaderLight->Bind();
			m_ShaderLight->SetUniform3f( "u_Color", 1.0f, 1.0f, 1.0f );
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

			glm::mat4 view = m_Camera.GetViewMatrix();
			m_UB->BufferSubData( glm::value_ptr( view ), sizeof( glm::mat4 ), sizeof( glm::mat4 ) );

			m_TextureDiffuse->ActivateTexture( 0 );
			m_TextureDiffuse->Bind();

			m_TextureSpecular->ActivateTexture( 1 );
			m_TextureSpecular->Bind();
			
			m_TextureEmissive->ActivateTexture( 2 );
			m_TextureEmissive->Bind();

			GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ) );
			GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ) );

			glm::mat4 model = glm::mat4( 1.0f );
			m_Shader->SetUniformMat4f( "u_Model", model );
			m_Shader->SetUniform1f( "u_Time", glfwGetTime() );
			m_Shader->SetUniform3f( "u_Light.ambient", 0.2f, 0.2f, 0.2f );
			m_Shader->SetUniform3f( "u_Light.diffuse", 0.5f, 0.5f, 0.5f );
			m_Shader->SetUniform3f( "u_Light.specular", 1.0f, 1.0f, 1.0f );

			m_Shader->SetUniform1f( "u_Material.shininess", 32.0f );
			m_Shader->SetUniform3f( "u_ViewPos", m_Camera.Position.r, m_Camera.Position.g, m_Camera.Position.b );
			renderer.DrawArrays( *m_VAO, 36, *m_Shader );

			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			model = glm::mat4( 1.0f );
			model = glm::translate( model, m_LightPos );
			model = glm::scale( model, glm::vec3( 0.2f ) ); // a smaller cube
			glm::mat4 mvp = projection * view * model; // THE RIGHT WAY pvm
			m_ShaderLight->Bind();
			m_ShaderLight->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOLight, 36, *m_ShaderLight );
		}
	private:
		std::unique_ptr<_VertexArray> m_VAO;
		std::unique_ptr<_VertexArray> m_VAOLight;
		
		std::unique_ptr<_VertexBuffer> m_VBO;
		
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderLight;
		
		std::unique_ptr<Texture> m_TextureDiffuse;
		std::unique_ptr<Texture> m_TextureSpecular;
		std::unique_ptr<Texture> m_TextureEmissive;

		glm::vec3 m_LightPos;

		std::unique_ptr<UniformBuffer> m_UB;
	};
}
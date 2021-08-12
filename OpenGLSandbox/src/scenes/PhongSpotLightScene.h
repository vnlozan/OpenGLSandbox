#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

namespace Scenes {

	class PhongSpotLightScene: public Scene {
	public:
		PhongSpotLightScene( GLuint width, GLuint height, GLFWwindow* window )
			: Scene{ width, height, window } {}
		virtual ~PhongSpotLightScene() override {}
		virtual void OnStart() override {
			Scene::OnStart();

			// tell GLFW to capture our mouse
			glfwSetInputMode( m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

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
			GLfloat vertices[] = {
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

			cubePositions.emplace_back( glm::vec3( 0.0f, 0.0f, 0.0f ) );
			cubePositions.emplace_back( glm::vec3( 2.0f, 5.0f, -15.0f ) );
			cubePositions.emplace_back( glm::vec3( -1.5f, -2.2f, -2.5f ) );
			cubePositions.emplace_back( glm::vec3( -3.8f, -2.0f, -12.3f ) );
			cubePositions.emplace_back( glm::vec3( 2.4f, -0.4f, -3.5f ) );
			cubePositions.emplace_back( glm::vec3( -1.7f, 3.0f, -7.5f ) );
			cubePositions.emplace_back( glm::vec3( 1.3f, -2.0f, -2.5f ) );
			cubePositions.emplace_back( glm::vec3( 1.5f, 2.0f, -2.5f ) );
			cubePositions.emplace_back( glm::vec3( 1.5f, 0.2f, -1.5f ) );
			cubePositions.emplace_back( glm::vec3( -1.3f, 1.0f, -1.5f ) );

			// Configure VBO
			m_VBO = std::make_unique<VertexBuffer>( vertices, sizeof( vertices ) );
			VertexBufferLayout layout; // position x3, normal x3, uv x2
			layout.Push<float>( 3 );
			layout.Push<float>( 3 );
			layout.Push<float>( 2 );
			// Configure lightning object
			m_VAO = std::make_unique<VertexArray>();
			m_VAO->AddBuffer( *m_VBO, layout );
			m_Texture = std::make_unique<Texture>( "res/textures/container_diffuse.png" );
			m_Texture->Bind( 0 );
			m_TextureSpec = std::make_unique<Texture>( "res/textures/container_specular.png" );
			m_TextureSpec->Bind( 1 );
			m_Shader = std::make_unique<Shader>( "res/shaders/PhongSpotLight.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Material.diffuse", 0 );
			m_Shader->SetUniform1i( "u_Material.specular", 1 );
			m_VAO->Unbind();
			m_Shader->Unbind();
			m_VBO->Unbind();
		}
		virtual void OnImGuiRender() override {
			Scene::OnImGuiRender();
			{
				ImGui::Begin( "Info" );
				ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
				ImGui::End();
			}
		}
		virtual void OnRender( Renderer& render ) override {
			m_Shader->Bind();
			m_Texture->Bind( 0 );
			m_TextureSpec->Bind( 1 );


			m_Shader->SetUniform3f( "u_Light.position", m_Camera.Position );
			m_Shader->SetUniform3f( "u_Light.direction", m_Camera.Front );
			m_Shader->SetUniform1f( "u_Light.cutOff", glm::cos( glm::radians( 12.5f ) ) );
			m_Shader->SetUniform1f( "u_Light.outerCutOff", glm::cos( glm::radians( 17.5f ) ) );
			m_Shader->SetUniform3f( "u_ViewPos", m_Camera.Position );

			m_Shader->SetUniform3f( "u_Light.ambient", 0.1f, 0.1f, 0.1f );
			m_Shader->SetUniform3f( "u_Light.diffuse", 0.8f, 0.8f, 0.8f );
			m_Shader->SetUniform3f( "u_Light.specular", 1.0f, 1.0f, 1.0f );
			m_Shader->SetUniform1f( "u_Light.constant", 1.0f );
			m_Shader->SetUniform1f( "u_Light.linear", 0.09f );
			m_Shader->SetUniform1f( "u_Light.quadratic", 0.032f );
			m_Shader->SetUniform1f( "u_Material.shininess", 32.0f );

			glm::mat4 projection = glm::perspective( 
				glm::radians( m_Camera.Zoom ), 
				( float ) m_Width / ( float ) m_Height,
				0.1f, 
				100.0f
			);
			glm::mat4 view = m_Camera.GetViewMatrix();

			for( size_t i = 0; i < cubePositions.size(); i++ ) {

				glm::mat4 model = glm::mat4( 1.0f );
				model = glm::translate( model, cubePositions[i] );

				float angle = 20.0f * i;
				model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 0.3f, 0.5f ) );
				glm::mat4 mvp = projection * view * model;

				m_Shader->SetUniformMat4f( "u_Model", model );
				m_Shader->SetUniformMat4f( "u_MVP", mvp );
				render.DrawArrays( *m_VAO, 36, *m_Shader );
			}
		}
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Texture> m_TextureSpec;
		std::unique_ptr<VertexArray> m_VAO_lightSource;
		std::unique_ptr<Shader> m_Shader_lightSource;
		std::vector<glm::vec3> cubePositions;
	};
}
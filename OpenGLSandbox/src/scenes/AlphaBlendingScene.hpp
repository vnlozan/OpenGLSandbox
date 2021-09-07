#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"
#include "Shader.h"
#include "Texture.h"

#include <map>

namespace Scenes {
	class AlphaBlendingScene: public Scene {
	public:
		AlphaBlendingScene( GLuint width, GLuint height, GLFWwindow* window ) : Scene{ width, height, window } {}
		virtual ~AlphaBlendingScene() override {}
		virtual void OnStart( Renderer& renderer ) override {
			Scene::OnStart( renderer );
			renderer.EnableBlend();

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

			GLfloat cubeVertices[] = {
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
			GLfloat planeVertices[] = {
				// positions       // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
				5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
				-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
				-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

				5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
				-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
				5.0f, -0.5f, -5.0f, 2.0f, 2.0f
			};
			GLfloat vegetVertices[] = {
				// positions         // texture Coords
				0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
				0.0f, -0.5f, 0.0f, 0.0f, 0.0f,
				1.0f, -0.5f, 0.0f, 1.0f, 0.0f,

				0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
				1.0f, -0.5f, 0.0f, 1.0f, 0.0f,
				1.0f, 0.5f, 0.0f, 1.0f, 1.0f
			};
			
			m_TransparentObjectPositions.push_back( glm::vec3( -1.5f, 0.0f, -0.48f ) );
			m_TransparentObjectPositions.push_back( glm::vec3( 1.5f, 0.0f, 0.51f ) );
			m_TransparentObjectPositions.push_back( glm::vec3( 0.0f, 0.0f, 0.7f ) );
			m_TransparentObjectPositions.push_back( glm::vec3( -0.3f, 0.0f, -2.3f ) );
			m_TransparentObjectPositions.push_back( glm::vec3( 0.5f, 0.0f, -0.6f ) );

			m_CubeTexture = std::make_unique<Texture>( "res/textures/marble.jpg" );
			m_PlaneTexture = std::make_unique<Texture>( "res/textures/metal.png" );
			m_WindowTexture = std::make_unique<Texture>( "res/textures/window.png", Texture::TYPE::DIFFUSE, GL_CLAMP_TO_BORDER );
			m_VegetationTexture = std::make_unique<Texture>( "res/textures/grass.png", Texture::TYPE::DIFFUSE, GL_CLAMP_TO_BORDER );

			{
				m_CubeVBO = std::make_unique<VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
				VertexBufferLayout layout;
				layout.Push<float>( 3 );
				layout.Push<float>( 2 );
				m_CubeVAO = std::make_unique<VertexArray>();
				m_CubeVAO->AddBuffer( *m_CubeVBO, layout );
				m_CubeVAO->Unbind();
				m_CubeVBO->Unbind();
			}
			{
				m_PlaneVBO = std::make_unique<VertexBuffer>( planeVertices, sizeof( planeVertices ) );
				VertexBufferLayout layout;
				layout.Push<float>( 3 );
				layout.Push<float>( 2 );
				m_PlaneVAO = std::make_unique<VertexArray>();
				m_PlaneVAO->AddBuffer( *m_PlaneVBO, layout );
				m_PlaneVAO->Unbind();
				m_PlaneVBO->Unbind();
			}
			{
				m_VegetVBO = std::make_unique<VertexBuffer>( vegetVertices, sizeof( vegetVertices ) );
				VertexBufferLayout layout;
				layout.Push<float>( 3 );
				layout.Push<float>( 2 );
				m_VegetVAO = std::make_unique<VertexArray>();
				m_VegetVAO->AddBuffer( *m_VegetVBO, layout );
				m_VegetVAO->Unbind();
				m_VegetVBO->Unbind();
			}
			
			m_Shader = std::make_unique<Shader>( "res/shaders/Basic_1.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Texture", 0 );

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
			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();
			
			m_Shader->Bind();

			m_PlaneTexture->ActivateTexture( 0 );
			m_PlaneTexture->Bind();
			glm::mat4 model = glm::mat4( 1.0f );
			glm::mat4 mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_PlaneVAO, 36, *m_Shader );

			m_CubeTexture->ActivateTexture( 0 );
			m_CubeTexture->Bind();
			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_CubeVAO, 36, *m_Shader );
			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_CubeVAO, 36, *m_Shader );

			m_WindowTexture->ActivateTexture( 0 );
			m_WindowTexture->Bind();

			std::map<float, glm::vec3> sorted;
			for( unsigned int i = 0; i < m_TransparentObjectPositions.size(); i++ ) {
				float distance = glm::length( m_Camera.Position - m_TransparentObjectPositions[i] );
				sorted[distance] = m_TransparentObjectPositions[i];
			}
			for( std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it ) {
				model = glm::mat4( 1.0f );
				model = glm::translate( model, it->second );
				mvp = projection * view * model;
				model = glm::mat4( 1.0f );
				model = glm::translate( model, it->second );
				m_Shader->SetUniformMat4f( "u_MVP", mvp );
				renderer.DrawArrays( *m_VegetVAO, 36, *m_Shader );
			}
		}
	private:
		std::unique_ptr<VertexArray> m_CubeVAO;
		std::unique_ptr<VertexArray> m_PlaneVAO;
		std::unique_ptr<VertexArray> m_VegetVAO;

		std::unique_ptr<VertexBuffer> m_CubeVBO;
		std::unique_ptr<VertexBuffer> m_PlaneVBO;
		std::unique_ptr<VertexBuffer> m_VegetVBO;

		std::unique_ptr<Shader> m_Shader;

		std::unique_ptr<Texture> m_VegetationTexture;
		std::unique_ptr<Texture> m_WindowTexture;
		std::unique_ptr<Texture> m_CubeTexture;
		std::unique_ptr<Texture> m_PlaneTexture;

		std::vector<glm::vec3> m_TransparentObjectPositions;
	};
}
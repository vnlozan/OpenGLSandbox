#include <memory>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"

#include "Shader.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "Texture.h"

namespace Scenes {
	class SkyboxScene: public Scene {
	public:
		SkyboxScene( GLuint width, GLuint height, GLFWwindow* window ): Scene{ width, height, window }, m_EnvironmentMappingMode{ 3 } {}
		virtual ~SkyboxScene() override {}
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
				// positions          // normals
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
				0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
				-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

				-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
				-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
				-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
				-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
				0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

				-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
				0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
				-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
				-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
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
			float skyboxVertices[] = {
				// positions          
				-1.0f, 1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,

				-1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,

				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,

				-1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f
			};
			
			std::vector<std::string> faces {
				"res/textures/skybox/right.jpg",
				"res/textures/skybox/left.jpg",
				"res/textures/skybox/top.jpg",
				"res/textures/skybox/bottom.jpg",
				"res/textures/skybox/front.jpg",
				"res/textures/skybox/back.jpg"
			};

			m_TextureMarble = std::make_unique<Texture2D>( "res/textures/marble.jpg", Texture2D::TYPE::DIFFUSE, true );
			m_TextureMetal = std::make_unique<Texture2D>( "res/textures/metal.png", Texture2D::TYPE::DIFFUSE, true );
			m_Cubemap = std::make_unique<TextureCubemap>( faces );

			m_VBOSkybox = std::make_unique<VertexBuffer>( skyboxVertices, sizeof( skyboxVertices ) );
			m_VBOSkybox->AddLayoutElement( GL_FLOAT, 3 );
			m_VAOSkybox = std::make_unique<VertexArray>();
			m_VAOSkybox->AddBuffer( *m_VBOSkybox );

			m_VBOCube = std::make_unique<VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 );
			m_VAOCube = std::make_unique<VertexArray>();
			m_VAOCube->AddBuffer( *m_VBOCube );

			m_VBOPlane = std::make_unique<VertexBuffer>( planeVertices, sizeof( planeVertices ) );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 3 );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 2 );
			m_VAOPlane = std::make_unique<VertexArray>();
			m_VAOPlane->AddBuffer( *m_VBOPlane );

			m_Shader = std::make_unique<Shader>( "res/shaders/EnvironmentMapping.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Skybox", 0 );

			m_ShaderSkybox = std::make_unique<Shader>( "res/shaders/Skybox.shader" );
			m_ShaderSkybox->Bind();
			m_ShaderSkybox->SetUniform1i( "u_Skybox", 0 );
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
				ImGui::Text( "Cube draw" );
				int state = m_EnvironmentMappingMode;
				ImGui::RadioButton( "Reflection", &m_EnvironmentMappingMode, 1 ); ImGui::SameLine();
				ImGui::RadioButton( "Refraction", &m_EnvironmentMappingMode, 2 ); ImGui::SameLine();
				ImGui::RadioButton( "Red color", &m_EnvironmentMappingMode, 3 ); ImGui::SameLine();
				if( state != m_EnvironmentMappingMode ) {
					m_Shader->Bind();
					m_Shader->SetUniform1i( "u_Mode", m_EnvironmentMappingMode );
				}
				ImGui::End();
			}
		}
		virtual void OnRender( Renderer& renderer ) override {
			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();

			glm::mat4 model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			glm::mat4 mvp = projection * view * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			m_Shader->SetUniformMat4f( "u_Model", model );
			m_Shader->SetUniform3f( "u_CameraPos", m_Camera.Position );
			renderer.DrawArrays( *m_VAOCube, 36, *m_Shader );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			mvp = projection * view * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			m_Shader->SetUniformMat4f( "u_Model", model );
			m_Shader->SetUniform3f( "u_CameraPos", m_Camera.Position );
			renderer.DrawArrays( *m_VAOCube, 36, *m_Shader );

			glDepthFunc( GL_LEQUAL );  // change depth function so depth test passes when values are equal to depth buffer's content
			m_Cubemap->ActivateTexture( 0 );
			m_Cubemap->Bind();
			view = glm::mat4( glm::mat3( m_Camera.GetViewMatrix() ) ); // remove translation from the view matrix
			mvp = projection * view;
			m_ShaderSkybox->Bind();
			m_ShaderSkybox->SetUniformMat4f( "u_VP", mvp );
			renderer.DrawArrays( *m_VAOSkybox, 36, *m_ShaderSkybox );
			glDepthFunc( GL_LESS ); // set depth function back to default
		}
	private:
		std::unique_ptr<VertexArray> m_VAOCube;
		std::unique_ptr<VertexArray> m_VAOPlane;
		std::unique_ptr<VertexArray> m_VAOSkybox;

		std::unique_ptr<VertexBuffer> m_VBOCube;
		std::unique_ptr<VertexBuffer> m_VBOPlane;
		std::unique_ptr<VertexBuffer> m_VBOSkybox;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderScreen;
		std::unique_ptr<Shader> m_ShaderSkybox;

		std::unique_ptr<Texture2D> m_TextureMarble;
		std::unique_ptr<Texture2D> m_TextureMetal;
		std::unique_ptr<TextureCubemap> m_Cubemap;

		int m_EnvironmentMappingMode;
	};
}
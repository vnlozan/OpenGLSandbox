#include <map>
#include <memory>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "scenes/Scene.hpp"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "FrameBuffer.h"
#include "RenderBuffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Scenes {
	class PostProcessingScene: public Scene {
	public:
		PostProcessingScene( GLuint width, GLuint height, GLFWwindow* window ): Scene{ width, height, window }, m_Mode{ 5 } {}
		virtual ~PostProcessingScene() override {}
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
			float quadVertices[] = {
				//positions	//textCoords
				-1.0f, 1.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 1.0f, 0.0f,

				-1.0f, 1.0f, 0.0f, 1.0f,
				1.0f, -1.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 1.0f, 1.0f
			};
			
			m_TextureColorBuffer = std::make_unique<Texture>( m_Width, m_Height );
			m_TextureCube = std::make_unique<Texture>( "res/textures/marble.jpg" );
			m_TexturePlane = std::make_unique<Texture>( "res/textures/metal.png" );
			
			m_VBOCube = std::make_unique<VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 2 );
			m_VAOCube = std::make_unique<VertexArray>();
			m_VAOCube->AddBuffer( *m_VBOCube );

			m_VBOPlane = std::make_unique<VertexBuffer>( planeVertices, sizeof( planeVertices ) );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 3 );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 2 );
			m_VAOPlane = std::make_unique<VertexArray>();
			m_VAOPlane->AddBuffer( *m_VBOPlane );

			m_VBOQuad = std::make_unique<VertexBuffer>( quadVertices, sizeof( quadVertices ) );
			m_VBOQuad->AddLayoutElement( GL_FLOAT, 2 );
			m_VBOQuad->AddLayoutElement( GL_FLOAT, 2 );
			m_VAOQuad = std::make_unique<VertexArray>();
			m_VAOQuad->AddBuffer( *m_VBOQuad );
			
			m_ShaderScreen = std::make_unique<Shader>( "res/shaders/PostProcessing.shader" );
			m_ShaderScreen->Bind();
			m_ShaderScreen->SetUniform1i( "u_ScreenTexture", 0 );
			m_ShaderScreen->SetUniform1i( "u_Mode", 2 );

			m_Shader = std::make_unique<Shader>( "res/shaders/Texture.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Texture", 0 );

			m_RBO = std::make_unique<RenderBuffer>( m_Width, m_Height );
			m_RBO->Bind();
			
			m_FBO = std::make_unique<FrameBuffer>();
			m_FBO->Bind();
			m_FBO->AddColorAttachment( *m_TextureColorBuffer );
			m_FBO->AddDepthStencilAttachment( *m_RBO );
			if( !m_FBO->IsComplete() ) {
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			}
			m_FBO->Unbind();
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
				ImGui::Text( "Post processing" );
				int state = m_Mode;
				ImGui::RadioButton( "Inversion", &m_Mode, 1 ); ImGui::SameLine();
				ImGui::RadioButton( "Grayscale", &m_Mode, 2 ); ImGui::SameLine();
				ImGui::RadioButton( "SharpenKernel", &m_Mode, 3 ); ImGui::SameLine();
				ImGui::RadioButton( "Blur", &m_Mode, 4 ); ImGui::SameLine();
				ImGui::RadioButton( "No effect", &m_Mode, 5 ); ImGui::SameLine();
				if( state != m_Mode ) {
					m_ShaderScreen->Bind();
					m_ShaderScreen->SetUniform1i( "u_Mode", m_Mode );
				}
				ImGui::End();
			}
		}
		virtual void OnRender( Renderer& renderer ) override {
			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();

			// bind to framebuffer and draw scene as we normally would to color texture 
			m_FBO->Bind();
			renderer.EnableDepthTest();
			// make sure we clear the framebuffer's content
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			m_Shader->Bind();

			m_TexturePlane->ActivateTexture( 0 );
			m_TexturePlane->Bind();
			glm::mat4 model = glm::mat4( 1.0f );
			glm::mat4 mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOPlane, 6, *m_Shader );

			m_TextureCube->ActivateTexture( 0 );
			m_TextureCube->Bind();
			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOCube, 36, *m_Shader );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOCube, 36, *m_Shader );

			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			m_FBO->Unbind();
			renderer.EnableDepthTest( false ); // disable depth test so screen-space quad isn't discarded due to depth test.
			glClear( GL_COLOR_BUFFER_BIT ); // clear all relevant buffers
			m_ShaderScreen->Bind();
			m_TextureColorBuffer->ActivateTexture( 0 );
			m_TextureColorBuffer->Bind();
			renderer.DrawArrays( *m_VAOQuad, 6, *m_ShaderScreen );
		}
	private:
		std::unique_ptr<VertexArray> m_VAOCube;
		std::unique_ptr<VertexArray> m_VAOPlane;
		std::unique_ptr<VertexArray> m_VAOQuad;

		std::unique_ptr<VertexBuffer> m_VBOCube;
		std::unique_ptr<VertexBuffer> m_VBOPlane;
		std::unique_ptr<VertexBuffer> m_VBOQuad;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderScreen;

		std::unique_ptr<RenderBuffer> m_RBO;
		std::unique_ptr<FrameBuffer> m_FBO;

		std::unique_ptr<Texture> m_TextureCube;
		std::unique_ptr<Texture> m_TexturePlane;
		std::unique_ptr<Texture> m_TextureColorBuffer;

		int m_Mode;
	};
}
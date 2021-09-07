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

namespace Scenes {
	class StencilTestsScene: public Scene {
	public:
		StencilTestsScene( GLuint width, GLuint height, GLFWwindow* window )
			: Scene{ width, height, window } {}
		virtual ~StencilTestsScene() override {}
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

			m_CubeTexture = std::make_unique<Texture>( "res/textures/marble.jpg" );
			m_PlaneTexture = std::make_unique<Texture>( "res/textures/metal.png" );

			//m_Shader = std::make_unique<Shader>( "res/shaders/DepthTests_1.shader" );
			m_Shader = std::make_unique<Shader>( "res/shaders/Basic_1.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Texture", 0 );
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
			m_ShaderColor = std::make_unique<Shader>( "res/shaders/Basic.shader" );
			m_ShaderColor->Bind();

			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_LESS );
			glEnable( GL_STENCIL_TEST );
			glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
			glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
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

			m_PlaneTexture->ActivateTexture( 0 );
			m_PlaneTexture->Bind();


			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();

			// draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers.
			// We set its mask to 0x00 to not write to the stencil buffer.
			glStencilMask( 0x00 );// disable writing to the stencil buffer
			glm::mat4 model = glm::mat4( 1.0f );
			glm::mat4 mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_PlaneVAO, 36, *m_Shader );

			// 1st. render pass, draw objects as normal, writing to the stencil buffer
			m_CubeTexture->ActivateTexture( 0 );
			m_CubeTexture->Bind();
			glStencilFunc( GL_ALWAYS, 1, 0xFF ); // all fragments should pass the stencil test
			glStencilMask( 0xFF ); // enable writing to the stencil buffer
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

			//// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
			//// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
			//// the objects' size differences, making it look like borders.
			glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
			//glStencilMask( 0x00 ); // disable writing to the stencil buffer
			glDisable( GL_DEPTH_TEST ); // the scaled up containers (e.g. the borders) do not get overwritten by the floor
			float scale = 1.05f;

			m_ShaderColor->Bind();
			m_ShaderColor->SetUniform3f( "u_Color", glm::vec3( 1.0f, 0.0f, 0.0f ) );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			model = glm::scale( model, glm::vec3( scale, scale, scale ) );
			mvp = projection * view * model;
			m_ShaderColor->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_CubeVAO, 36, *m_ShaderColor );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			model = glm::scale( model, glm::vec3( scale, scale, scale ) );
			mvp = projection * view * model;
			m_ShaderColor->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_CubeVAO, 36, *m_ShaderColor );

			//glStencilMask( 0xFF );
			glStencilFunc( GL_ALWAYS, 0, 0xFF ); // !!
			glEnable( GL_DEPTH_TEST );
		}
	private:
		std::unique_ptr<VertexArray> m_CubeVAO;
		std::unique_ptr<VertexArray> m_PlaneVAO;
		std::unique_ptr<VertexBuffer> m_CubeVBO;
		std::unique_ptr<VertexBuffer> m_PlaneVBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderColor;
		std::unique_ptr<Texture> m_CubeTexture;
		std::unique_ptr<Texture> m_PlaneTexture;
	};
}
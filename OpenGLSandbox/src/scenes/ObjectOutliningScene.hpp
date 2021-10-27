#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"

#include "Shader.h"

#include "_Texture.h"
#include "_VertexArray.h"
#include "_Buffer.h"


namespace Scenes {
	class ObjectOutliningScene: public Scene {
	public:
		ObjectOutliningScene( GLuint width, GLuint height, GLFWwindow* window ) : Scene{ width, height, window } {}
		virtual ~ObjectOutliningScene() override {}
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

			m_VBOCube = std::make_unique<_VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 2 );
			
			m_VAOCube = std::make_unique<_VertexArray>();
			m_VAOCube->AddBuffer( *m_VBOCube );

			m_VBOPlane = std::make_unique<_VertexBuffer>( planeVertices, sizeof( planeVertices ) );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 3 );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 2 );

			m_VAOPlane = std::make_unique<_VertexArray>();
			m_VAOPlane->AddBuffer( *m_VBOPlane );

			m_TextureMarble = std::make_unique<_Texture2D>( "res/textures/marble.jpg", _Texture2D::TYPE::DIFFUSE, true );
			m_TextureMetal = std::make_unique<_Texture2D>( "res/textures/metal.png", _Texture2D::TYPE::DIFFUSE, true );

			m_ShaderTexture = std::make_unique<Shader>( "res/shaders/Texture.shader" );
			m_ShaderTexture->Bind();
			m_ShaderTexture->SetUniform1i( "u_Texture", 0 );

			m_ShaderColor = std::make_unique<Shader>( "res/shaders/Color.shader" );
			m_ShaderColor->Bind();

			renderer.EnableDepthTest( true, GL_LESS );
			renderer.EnableStencilTest( true );
			renderer.SetStencilFunc( GL_NOTEQUAL, 1, 0xFF );
			renderer.SetStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
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
			
			m_ShaderTexture->Bind();
			m_TextureMetal->ActivateTexture( 0 );
			m_TextureMetal->Bind();
			// draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers.
			// We set its mask to 0x00 to not write to the stencil buffer.
			renderer.WriteStencilBuffer( false ); // disable writing to the stencil buffer
			glm::mat4 model = glm::mat4( 1.0f );
			glm::mat4 mvp = projection * view * model;
			m_ShaderTexture->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOPlane, 36, *m_ShaderTexture );

			// 1st. render pass, draw objects as normal, writing to the stencil buffer
			m_TextureMarble->ActivateTexture( 0 );
			m_TextureMarble->Bind();
			renderer.SetStencilFunc( GL_ALWAYS, 1, 0xFF ); // all fragments should pass the stencil test
			renderer.WriteStencilBuffer( true ); // enable writing to the stencil buffer

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			mvp = projection * view * model;
			m_ShaderTexture->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderTexture );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			mvp = projection * view * model;
			m_ShaderTexture->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderTexture );

			//// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
			//// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
			//// the objects' size differences, making it look like borders.
			renderer.SetStencilFunc( GL_NOTEQUAL, 1, 0xFF );
			renderer.EnableDepthTest( false ); // the scaled up containers (e.g. the borders) do not get overwritten by the floor
			float scale = 1.05f;

			m_ShaderColor->Bind();
			m_ShaderColor->SetUniform3f( "u_Color", glm::vec3( 1.0f, 0.0f, 0.0f ) );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			model = glm::scale( model, glm::vec3( scale, scale, scale ) );
			mvp = projection * view * model;
			m_ShaderColor->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderColor );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			model = glm::scale( model, glm::vec3( scale, scale, scale ) );
			mvp = projection * view * model;
			m_ShaderColor->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderColor );
			renderer.SetStencilFunc( GL_ALWAYS, 0, 0xFF );
			renderer.EnableDepthTest( true );
		}
	private:
		std::unique_ptr<_VertexArray> m_VAOCube;
		std::unique_ptr<_VertexArray> m_VAOPlane;

		std::unique_ptr<_VertexBuffer> m_VBOCube;
		std::unique_ptr<_VertexBuffer> m_VBOPlane;

		std::unique_ptr<Shader> m_ShaderTexture;
		std::unique_ptr<Shader> m_ShaderColor;

		std::unique_ptr<_Texture2D> m_TextureMarble;
		std::unique_ptr<_Texture2D> m_TextureMetal;
	};
}
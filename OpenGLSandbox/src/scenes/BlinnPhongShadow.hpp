#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"

#include "_VertexArray.h"
#include "_Texture.h"
#include "_Buffer.h"
#include "Shader.h"

namespace Scenes {

	class BlinnPhongShadow: public Scene {
	public:
		BlinnPhongShadow( GLuint width, GLuint height, GLFWwindow* window )
			: Scene{ width, height, window }, m_LightPos{ glm::vec3( -2.0f, 4.0f, -1.0f ) }, m_FarPlane{ 7.5f }, m_NearPlane{ 1.0f } {}
		virtual ~BlinnPhongShadow() override {
		}
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


			/* Prepare textures */
			m_TextureWood = std::make_unique<_Texture2D>( "res/textures/wood.png", _Texture2D::TYPE::DIFFUSE, true );
			
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			m_DepthMap = std::make_unique<_Texture2D>( 1024, 1024, GL_DEPTH_COMPONENT );
			m_DepthMap->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			m_DepthMap->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			m_DepthMap->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
			m_DepthMap->SetParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
			m_DepthMap->SetParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

			/* Prepare cube */
			float cubeVertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
				1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
				-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
				-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
				// right face
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			   // bottom face
			   -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			   1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			   1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			   1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			   -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			   -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			   // top face
			   -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			   1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			   1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			   1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			   -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			   -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left        
			};
			m_VAOCube = std::make_unique<_VertexArray>();
			m_VBOCube = std::make_unique<_VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 ); // normals
			m_VBOCube->AddLayoutElement( GL_FLOAT, 2 ); // tex coords
			m_VAOCube->AddBuffer( *m_VBOCube );

			/* Prepare plane */
			float planeVertices[] = {
				// positions            // normals			// texcoords
				25.0f, -0.5f, 25.0f,	0.0f, 1.0f, 0.0f,	25.0f, 0.0f,
				-25.0f, -0.5f, 25.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
				-25.0f, -0.5f, -25.0f,	0.0f, 1.0f, 0.0f,	0.0f, 25.0f,

				25.0f, -0.5f, 25.0f,	0.0f, 1.0f, 0.0f,	25.0f, 0.0f,
				-25.0f, -0.5f, -25.0f,	0.0f, 1.0f, 0.0f,	0.0f, 25.0f,
				25.0f, -0.5f, -25.0f,	0.0f, 1.0f, 0.0f,	25.0f, 10.0f
			};
			m_VAOPlane = std::make_unique<_VertexArray>();
			m_VBOPlane = std::make_unique<_VertexBuffer>( planeVertices, sizeof( planeVertices ) );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 3 ); // normals
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 2 ); // tex coords
			m_VAOPlane->AddBuffer( *m_VBOPlane );


			m_FBO = std::make_unique<_FrameBuffer>();
			m_FBO->AddDepthAttachment( *m_DepthMap );
			m_FBO->SetDrawBuffer( GL_NONE );
			m_FBO->SetReadBuffer( GL_NONE );
			m_FBO->Unbind();

			/* Prepare shaders */
			glm::mat4 lightProjection = glm::ortho( -10.0f, 10.0f, -10.0f, 10.0f, m_NearPlane, m_FarPlane );
			glm::mat4 lightView = glm::lookAt( m_LightPos, glm::vec3( 0.0f ), glm::vec3( 0.0, 1.0, 0.0 ) );
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;
			m_ShaderDepthSimple = std::make_unique<Shader>( "res/shaders/DepthSimple.shader" );
			m_ShaderDepthSimple->Bind();
			m_ShaderDepthSimple->SetUniformMat4f( "u_LightSpaceMatrix", lightSpaceMatrix );

			m_ShaderBlinnPhongShadow = std::make_unique<Shader>( "res/shaders/BlinnPhongShadow.shader" );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniform1i( "u_DiffuseTexture", 0 );
			m_ShaderBlinnPhongShadow->SetUniform1i( "u_ShadowMap", 1 );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_LightSpaceMatrix", lightSpaceMatrix );
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

			// 1. render depth of scene to texture (from light's perspective)
			m_ShaderDepthSimple->Bind();

			renderer.SetViewport( 0, 0, 1024, 1024 );
			m_FBO->Bind();
			renderer.Clear( GL_DEPTH_BUFFER_BIT );

			glm::mat4 model = glm::mat4( 1.0f );
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOPlane, 6, *m_ShaderDepthSimple, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 0.0f, 1.5f, 0.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 1.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, 2.0 ) );
			model = glm::rotate( model, glm::radians( 60.0f ), glm::normalize( glm::vec3( 1.0, 0.0, 1.0 ) ) );
			model = glm::scale( model, glm::vec3( 0.25 ) );
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			m_FBO->Unbind();

			renderer.SetViewport( 0, 0, m_Width, m_Height );
			renderer.Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			// 2. render scene as normal using the generated depth/shadow map
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_ProjectionView", projection * view );
			m_ShaderBlinnPhongShadow->SetUniform3f( "u_ViewPos", m_Camera.Position );
			m_ShaderBlinnPhongShadow->SetUniform3f( "u_LightPos", m_LightPos );
			m_ShaderBlinnPhongShadow->SetUniform3f( "u_LightColor", glm::vec3( 0.3 ) );


			m_TextureWood->ActivateTexture( 0 );
			m_TextureWood->Bind();
			m_DepthMap->ActivateTexture( 1 );
			m_DepthMap->Bind();

			model = glm::mat4( 1.0f );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOPlane, 6, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 0.0f, 1.5f, 0.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 1.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, 2.0 ) );
			model = glm::rotate( model, glm::radians( 60.0f ), glm::normalize( glm::vec3( 1.0, 0.0, 1.0 ) ) );
			model = glm::scale( model, glm::vec3( 0.25 ) );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );
		}
	private:
		float m_NearPlane;
		float m_FarPlane;

		std::unique_ptr<_Texture2D> m_DepthMap;
		std::unique_ptr<_Texture2D> m_TextureWood;
	
		std::unique_ptr<_VertexBuffer> m_VBOPlane;
		std::unique_ptr<_VertexBuffer> m_VBOCube;

		std::unique_ptr<_VertexArray> m_VAOQuad;
		std::unique_ptr<_VertexArray> m_VAOCube;
		std::unique_ptr<_VertexArray> m_VAOPlane;

		std::unique_ptr<_RenderBuffer> m_RBO;
		std::unique_ptr<_FrameBuffer> m_FBO;

		std::unique_ptr<Shader> m_ShaderDepthSimple;
		std::unique_ptr<Shader> m_ShaderBlinnPhongShadow;
		
		glm::vec3 m_LightPos;
	};
}
#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"

#include "VertexArray.h"
#include "Texture.h"
#include "Buffer.h"
#include "Shader.h"

namespace Scenes {
	class BlinnPhongPointShadow: public Scene {
	public:
		BlinnPhongPointShadow( GLuint width, GLuint height, GLFWwindow* window ): Scene{ width, height, window },
			m_LightPos{ glm::vec3( -2.0f, 4.0f, -1.0f ) },
			m_FarPlane{ 25.0f },
			m_NearPlane{ 1.0f },
			m_DepthMapWidth{ 1024 },
			m_DepthMapHeight{ 1024 },
			m_CalculateShadows{ 1 }
		{}
		virtual ~BlinnPhongPointShadow() override {}
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
			m_TextureWood = std::make_unique<Texture2D>( "res/textures/wood.png", Texture2D::TYPE::DIFFUSE, true );
			m_DepthMap = std::make_unique<TextureCubemap>( m_DepthMapWidth, m_DepthMapHeight );

			/* Prepare framebuffer object */
			m_FBO = std::make_unique<FrameBuffer>();
			m_FBO->AddCubeMapDepthAttachment( *m_DepthMap );
			m_FBO->SetDrawBuffer( GL_NONE );
			m_FBO->SetReadBuffer( GL_NONE );
			m_FBO->Unbind();

			/* Prepare cube */
			float cubeVertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f, // bottom-left
				1.0f, 1.0f, -1.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f, // bottom-right         
				1.0f, 1.0f, -1.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f, // bottom-left
				-1.0f, 1.0f, -1.0f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom-left
				1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f, // bottom-right
				1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top-right
				1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top-right
				-1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top-left
				-1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom-left
				// left face
				-1.0f, 1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,	1.0f, 0.0f, // top-right
				-1.0f, 1.0f, -1.0f,		-1.0f, 0.0f, 0.0f,	1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // bottom-right
				-1.0f, 1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,	1.0f, 0.0f, // top-right
				// right face
				1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f, // top-left
				1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f, // top-right         
				1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f, // top-left
				1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // bottom-left     
			   // bottom face
			   -1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f, // top-right
			   1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,	1.0f, 1.0f, // top-left
			   1.0f, -1.0f, 1.0f,		0.0f, -1.0f, 0.0f,	1.0f, 0.0f, // bottom-left
			   1.0f, -1.0f, 1.0f,		0.0f, -1.0f, 0.0f,	1.0f, 0.0f, // bottom-left
			   -1.0f, -1.0f, 1.0f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f, // bottom-right
			   -1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f, // top-right
			   // top face
			   -1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // top-left
			   1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // bottom-right
			   1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f, // top-right     
			   1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // bottom-right
			   -1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // top-left
			   -1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f  // bottom-left        
			};
			m_VAOCube = std::make_unique<VertexArray>();
			m_VBOCube = std::make_unique<VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 ); // normals
			m_VBOCube->AddLayoutElement( GL_FLOAT, 2 ); // tex coords
			m_VAOCube->AddBuffer( *m_VBOCube );

			/* Create depth cubemap transformation matrices */
			glm::mat4 shadowProj = glm::perspective( glm::radians( 90.0f ), float( m_DepthMapWidth ) / float( m_DepthMapHeight ), m_NearPlane, m_FarPlane );
			std::vector<glm::mat4> shadowTransforms;
			shadowTransforms.push_back( shadowProj * glm::lookAt( m_LightPos, m_LightPos + glm::vec3( 1.0f, 0.0f, 0.0f ), glm::vec3( 0.0f, -1.0f, 0.0f ) ) );
			shadowTransforms.push_back( shadowProj * glm::lookAt( m_LightPos, m_LightPos + glm::vec3( -1.0f, 0.0f, 0.0f ), glm::vec3( 0.0f, -1.0f, 0.0f ) ) );
			shadowTransforms.push_back( shadowProj * glm::lookAt( m_LightPos, m_LightPos + glm::vec3( 0.0f, 1.0f, 0.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) );
			shadowTransforms.push_back( shadowProj * glm::lookAt( m_LightPos, m_LightPos + glm::vec3( 0.0f, -1.0f, 0.0f ), glm::vec3( 0.0f, 0.0f, -1.0f ) ) );
			shadowTransforms.push_back( shadowProj * glm::lookAt( m_LightPos, m_LightPos + glm::vec3( 0.0f, 0.0f, 1.0f ), glm::vec3( 0.0f, -1.0f, 0.0f ) ) );
			shadowTransforms.push_back( shadowProj * glm::lookAt( m_LightPos, m_LightPos + glm::vec3( 0.0f, 0.0f, -1.0f ), glm::vec3( 0.0f, -1.0f, 0.0f ) ) );
			
			/* Prepare shaders */
			m_ShaderDepthSimple = std::make_unique<Shader>( "res/shaders/DepthPointShadows.shader" );
			m_ShaderDepthSimple->Bind();
			for( unsigned int i = 0; i < 6; ++i ) {
				m_ShaderDepthSimple->SetUniformMat4f( "u_ShadowMatrices[" + std::to_string( i ) + "]", shadowTransforms[i] );
			}

			m_ShaderBlinnPhongShadow = std::make_unique<Shader>( "res/shaders/BlinnPhongShadowPoint.shader" );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniform1i( "u_DiffuseTexture", 0 );
			m_ShaderBlinnPhongShadow->SetUniform1i( "u_DepthMap", 1 );
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
			// 1. render scene to depth cubemap
			renderer.SetViewport( 0, 0, m_DepthMapWidth, m_DepthMapHeight ); // set depthmap resolution
			m_FBO->Bind();
			renderer.Clear( GL_DEPTH_BUFFER_BIT );

			/* Render Room Cube */
			glm::mat4 model = glm::mat4( 1.0f );
			model = glm::scale( model, glm::vec3( 5.0f ) );
			m_ShaderDepthSimple->Bind();
			m_ShaderDepthSimple->SetUniform1f( "u_FarPlane", m_FarPlane );
			m_ShaderDepthSimple->SetUniform3f( "u_LightPos", m_LightPos );
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			/* Render cubes */
			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 4.0f, -3.5f, 0.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderDepthSimple->Bind();
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 3.0f, 1.0 ) );
			model = glm::scale( model, glm::vec3( 0.75f ) );
			m_ShaderDepthSimple->Bind();
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -3.0f, -1.0f, 0.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderDepthSimple->Bind();
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.5f, 1.0f, 1.5 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderDepthSimple->Bind();
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.5f, 2.0f, -3.0 ) );
			model = glm::rotate( model, glm::radians( 60.0f ), glm::normalize( glm::vec3( 1.0, 0.0, 1.0 ) ) );
			model = glm::scale( model, glm::vec3( 0.75f ) );
			m_ShaderDepthSimple->Bind();
			m_ShaderDepthSimple->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderDepthSimple, GL_TRIANGLES );

			m_FBO->Unbind();
			renderer.SetViewport( 0, 0, m_Width, m_Height ); // get back screen resolution
			renderer.Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			// 2. render scene as normal 
			glm::mat4 projection = glm::perspective( glm::radians( m_Camera.Zoom ), ( float )m_Width / ( float )m_Height, 0.1f, 100.0f );
			glm::mat4 view = m_Camera.GetViewMatrix();

			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniform3f( "u_ViewPos", m_Camera.Position );
			m_ShaderBlinnPhongShadow->SetUniform1i( "u_CalculateShadows", m_CalculateShadows );
			m_ShaderBlinnPhongShadow->SetUniform3f( "u_LightPos", m_LightPos );
			m_ShaderBlinnPhongShadow->SetUniform1f( "u_FarPlane", m_FarPlane );

			m_TextureWood->ActivateTexture( 0 );
			m_TextureWood->Bind();
			m_DepthMap->ActivateTexture( 1 );
			m_DepthMap->Bind();

			/* Render cube */
			model = glm::mat4( 1.0f );
			model = glm::scale( model, glm::vec3( 5.0f ) );
			renderer.EnableFaceCull( false );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniform1i( "u_ReverseNormals", 1 );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_PVM", projection * view * model );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniform1i( "u_ReverseNormals", 0 );
			renderer.EnableFaceCull( true );

			/* Render cubes */
			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 4.0f, -3.5f, 0.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_PVM", projection * view * model );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 3.0f, 1.0 ) );
			model = glm::scale( model, glm::vec3( 0.75f ) );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_PVM", projection * view * model );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -3.0f, -1.0f, 0.0 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_PVM", projection* view* model );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.5f, 1.0f, 1.5 ) );
			model = glm::scale( model, glm::vec3( 0.5f ) );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_PVM", projection* view* model );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.5f, 2.0f, -3.0 ) );
			model = glm::rotate( model, glm::radians( 60.0f ), glm::normalize( glm::vec3( 1.0, 0.0, 1.0 ) ) );
			model = glm::scale( model, glm::vec3( 0.75f ) );
			m_ShaderBlinnPhongShadow->Bind();
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_PVM", projection* view* model );
			m_ShaderBlinnPhongShadow->SetUniformMat4f( "u_Model", model );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderBlinnPhongShadow, GL_TRIANGLES );
		}
	private:
		int			m_CalculateShadows;
		float		m_DepthMapWidth;
		float		m_DepthMapHeight;
		float		m_NearPlane;
		float		m_FarPlane;
		glm::vec3	m_LightPos;

		std::unique_ptr<TextureCubemap> m_DepthMap;
		std::unique_ptr<Texture2D>		m_TextureWood;
		std::unique_ptr<VertexArray>	m_VAOCube;
		std::unique_ptr<VertexBuffer>	m_VBOCube;
		std::unique_ptr<RenderBuffer>	m_RBO;
		std::unique_ptr<FrameBuffer>	m_FBO;
		std::unique_ptr<Shader>			m_ShaderDepthSimple;
		std::unique_ptr<Shader>			m_ShaderBlinnPhongShadow;
	};
}
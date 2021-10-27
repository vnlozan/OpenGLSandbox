#include <memory>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scenes/Scene.hpp"

#include "Shader.h"

#include "_Texture.h"
#include "_VertexArray.h"
#include "_Buffer.h"

namespace Scenes {
	class AlphaBlendingScene: public Scene {
	public:
		AlphaBlendingScene( GLuint width, GLuint height, GLFWwindow* window ): Scene{ width, height, window } {}
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

			float cubeVertices[] = {
				// cw
				// Back face
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right    
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right              
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left                
				// Front face
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
				0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
				0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right        
				0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
				-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left        
				// Left face
				-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
				-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
				-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left       
				-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
				-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
				-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
				// Right face
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right      
				0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right          
				0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
				0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
				0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
			   // Bottom face
			   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
			   0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
			   0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left        
			   0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
			   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
			   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
			   // Top face
			   -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
			   0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
			   0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right                 
			   0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
			   -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left  
			   -0.5f, 0.5f, -0.5f, 0.0f, 1.0f  // top-left              
			};
			float planeVertices[] = {
				// ccw
				// positions       // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
				5.0f, -0.6f, 5.0f, 2.0f, 0.0f,
				-5.0f, -0.6f, 5.0f, 0.0f, 0.0f,
				-5.0f, -0.6f, -5.0f, 0.0f, 2.0f,

				5.0f, -0.6f, 5.0f, 2.0f, 0.0f,
				-5.0f, -0.6f, -5.0f, 0.0f, 2.0f,
				5.0f, -0.6f, -5.0f, 2.0f, 2.0f
			};
			float vegetVertices[] = {
				// ccw
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

			m_TextureMarble = std::make_unique<_Texture2D>( "res/textures/marble.jpg", _Texture2D::TYPE::DIFFUSE, true );
			m_TextureMetal = std::make_unique<_Texture2D>( "res/textures/metal.png", _Texture2D::TYPE::DIFFUSE, true );
			m_TextureWindow = std::make_unique<_Texture2D>( "res/textures/window.png", _Texture2D::TYPE::DIFFUSE, GL_CLAMP_TO_BORDER );
			m_TextureVegetation = std::make_unique<_Texture2D>( "res/textures/grass.png", _Texture2D::TYPE::DIFFUSE, GL_CLAMP_TO_BORDER );


			m_VBOCube = std::make_unique<_VertexBuffer>( cubeVertices, sizeof( cubeVertices ) );
			m_VBOCube->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBOCube->AddLayoutElement( GL_FLOAT, 2 ); // tex coords
			m_VAOCube = std::make_unique<_VertexArray>();
			m_VAOCube->AddBuffer( *m_VBOCube );

			m_VBOPlane = std::make_unique<_VertexBuffer>( planeVertices, sizeof( planeVertices ) );
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBOPlane->AddLayoutElement( GL_FLOAT, 2 ); // tex coords
			m_VAOPlane = std::make_unique<_VertexArray>();
			m_VAOPlane->AddBuffer( *m_VBOPlane );

			m_VBOVeget = std::make_unique<_VertexBuffer>( vegetVertices, sizeof( vegetVertices ) );
			m_VBOVeget->AddLayoutElement( GL_FLOAT, 3 ); // positions
			m_VBOVeget->AddLayoutElement( GL_FLOAT, 2 ); // tex coords
			m_VAOVeget = std::make_unique<_VertexArray>();
			m_VAOVeget->AddBuffer( *m_VBOVeget );

			m_Shader = std::make_unique<Shader>( "res/shaders/Texture.shader" );
			m_Shader->Bind();
			m_Shader->SetUniform1i( "u_Texture", 0 );

			m_ShaderNormal = std::make_unique<Shader>( "res/shaders/TriangleNormal.shader" );
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

			m_TextureMetal->ActivateTexture( 0 );
			m_TextureMetal->Bind();
			glm::mat4 model = glm::mat4( 1.0f );
			glm::mat4 mvp = projection * view * model;
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAOPlane, 36, *m_Shader );

			renderer.EnableFaceCull( true, GL_BACK, GL_CW );

			m_TextureMarble->ActivateTexture( 0 );
			m_TextureMarble->Bind();
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

			m_TextureWindow->ActivateTexture( 0 );
			m_TextureWindow->Bind();

			renderer.EnableFaceCull( false );

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
				renderer.DrawArrays( *m_VAOVeget, 36, *m_Shader );
			}

			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 2.0f, 0.0f, 0.0f ) );
			mvp = projection * view * model;
			m_ShaderNormal->Bind();
			m_ShaderNormal->SetUniformMat4f( "u_Model", model );
			m_ShaderNormal->SetUniformMat4f( "u_View", view );
			m_ShaderNormal->SetUniformMat4f( "u_Projection", projection );
			renderer.DrawArrays( *m_VAOCube, 36, *m_ShaderNormal );
		}
	private:
		std::unique_ptr<_VertexArray> m_VAOCube;
		std::unique_ptr<_VertexArray> m_VAOPlane;
		std::unique_ptr<_VertexArray> m_VAOVeget;

		std::unique_ptr<_VertexBuffer> m_VBOCube;
		std::unique_ptr<_VertexBuffer> m_VBOPlane;
		std::unique_ptr<_VertexBuffer> m_VBOVeget;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderNormal;

		std::unique_ptr<_Texture2D> m_TextureVegetation;
		std::unique_ptr<_Texture2D> m_TextureWindow;
		std::unique_ptr<_Texture2D> m_TextureMarble;
		std::unique_ptr<_Texture2D> m_TextureMetal;

		std::vector<glm::vec3> m_TransparentObjectPositions;
	};
}
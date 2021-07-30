#pragma once
#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

namespace tests {
	class TestSimpleLight: public Test {
	public:
		TestSimpleLight( GLuint width, GLuint height, GLFWwindow*& window ): Test( width, height ), window{ window } {}
		~TestSimpleLight() {}
		void OnStart() {

			lastX = m_Width / 2.0f;
			lastY = m_Height / 2.0f;
			firstMouse = true;

			glfwSetWindowUserPointer( window, (void*)this );
			glfwSetCursorPosCallback( window, []( GLFWwindow* window, double xpos, double ypos ) {
				TestSimpleLight* tsl = ( TestSimpleLight* ) glfwGetWindowUserPointer( window );
				if( tsl->firstMouse ) {
					tsl->lastX = xpos;
					tsl->lastY = ypos;
					tsl->firstMouse = false;
				}
				float xoffset = xpos - tsl->lastX;
				float yoffset = tsl->lastY - ypos; // reversed since y-coordinates go from bottom to top

				tsl->lastX = xpos;
				tsl->lastY = ypos;

				tsl->camera.ProcessMouseMovement( xoffset, yoffset );
			} );
			glfwSetScrollCallback( window, [] ( GLFWwindow* window, double xoffset, double yoffset ) {
				TestSimpleLight* tsl = ( TestSimpleLight* ) glfwGetWindowUserPointer( window );
				tsl->camera.ProcessMouseScroll( yoffset );
			} );
			// tell GLFW to capture our mouse
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

			// Configure VBO
			// 6 cube facets
			GLfloat vertices[] = {
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// triangle 1 vertices + texture coordinates (uv)
				0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
				-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,	// triangle 2 vertices + texture coordinates (uv)

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
			m_VBO = std::make_unique<VertexBuffer>( vertices, sizeof( vertices ) );
			VertexBufferLayout layout; // position x3, uv x2,
			layout.Push<float>( 3 );
			layout.Push<float>( 2 );
			// Configure lightning object
			m_VAO = std::make_unique<VertexArray>();
			m_VAO->AddBuffer( *m_VBO, layout );

			m_Texture = std::make_unique<Texture>( "res/textures/bricks.jpg" );
			m_Texture->Bind( 0 );

			m_Shader = std::make_unique<Shader>( "res/shaders/Basic_1.shader" );
			m_Shader->Bind();

			m_Shader->SetUniform1i( "u_Texture", 0 );
			m_Shader->SetUniform3f( "u_LightColor", 1.0f, 0.0f, 0.0f );

			m_VAO->Unbind();
			m_Shader->Unbind();
			// Configure lightning source
			m_VAO_lightSource = std::make_unique<VertexArray>();
			m_VAO_lightSource->AddBuffer( *m_VBO, layout );

			m_Shader_lightSource = std::make_unique<Shader>( "res/shaders/Basic.shader" );
			m_Shader_lightSource->Bind();
			m_Shader_lightSource->SetUniform3f( "u_Color", 1.0f, 1.0f, 1.0f );

			m_VAO_lightSource->Unbind();
			m_Shader_lightSource->Unbind();
			//
			m_VBO->Unbind();
		}
		void OnUpdate( float deltaTime ) override {
			processInput( deltaTime );
		}
		void OnRender( Renderer& renderer ) override {
			renderer.Clear();

			m_Shader->Bind();
			glm::mat4 projection = glm::perspective( glm::radians( camera.Zoom ), ( float ) m_Width / ( float ) m_Height, 0.1f, 100.0f );
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 model = glm::mat4( 1.0f );
			glm::mat4 mvp = projection * view * model; // THE RIGHT WAY pvm
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAO, 36, *m_Shader );


			model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( 1.2f, 1.0f, 2.0f ) );
			model = glm::scale( model, glm::vec3( 0.2f ) ); // a smaller cube
			mvp = projection * view * model; // THE RIGHT WAY pvm
			m_Shader_lightSource->Bind();
			m_Shader_lightSource->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAO_lightSource, 36, *m_Shader_lightSource );
		}
		void OnImGuiRender() override {
			//{
			//	ImGui::Begin( "View Transform" );
			//	ImGui::SliderFloat3( "Translation", &view_Translation.x, -( float ) m_Width, ( float ) m_Width );
			//	ImGui::SliderFloat3( "Scale", &view_Scale.x, 0.0f, 1.0f );
			//	ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
			//	ImGui::End();
			//}
			//{
			//	ImGui::Begin( "Model Transform" );
			//	ImGui::SliderFloat3( "Translation", &model_Translation.x, -( float ) m_Width, ( float ) m_Width );
			//	ImGui::SliderFloat( "YRotation", &model_Rotation.y, -360.0f, 360.0f );
			//	ImGui::SliderFloat3( "Scale", &model_Scale.x, 0.0f, 1.0f );
			//	ImGui::End();
			//}
		}
		void processInput( float deltaTime ) {
			if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
				glfwSetWindowShouldClose( window, true );
			}
			if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS ) {
				camera.ProcessKeyboard( Camera::Movement::FORWARD, deltaTime );
			}
			if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS ) {
				camera.ProcessKeyboard( Camera::Movement::BACKWARD, deltaTime );
			}
			if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS ) {
				camera.ProcessKeyboard( Camera::Movement::LEFT, deltaTime );
			}
			if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS ) {
				camera.ProcessKeyboard( Camera::Movement::RIGHT, deltaTime );
			}
		}
public:
	float lastX;
	float lastY;
	bool firstMouse;
	Camera camera;
private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		std::unique_ptr<VertexArray> m_VAO_lightSource;
		std::unique_ptr<Shader> m_Shader_lightSource;

		GLFWwindow* window;
	};
}
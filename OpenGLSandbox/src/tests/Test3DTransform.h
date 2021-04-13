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
#include "imgui_1.79/imgui.h"
#include "imgui_1.79/imgui_impl_glfw.h"
#include "imgui_1.79/imgui_impl_opengl3.h"
#include <memory>

namespace tests {
	class Test3DTransform : public Test {
	public:
		Test3DTransform( GLuint width, GLuint height )
			: Test( width, height ),
			model_Translation{ 0.0f, 0.0f, 0.0f }, model_Rotation{ 0.0f, 0.0f, 0.0f }, model_Scale{ 1.0f, 1.0f, 1.0f },
			model_R{ 1.0f },
			view_Translation{ 0.0f, 0.0f, 80.0f }, view_Rotation{ 0.0f, 0.0f, 0.0f }, view_Scale{ 0.5f, 0.5f, 0.5f } 
		{
			GLfloat vertices[] = {
				-50.0f, -50.0f, -50.0f,  0.0f, 0.0f,
				 50.0f, -50.0f, -50.0f,  1.0f, 0.0f,
				 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
				 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
				-50.0f,  50.0f, -50.0f,  0.0f, 1.0f,
				-50.0f, -50.0f, -50.0f,  0.0f, 0.0f,

				-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
				 50.0f, -50.0f,  50.0f,  1.0f, 0.0f,
				 50.0f,  50.0f,  50.0f,  1.0f, 1.0f,
				 50.0f,  50.0f,  50.0f,  1.0f, 1.0f,
				-50.0f,  50.0f,  50.0f,  0.0f, 1.0f,
				-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,

				-50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
				-50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
				-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
				-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
				-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
				-50.0f,  50.0f,  50.0f,  1.0f, 0.0f,

				 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
				 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
				 50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
				 50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
				 50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
				 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,

				-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
				 50.0f, -50.0f, -50.0f,  1.0f, 1.0f,
				 50.0f, -50.0f,  50.0f,  1.0f, 0.0f,
				 50.0f, -50.0f,  50.0f,  1.0f, 0.0f,
				-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
				-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,

				-50.0f,  50.0f, -50.0f,  0.0f, 1.0f,
				 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
				 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
				 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
				-50.0f,  50.0f,  50.0f,  0.0f, 0.0f,
				-50.0f,  50.0f, -50.0f,  0.0f, 1.0f
			};
			
			m_VAO = std::make_unique<VertexArray>();
			m_VBO = std::make_unique<VertexBuffer>( vertices, 5 * 36 * sizeof( float ) );

			VertexBufferLayout layout;
			layout.Push<float>( 3 );
			layout.Push<float>( 2 );
			
			m_VAO->AddBuffer( *m_VBO, layout );
			m_Shader = std::make_unique<Shader>( "res/shaders/Basic_2.shader" );
			m_Shader->Bind();
			m_Texture = std::make_unique<Texture>( "res/textures/bricks.jpg" );
			m_Texture->Bind( 0 );
			m_Shader->SetUniform1i( "u_Texture", 0 );

			m_VAO->Unbind();
			m_Shader->Unbind();
			m_VBO->Unbind();
		}
		~Test3DTransform() {}
		void OnUpdate( float deltaTime ) override {
		}
		void OnRender( Renderer& renderer ) override {
			renderer.Clear();
			m_Shader->Bind();
			// MODEL
			glm::mat4 modelMat_Translation = glm::translate( glm::mat4( 1.0f ), model_Translation );
			glm::mat4 modelMat_Scale = glm::scale( glm::mat4( 1.0f ), model_Scale );
			//glm::mat4 modelMat_Rotation( 1.0f );
			glm::mat4 modelMat_Rotation = glm::rotate( model_R, glm::radians( model_Rotation.y ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
			glm::mat4 model = modelMat_Translation * modelMat_Rotation * modelMat_Scale;
			// VIEW
			glm::mat4 viewMat_Translation = glm::translate( glm::mat4( 1.0f ), -view_Translation );
			glm::mat4 viewMat_Scale = glm::scale( glm::mat4( 1.0f ), view_Scale );
			glm::mat4 viewMat_Rotation( 1.0f );
			glm::mat4 view = viewMat_Translation * viewMat_Rotation * viewMat_Scale;
			// PROJECTION
			glm::mat4 projection = glm::perspective( 45.0f, ( GLfloat )m_Width / ( GLfloat )m_Height, 0.1f, 100.0f ); // increase by z = 1000
			// PROJECTION x VIEW x MODEL
			glm::mat4 mvp = projection * view * model; // THE RIGHT WAY pvm
			m_Shader->SetUniformMat4f( "u_MVP", mvp );
			renderer.DrawArrays( *m_VAO, 36, *m_Shader );
		}
		void OnImGuiRender() override {
			{
				ImGui::Begin( "View Transform" );
				ImGui::SliderFloat3( "Translation", &view_Translation.x, -( float )m_Width, ( float )m_Width );
				ImGui::SliderFloat3( "Scale", &view_Scale.x, 0.0f, 1.0f );
				ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
				ImGui::End();
			}
			{
				ImGui::Begin( "Model Transform" );
				ImGui::SliderFloat3( "Translation", &model_Translation.x, -( float )m_Width, ( float )m_Width );
				ImGui::SliderFloat( "YRotation", &model_Rotation.y, -360.0f, 360.0f );
				ImGui::SliderFloat3( "Scale", &model_Scale.x, 0.0f, 1.0f );
				ImGui::End();
			}
		}
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::vec3 model_Translation;
		glm::vec3 model_Rotation;
		glm::mat4 model_R;
		glm::vec3 model_Scale;
		glm::vec3 view_Translation;
		glm::vec3 view_Rotation;
		glm::vec3 view_Scale;
	};
}
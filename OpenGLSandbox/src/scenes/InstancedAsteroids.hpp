#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm/gtc/type_ptr.hpp>

#include "scenes/Scene.hpp"

#include "Shader.h"
#include "Model.h"

namespace Scenes {
	class InstancedAsteroids: public Scene {
	public:
		InstancedAsteroids( GLuint width, GLuint height, GLFWwindow* window ): Scene{ width, height, window }, m_AsteroidsAmount{ 100000 } {}
		virtual ~InstancedAsteroids() override {}
		virtual void OnStart( Renderer& renderer ) override {
			Scene::OnStart( renderer );

			renderer.EnableFaceCull( true, GL_FRONT, GL_CCW );


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


			m_ModelMatrices = new glm::mat4[m_AsteroidsAmount];
			srand( glfwGetTime() ); // initialize random seed	
			float radius = 50.0;
			float offset = 25.0f;
			for( unsigned int i = 0; i < m_AsteroidsAmount; i++ )     {
				glm::mat4 model = glm::mat4( 1.0f );

				// 1. translation: displace along circle with 'radius' in range [-offset, offset]
				float angle = ( float ) i / ( float ) m_AsteroidsAmount * 360.0f;
				float displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
				float x = sin( angle ) * radius + displacement;
				displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
				float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
				displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
				float z = cos( angle ) * radius + displacement;
				model = glm::translate( model, glm::vec3( x, y, z ) );

				// 2. scale: Scale between 0.05 and 0.25f
				float scale = ( rand() % 20 ) / 100.0f + 0.05;
				model = glm::scale( model, glm::vec3( scale ) );

				// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
				float rotAngle = ( rand() % 360 );
				model = glm::rotate( model, rotAngle, glm::vec3( 0.4f, 0.6f, 0.8f ) );

				// 4. now add to list of matrices
				m_ModelMatrices[i] = model;
			}

			m_ModelPlanet = std::make_unique<Model>( "res/models/planet/planet.obj" );
			m_ModelAsteroid = std::make_unique<Model>( "res/models/rock/rock.obj" );

			m_ShaderPlanet = std::make_unique<Shader>( "res/shaders/ModelRotating.shader" );
			m_ShaderAsteroid = std::make_unique<Shader>( "res/shaders/InstancedRotAsteroids.shader" );

			// 4x4 matrix
			m_VBO = std::make_unique<VertexBuffer>( m_ModelMatrices, m_AsteroidsAmount * sizeof( m_ModelMatrices ) );
			m_VBO->AddLayoutElement( GL_FLOAT, 4, GL_FALSE, true, 1 ); // 5
			m_VBO->AddLayoutElement( GL_FLOAT, 4, GL_FALSE, true, 1 ); // 6
			m_VBO->AddLayoutElement( GL_FLOAT, 4, GL_FALSE, true, 1 ); // 7
			m_VBO->AddLayoutElement( GL_FLOAT, 4, GL_FALSE, true, 1 ); // 8

			for( Mesh& mesh : m_ModelAsteroid->m_Meshes ) {
				mesh.AddVBO( *m_VBO );
			}
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

			m_ShaderPlanet->Bind();
			glm::mat4 model = glm::mat4( 1.0f );
			model = glm::translate( model, glm::vec3( -1.0f, 0.0f, -1.0f ) );
			m_ShaderPlanet->SetUniformMat4f( "u_Projection", projection );
			m_ShaderPlanet->SetUniformMat4f( "u_View", view );
			m_ShaderPlanet->SetUniformMat4f( "u_Model", model );
			m_ShaderPlanet->SetUniform1f( "u_AngularVelocity", ( float ) glfwGetTime() * glm::radians( 360.0f ) / 5.0f ); // rad/sec
			m_ModelPlanet->Draw( renderer, *m_ShaderPlanet );

			m_ShaderAsteroid->Bind();
			m_ShaderAsteroid->SetUniformMat4f( "u_Projection", projection );
			m_ShaderAsteroid->SetUniform1f( "u_Angle", ( float ) glfwGetTime() * glm::radians( 30.0f ) );
			m_ShaderAsteroid->SetUniformMat4f( "u_View", view );
			m_ModelAsteroid->DrawInstanced( renderer, *m_ShaderAsteroid, m_AsteroidsAmount );
		}
	private:
		std::unique_ptr<VertexBuffer> m_VBO;

		unsigned int m_AsteroidsAmount;
		glm::mat4* m_ModelMatrices;

		std::unique_ptr<Shader> m_ShaderPlanet;
		std::unique_ptr<Shader> m_ShaderAsteroid;

		std::unique_ptr<Model> m_ModelPlanet;
		std::unique_ptr<Model> m_ModelAsteroid;
	};
}
#include <iostream>
#include "scenes/Scene.hpp"
#include "scenes/MenuScene.hpp"
#include "Log.h"

class Application {
public:
	Application( GLuint width, GLuint height, std::string windowTitle )
		: m_Renderer{}, m_Width{ width }, m_Height{ height }, m_WindowTitle{ windowTitle }, m_CurrentScene{ nullptr }, m_Window{ nullptr } {}
	Application( const Application& ) = delete;
	Application( const Application&& ) = delete;
	~Application() {}
	
	template<typename T>
	void RegisterScene( const char* name ) {
		//std::cout << "Registering test " << name << std::endl;
		LOG_WARN( ( std::string )"Registering test " + name );
		m_Scenes.push_back( std::make_pair( name, [&, name] () {
			this->ChangeWindowTitle( m_WindowTitle + " - " + name );
			T* t = new T( m_Width, m_Height, m_Window );
			t->OnStart( *m_Renderer );
			t->SetMenuBackFunction( [this](void) { this->GetBackToMenu(); } );
			return t;
		} ) );
	}
	int Init() {
		if( !glfwInit() ) {
			return -1;
		}
		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
		glfwWindowHint( GLFW_SAMPLES, 4 ); /* GLFW MSAA Enable */

		m_Window = glfwCreateWindow( m_Width, m_Height, m_WindowTitle.c_str(), NULL, NULL );
		if( !m_Window ) {
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent( m_Window );

		/* Vertical synchronization Enable */
		glfwSwapInterval( 1 );


		if( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) ) {
			std::cout << "Glad Error" << std::endl;
			return -1;
		}

		m_Renderer->EnableMSAA( true );
		m_Renderer->EnableGammaCorrection( true );

		/* Resizable window */
		glfwWindowHint( GLFW_RESIZABLE, GL_TRUE );
		glfwSetFramebufferSizeCallback( m_Window, [] ( GLFWwindow* window, int width, int height ) {
			glViewport( 0, 0, width, height );
		} );

		SetDefaults();

		InitMenu();

		return 1;
	}
	void PlayLoop() {
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;
		float maxPeriod = 1.0f / 60.0f;

		StartImGui();
		
		while( !glfwWindowShouldClose( m_Window ) ) {
			
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;

			if( deltaTime >= maxPeriod ) {
				lastFrame = currentFrame;

				glfwPollEvents();

				m_Renderer->Clear();

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				m_CurrentScene->OnUpdate( deltaTime );
				m_CurrentScene->OnRender( *m_Renderer );
				m_CurrentScene->OnImGuiRender();

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

				glfwSwapBuffers( m_Window );
			}
		}
		
		delete m_CurrentScene;
		
		ShutDownImGui();

		glfwTerminate();
	}
private:
	void InitMenu() {
		m_CurrentScene = new Scenes::MenuScene( m_Width, m_Height, m_Window, &m_CurrentScene, &m_Scenes );
		m_CurrentScene->OnStart( *m_Renderer );
	}
	void ChangeWindowTitle( std::string name ) {
		glfwSetWindowTitle( m_Window, name.c_str() );
	}
	void StartImGui() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); ( void ) io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL( m_Window, true );
		ImGui_ImplOpenGL3_Init( ( char* ) glGetString( GL_NUM_SHADING_LANGUAGE_VERSIONS ) );
	}
	void ShutDownImGui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void GetBackToMenu() {
		SetDefaults();
		ChangeWindowTitle( m_WindowTitle );
		delete m_CurrentScene;
		InitMenu();
	}
	void SetDefaults() {
		m_Renderer->ClearColor();
		m_Renderer->SetPolygonMode();
		m_Renderer->EnableDepthTest();
		//m_Renderer->EnableStencilTest( true );
		//m_Renderer->EnableBlend();
		//m_Renderer->EnableFaceCull();
	}
public:
	GLFWwindow* m_Window;
	GLuint m_Width;
	GLuint m_Height;
	std::string m_WindowTitle;
private:
	Renderer* m_Renderer;
	std::vector < std::pair<std::string, std::function<Scenes::Scene* ( )>>> m_Scenes;
	Scenes::Scene* m_CurrentScene;
};
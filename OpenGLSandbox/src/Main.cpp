#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "tests/Test3DTransform.h"
#include "tests/TestClearColor.h"
#include "tests/TestSimpleLight.h"
#include "tests/TestMenu.h"


static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mode ) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose( window, GL_TRUE );
	}
}

int main( void ) {
	const GLuint WIDTH = 800, HEIGHT = 600;
	GLFWwindow* window;
	if ( !glfwInit() ) { return -1; }
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
	window = glfwCreateWindow( WIDTH, HEIGHT, "OpenGL Sandbox", NULL, NULL );
	if ( !window ) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // Vertical synchronization
	glfwSetKeyCallback( window, key_callback );


	// tell GLFW to capture our mouse
	//glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );


	if ( !gladLoadGLLoader( ( GLADloadproc )glfwGetProcAddress ) ) {
		std::cout << "Glad Error" << std::endl;
	}


	glViewport( 0, 0, WIDTH, HEIGHT );
	GLCall( glEnable( GL_BLEND ) );
	GLCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
	unsigned int vao;
	GLCall( glGenVertexArrays( 1, &vao ) );
	GLCall( glBindVertexArray( vao ) );

	Renderer renderer;
	tests::TestMenu testMenu{ WIDTH, HEIGHT, window };
	testMenu.StartImGui();
	testMenu.RegisterTest<tests::TestClearColor>( "Scene: Clear Color" );
	testMenu.RegisterTest<tests::Test3DTransform>( "Scene: 3D Transform" );
	testMenu.RegisterTest<tests::TestSimpleLight>( "Scene: 3D Light" );

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose( window )) {
		glfwPollEvents();
		renderer.Clear();

		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		testMenu.OnUpdate( deltaTime );
		testMenu.OnRender( renderer );
		testMenu.OnImGuiRender();
		glfwSwapBuffers( window );
	}
	testMenu.ShutDownImGui();
	glfwTerminate();
	return 0;
}
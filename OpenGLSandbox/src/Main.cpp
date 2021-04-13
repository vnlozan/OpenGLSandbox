#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "tests/Test3DTransform.h"
#include "tests/TestClearColor.h"
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
	glfwSwapInterval( 1 );								// Vertical synchronization
	glfwSetKeyCallback( window, key_callback );
	glewExperimental = GL_TRUE;							// Use a modern approach to retrieving function pointers and extensions
	if ( glewInit() != GLEW_OK ) {
		std::cout << "Glew Error" << std::endl;
	}
	glViewport( 0, 0, WIDTH, HEIGHT );
	GLCall( glEnable( GL_BLEND ) );
	GLCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
	unsigned int vao;
	GLCall( glGenVertexArrays( 1, &vao ) );
	GLCall( glBindVertexArray( vao ) );
	Renderer renderer;
	tests::TestMenu testMenu{ WIDTH, HEIGHT };
	testMenu.StartImGui( window );
	testMenu.RegisterTest<tests::TestClearColor>( "Scene: Clear Color" );
	testMenu.RegisterTest<tests::Test3DTransform>( "Scene: 3D Transform" );
	while (!glfwWindowShouldClose( window )) {
		glfwPollEvents();
		renderer.Clear();
		testMenu.OnUpdate( 0.0f );
		testMenu.OnRender( renderer );
		testMenu.OnImGuiRender();
		glfwSwapBuffers( window );
	}
	testMenu.ShutDownImGui();
	glfwTerminate();
	return 0;
}
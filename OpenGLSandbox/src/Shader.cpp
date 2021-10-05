#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader( const std::string& filepath ) : m_FilePath{ filepath }, m_RendererId{ 0 } {
	std::string vertexShader, fragmentShader, geometryShader;
	ParseShader( filepath, vertexShader, fragmentShader, geometryShader );

	m_RendererId = CreateShader( vertexShader, fragmentShader, geometryShader );
}
Shader::~Shader() {
	GLCall( glDeleteProgram( m_RendererId ) );
}
void Shader::Bind() const {
	GLCall( glUseProgram( m_RendererId ) );
}
void Shader::Unbind() const {
	GLCall( glUseProgram( 0 ) );
}
void Shader::SetUniform2f( const std::string& name, glm::vec2& vec ) {
	GLCall( glUniform2f( GetUniformLocation( name ), vec.r, vec.g ) );
}
void Shader::SetUniform3f( const std::string& name, glm::vec3& vec3 ) {
	GLCall( glUniform3f( GetUniformLocation( name ), vec3.r, vec3.g, vec3.b ) );
}
void Shader::SetUniform3f( const std::string& name, float v0, float v1, float v2 ) {
	GLCall( glUniform3f( GetUniformLocation( name ), v0, v1, v2 ) );
}
void Shader::SetUniform4f( const std::string& name, float v0, float v1, float v2, float v3 ) {
	GLCall( glUniform4f( GetUniformLocation( name ), v0, v1, v2, v3 ) );
}
void Shader::SetUniform1f( const std::string& name, float value ) {
	GLCall( glUniform1f( GetUniformLocation( name ), value ) );
}
void Shader::SetUniform1i( const std::string& name, int value ) {
	GLCall( glUniform1i( GetUniformLocation( name ), value ) );
}
void Shader::SetUniformMat4f( const std::string& name, const glm::mat4& matrix ) {
	GLCall( glUniformMatrix4fv( GetUniformLocation( name ), 1, GL_FALSE, &matrix[0][0] ) );
}
void Shader::LinkUniformBlock( const std::string& key, int bindingPoint ) {
	unsigned int index = GetUniformBlockIndex( key );
	glUniformBlockBinding( m_RendererId, index, bindingPoint );
}
unsigned int Shader::GetUniformBlockIndex( const std::string& key ) {
	return glGetUniformBlockIndex( m_RendererId, key.c_str() );
}
unsigned int Shader::CompileShader( unsigned int type, const std::string& source ) {
	unsigned int id = glCreateShader( type );
	const char* src = source.c_str(); // or &source[0]
	glShaderSource( id, 1, &src, nullptr );
	glCompileShader( id );
	int result;
	glGetShaderiv( id, GL_COMPILE_STATUS, &result );
	if( result == GL_FALSE ) {
		int length;
		glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
		char* message = ( char* ) _malloca( length * sizeof( char ) );
		glGetShaderInfoLog( id, length, &length, message );
		std::cout << "Failed to compile " << ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) << std::endl;
		std::cout << message << std::endl;
		glDeleteShader( id );
		return 0;
	}
	return id;
}
unsigned int Shader::CreateShader( const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader ) {
	unsigned int program = glCreateProgram();
	
	unsigned int vs = CompileShader( GL_VERTEX_SHADER, vertexShader );
	glAttachShader( program, vs );

	unsigned int fs = CompileShader( GL_FRAGMENT_SHADER, fragmentShader );
	glAttachShader( program, fs );
	
	unsigned int gs = 0;
	if( !geometryShader.empty() ) {
		gs = CompileShader( GL_GEOMETRY_SHADER, geometryShader );
		glAttachShader( program, gs );
	}
	
	glLinkProgram( program );
	glValidateProgram( program );

	glDeleteShader( vs );
	glDeleteShader( fs );
	if( !gs ) {
		glDeleteShader( gs );
	}
	
	return program;
}
int Shader::GetUniformLocation( const std::string& name ) {
	//GLint numActiveUniforms = 0;
	//glGetProgramiv( m_RendererId, GL_ACTIVE_UNIFORMS, &numActiveUniforms );
	//std::vector<GLchar> nameData( 256 );
	//for( int unif = 0; unif < numActiveUniforms; ++unif ) {
	//	GLint arraySize = 0;
	//	GLenum type = 0;
	//	GLsizei actualLength = 0;
	//	glGetActiveUniform( m_RendererId, unif, nameData.size(), &actualLength, &arraySize, &type, &nameData[0] );
	//	std::string name( ( char* ) &nameData[0], actualLength );
	//	std::cout << name << std::endl;
	//}


	if( m_UniformLocationCache.find( name ) != m_UniformLocationCache.end() ) {
		return m_UniformLocationCache[name];
	}
	GLCall( int location = glGetUniformLocation( m_RendererId, name.c_str() ) );
	if( location == -1 ) {
		std::cout << "Warning: uniform \'" << name << "\' doesn\'t exist!" << std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}
void Shader::ParseShader( const std::string& filePath, std::string& vertexShader, std::string& fragmentShader, std::string& geometryShader ) {
	std::ifstream stream( filePath );
	enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2 };
	ShaderType type = ShaderType::NONE;
	std::string line;
	std::stringstream ss[3];
	while( getline( stream, line ) ) {
		if( line.find( "#shader" ) != std::string::npos ) {
			if( line.find( "vertex" ) != std::string::npos ) {
				type = ShaderType::VERTEX;
			} else if( line.find( "fragment" ) != std::string::npos ) {
				type = ShaderType::FRAGMENT;
			} else if( line.find( "geometry" ) != std::string::npos ) {
				type = ShaderType::GEOMETRY;
			}
		} else {
			ss[( int ) type] << line << '\n';
		}
	}
	vertexShader = ss[( int ) ShaderType::VERTEX].str();
	fragmentShader = ss[( int ) ShaderType::FRAGMENT].str();
	geometryShader = ss[( int ) ShaderType::GEOMETRY].str();
}
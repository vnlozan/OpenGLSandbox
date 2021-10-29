#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "glm/glm.hpp"

#include "Renderer.h"
#include "Log.h"

class Shader {
public:
	Shader( const std::string& filepath ): m_FilePath{ filepath }, m_RendererId{ 0 } {
		std::string vertexShader, fragmentShader, geometryShader;
		ParseShader( filepath, vertexShader, fragmentShader, geometryShader );

		m_RendererId = CreateShader( vertexShader, fragmentShader, geometryShader );
	}
	~Shader() {
		GLCall( glDeleteProgram( m_RendererId ) );
	}

	void Bind() const {
		GLCall( glUseProgram( m_RendererId ) );
	}
	void Unbind() const {
		GLCall( glUseProgram( 0 ) );
	}

	void SetUniform2f( const std::string& name, glm::vec2& vec ) {
		GLCall( glUniform2f( GetUniformLocation( name ), vec.r, vec.g ) );
	}
	void SetUniform3f( const std::string& name, glm::vec3& vec3 ) {
		GLCall( glUniform3f( GetUniformLocation( name ), vec3.r, vec3.g, vec3.b ) );
	}
	void SetUniform3f( const std::string& name, float v0, float v1, float v2 ) {
		GLCall( glUniform3f( GetUniformLocation( name ), v0, v1, v2 ) );
	}
	void SetUniform4f( const std::string& name, float v0, float v1, float v2, float v3 ) {
		GLCall( glUniform4f( GetUniformLocation( name ), v0, v1, v2, v3 ) );
	}
	void SetUniform1f( const std::string& name, float value ) {
		GLCall( glUniform1f( GetUniformLocation( name ), value ) );
	}
	void SetUniform1i( const std::string& name, int value ) {
		GLCall( glUniform1i( GetUniformLocation( name ), value ) );
	}
	void SetUniformMat4f( const std::string& name, const glm::mat4& matrix ) {
		GLCall( glUniformMatrix4fv( GetUniformLocation( name ), 1, GL_FALSE, &matrix[0][0] ) );
	}
	void LinkUniformBlock( const std::string& key, int bindingPoint ) {
		unsigned int index = GetUniformBlockIndex( key );
		glUniformBlockBinding( m_RendererId, index, bindingPoint );
	}
private:
	unsigned int GetUniformBlockIndex( const std::string& key ) {
		return glGetUniformBlockIndex( m_RendererId, key.c_str() );
	}
	void ParseShader( const std::string& filePath, std::string& vertexShader, std::string& fragmentShader, std::string& geometryShader ) {
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
	unsigned int CompileShader( unsigned int type, const std::string& source ) {
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

			std::string shaderType;
			switch( type ) {
				case GL_VERTEX_SHADER:		shaderType = "Vertex Shader";	break;
				case GL_FRAGMENT_SHADER:	shaderType = "Fragment Shader";	break;
				case GL_GEOMETRY_SHADER:	shaderType = "Geometry Shader";	break;
			}
			LOG_ERROR( "Shader:: Failed to compile " + shaderType + "." );
			glDeleteShader( id );
			return 0;
		}
		return id;
	}
	unsigned int CreateShader( const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader ) {
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
	int GetUniformLocation( const std::string& name ) {
		if( m_UniformLocationCache.find( name ) != m_UniformLocationCache.end() ) {
			return m_UniformLocationCache[name];
		}
		GLCall( int location = glGetUniformLocation( m_RendererId, name.c_str() ) );
		if( location == -1 ) {
			LOG_WARN( "Shader:: Uniform \'" + name + "\' doesn\'t exist!" );
		}
		m_UniformLocationCache[name] = location;
		return location;
	}
private:
	unsigned int	m_RendererId;
	std::string		m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};
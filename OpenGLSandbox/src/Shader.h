#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

class Shader {
private:
	unsigned int	m_RendererId;
	std::string		m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader( const std::string& filepath );
	~Shader();

	void Bind() const;
	void Unbind() const;
	void SetUniform2f( const std::string& name, glm::vec2& vec );
	void SetUniform3f( const std::string& name, glm::vec3& vec );
	void SetUniform3f( const std::string& name, float v0, float v1, float v2 );
	void SetUniform4f( const std::string& name, float v0, float v1, float v2, float v3 );
	void SetUniform1f( const std::string& name, float value );
	void SetUniform1i( const std::string& name, int value );
	void SetUniformMat4f( const std::string& name, const glm::mat4& matrix );

	void LinkUniformBlock( const std::string& key, int bindingPoint );
private:
	unsigned int	GetUniformBlockIndex( const std::string& key );

	void			ParseShader( const std::string& filePath, std::string& vertexShader, std::string& fragmentShader, std::string& geometryShader );
	unsigned int	CompileShader( unsigned int type, const std::string& source );
	unsigned int	CreateShader( const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader );
	int				GetUniformLocation( const std::string& name );
};
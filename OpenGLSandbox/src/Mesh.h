#pragma once
#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Log.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "Texture.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Mesh {
public:
    Mesh( std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture2D>& textures )
        : m_Vertices{ std::move( vertices ) }, m_Indices{ std::move( indices ) }, m_Textures{ std::move( textures ) } {
        Setup();
    }
    void Draw( Renderer& render, Shader& shader ) {
        unsigned int diffNum = 1;
        unsigned int specNum = 1;
        unsigned int normNum = 1;
        unsigned int heiNum = 1;

        for( unsigned int i = 0; i < m_Textures.size(); i++ ) {
            m_Textures[i].ActivateTexture( i );
            switch( m_Textures[i].type ) {
                case Texture2D::TYPE::DIFFUSE:
                    shader.SetUniform1i( "u_TextureDiffuse" + std::to_string( diffNum++ ), i );
                    break;
                case Texture2D::TYPE::HEIGHT:
                    shader.SetUniform1i( "u_TextureHeight" + std::to_string( heiNum++ ), i );
                    break;
                case Texture2D::TYPE::NORMAL:
                    shader.SetUniform1i( "u_TextureNormal" + std::to_string( normNum++ ), i );
                    break;
                case Texture2D::TYPE::SPECULAR:
                    shader.SetUniform1i( "u_TextureSpecular" + std::to_string( specNum++ ), i );
                    break;
                default: break;
            }
            m_Textures[i].Bind();
        }
        render.DrawElements( *m_VAO, *m_IBO, shader );
    }
    void DrawInstanced( Renderer& render, Shader& shader, int instancesCount ) {
        unsigned int diffNum = 1;
        unsigned int specNum = 1;
        unsigned int normNum = 1;
        unsigned int heiNum = 1;

        for( unsigned int i = 0; i < m_Textures.size(); i++ ) {
            m_Textures[i].ActivateTexture( i );
            switch( m_Textures[i].type ) {
                case Texture2D::TYPE::DIFFUSE:
                    shader.SetUniform1i( "u_TextureDiffuse" + std::to_string( diffNum++ ), i );
                    break;
                case Texture2D::TYPE::HEIGHT:
                    shader.SetUniform1i( "u_TextureHeight" + std::to_string( heiNum++ ), i );
                    break;
                case Texture2D::TYPE::NORMAL:
                    shader.SetUniform1i( "u_TextureNormal" + std::to_string( normNum++ ), i );
                    break;
                case Texture2D::TYPE::SPECULAR:
                    shader.SetUniform1i( "u_TextureSpecular" + std::to_string( specNum++ ), i );
                    break;
                default: break;
            }
            m_Textures[i].Bind();
        }
        render.DrawElementsInstanced( *m_VAO, *m_IBO, shader, instancesCount );
    }
    void AddVBO( const VertexBuffer& vbo ) {
        m_VAO->Bind();
        m_VAO->AddBuffer( vbo );
        m_VAO->Unbind();
    }
private:
    void Setup() {
        LOG_INFO( "Mesh:: There are " + std::to_string( m_Vertices.size() ) + " vertices." );
        LOG_INFO( "Mesh:: There are " + std::to_string( m_Indices.size() ) + " indices." );

        m_VAO = std::make_unique<VertexArray>();
        m_VAO->Bind();
        m_VBO = std::make_unique<VertexBuffer>( m_Vertices.data(), m_Vertices.size() * sizeof( Vertex ) );
        m_VBO->Bind();
        m_IBO = std::make_unique<IndexBuffer>( m_Indices.data(), m_Indices.size() * sizeof( unsigned int ), m_Indices.size() );
        m_IBO->Bind();

        m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // vertices
        m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // normals
        m_VBO->AddLayoutElement( GL_FLOAT, 2 ); // texture coords
        m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // vertex  tangents
        m_VBO->AddLayoutElement( GL_FLOAT, 3 ); // vetex   bitangents

        m_VAO->AddBuffer( *m_VBO );

        m_IBO->Unbind();
        m_VBO->Unbind();
        m_VAO->Unbind();
    }
public:
    std::vector<Vertex>             m_Vertices;
    std::vector<unsigned int>       m_Indices;
    std::unique_ptr<VertexArray>    m_VAO;
    std::vector<Texture2D>          m_Textures;
private:
    std::unique_ptr<VertexBuffer>   m_VBO;
    std::unique_ptr<IndexBuffer>    m_IBO;
};
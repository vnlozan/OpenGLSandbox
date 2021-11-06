#pragma once
#include <vector>
#include <unordered_map>
#include <fstream>

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "stb_image/stb_image.h"

#include "Log.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Renderer;
class Shader;

static const int MIN_TEX_COUNT = 5;

class Model {
public:
    Model( std::string const& path, bool gamma=false ): m_GammaCorrection{ gamma } {
        //m_TexturesLoaded.reserve( 5 );

        m_TLoaded.reserve( MIN_TEX_COUNT );

        std::ifstream fin( path.c_str() );
        if( !fin.fail() ) 	{
            fin.close();
        } else {
            LOG_WARN( "ASSIMP::Model file doesnt exist!" );
        }

        loadModel( path );
    }
    void Draw( Renderer& render, Shader& shader ) {
        for( Mesh& mesh : m_Meshes ) {
            mesh.Draw( render, shader );
        }
    }
    void DrawInstanced( Renderer& render, Shader& shader, int instancesCount ) {
        for( Mesh& mesh : m_Meshes ) {
            mesh.DrawInstanced( render, shader, instancesCount );
        }
    }
    //inline auto Meshes() const { return m_Meshes.data(); }
private:
    /* Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector. */
    void loadModel( std::string const& path ) {

        LOG_INFO( "Model:: Loading model from path: " + path );

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile( path,
            aiProcess_MakeLeftHanded |
            aiProcess_FlipWindingOrder |
            aiProcess_FlipUVs |
            aiProcess_PreTransformVertices |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_Triangulate |
            aiProcess_FixInfacingNormals |
            aiProcess_FindInvalidData |
            aiProcess_ValidateDataStructure |
            0
            //aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
        );
        // check for errors
        if( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
            LOG_ERROR( "ASSIMP:: " + ( std::string )importer.GetErrorString() );
            return;
        }
        // retrieve the directory path of the filepath
        m_Directory = path.substr( 0, path.find_last_of( "/" ) );

        LOG_INFO( "Model:: Found: " + std::to_string( scene->mNumMeshes ) + " scene meshes."  );
        LOG_INFO( "Model:: Found: " + std::to_string( scene->mRootNode->mNumMeshes ) + " RootNode meshes." );
        LOG_INFO( "Model:: Found: " + std::to_string( scene->mRootNode->mNumChildren ) + " RootNode children." );

        // process ASSIMP's root node recursively
        processNode( scene->mRootNode, scene );
    }
    /* Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any). */
    void processNode( aiNode* node, const aiScene* scene ) {
        LOG_INFO( node->mName.C_Str() );
        // process each mesh located at the current node
        for( unsigned int i = 0; i < node->mNumMeshes; i++ ) {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back( processMesh( mesh, scene ) );
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for( unsigned int i = 0; i < node->mNumChildren; i++ ) {
            processNode( node->mChildren[i], scene );
        }
    }
    Mesh processMesh( aiMesh* mesh, const aiScene* scene ) {

        LOG_INFO( mesh->mName.C_Str() );

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture2D> textures;
        textures.reserve( MIN_TEX_COUNT );

        for( unsigned int i = 0; i < mesh->mNumVertices; i++ ) {
            Vertex vertex;
            glm::vec3 _vector3;
            _vector3.x = mesh->mVertices[i].x;
            _vector3.y = mesh->mVertices[i].y;
            _vector3.z = mesh->mVertices[i].z;
            vertex.Position = _vector3;
            if( mesh->HasNormals() ) {
                _vector3.x = mesh->mNormals[i].x;
                _vector3.y = mesh->mNormals[i].y;
                _vector3.z = mesh->mNormals[i].z;
                vertex.Normal = _vector3;
            }
            if( mesh->HasTextureCoords( 0 ) ) {
                glm::vec2 _vec2;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                _vec2.x = mesh->mTextureCoords[0][i].x;
                _vec2.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = _vec2;
                // tangent
                _vector3.x = mesh->mTangents[i].x;
                _vector3.y = mesh->mTangents[i].y;
                _vector3.z = mesh->mTangents[i].z;
                vertex.Tangent = _vector3;
                // bitangent
                _vector3.x = mesh->mBitangents[i].x;
                _vector3.y = mesh->mBitangents[i].y;
                _vector3.z = mesh->mBitangents[i].z;
                vertex.Bitangent = _vector3;
            } else {
                vertex.TexCoords = glm::vec2( 0.0f, 0.0f );
            }
            vertices.push_back( vertex );
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle)
        // and retrieve the corresponding vertex indices.
        for( unsigned int i = 0; i < mesh->mNumFaces; i++ ) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for( unsigned int j = 0; j < face.mNumIndices; j++ ) {
                indices.push_back( face.mIndices[j] );
            }
        }
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        loadMaterialTextures( material, aiTextureType_DIFFUSE, Texture2D::TYPE::DIFFUSE, textures );
        loadMaterialTextures( material, aiTextureType_SPECULAR, Texture2D::TYPE::SPECULAR, textures );
        loadMaterialTextures( material, aiTextureType_HEIGHT, Texture2D::TYPE::HEIGHT, textures );
        loadMaterialTextures( material, aiTextureType_AMBIENT, Texture2D::TYPE::AMBIENT, textures );

        //std::cout << "TEXTURES SIZE = " << textures.size() << std::endl;
        //std::cout << "TEXTURES_Loaded SIZE = " << m_TexturesLoaded.size() << std::endl;
        return Mesh( vertices, indices, textures );
    }
    /* Checks all material textures of a given type and loads the textures if they're not loaded yet. The required info is returned as a Texture struct. */
    void loadMaterialTextures( aiMaterial* mat, aiTextureType aiType, Texture2D::TYPE type, std::vector<Texture2D>& outTextures ) {
        for( unsigned int i = 0; i < mat->GetTextureCount( aiType ); i++ ) {
            aiString str;
            mat->GetTexture( aiType, i, &str );
            if( m_TLoaded.count( str.C_Str() ) > 0 ) {
                outTextures.push_back( *m_TLoaded[str.C_Str()] );
            } else {
                std::string fileName = m_Directory + '/' + str.data;
                Texture2D* texture = new Texture2D{ fileName.c_str(), type, true };
                m_TLoaded[str.C_Str()] = texture;
                outTextures.push_back( std::move( *texture ) );
            }
        }
    }
public:
    std::vector<Mesh>       m_Meshes;
private:
    std::vector<Texture2D>  m_TexturesLoaded;
    std::string             m_Directory;
    bool                    m_GammaCorrection;

    std::unordered_map<std::string, Texture2D*> m_TLoaded;
};
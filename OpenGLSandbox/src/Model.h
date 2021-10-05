#pragma once
#include <vector>
#include <unordered_map>
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include "Texture.h"
#include "Mesh.hpp"

class Renderer;
class Shader;

class Model {
public:
    Model();
    Model( std::string const& path, bool gamma = false );
    void Draw( Renderer& render, Shader& shader );
private:
    void loadModel( std::string const& path );
    void processNode( aiNode* node, const aiScene* scene );
    Mesh processMesh( aiMesh* mesh, const aiScene* scene );
    std::vector<Texture> loadMaterialTextures( aiMaterial* mat, aiTextureType aiType, Texture::TYPE type );
    void loadMaterialTextures( aiMaterial* mat, aiTextureType aiType, Texture::TYPE type, std::vector<Texture>& outTextures );
public:
    std::vector<Texture>    m_TexturesLoaded;
    std::unordered_map<std::string, Texture*> m_TLoaded;
    std::vector<Mesh>       m_Meshes;
    std::string             m_Directory;
    bool                    m_GammaCorrection;
};
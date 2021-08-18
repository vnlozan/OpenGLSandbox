#include <iostream>

#include "stb_image/stb_image.h"

#include "Model.h"

Model::Model( std::string const& path, bool gamma ) : m_GammaCorrection{ gamma } {}
void Model::Draw( Renderer& render, Shader& shader ) {
	for( Mesh& mesh : m_Meshes ) {
		mesh.Draw( render, shader );
	}
}
// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel( std::string const& path ) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );
    // check for errors
    if( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    m_Directory = path.substr( 0, path.find_last_of( "/" ) );
    // process ASSIMP's root node recursively
    processNode( scene->mRootNode, scene );
}
// Processes a node in a recursive fashion.
// Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode( aiNode* node, const aiScene* scene ) {
    // process each mesh located at the current node
    for( unsigned int i = 0; i < node->mNumMeshes; i++ )         {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back( processMesh( mesh, scene ) );
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for( unsigned int i = 0; i < node->mNumChildren; i++ )         {
        processNode( node->mChildren[i], scene );
    }
}
Mesh Model::processMesh( aiMesh* mesh, const aiScene* scene ) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for( unsigned int i = 0; i < mesh->mNumVertices; i++ ) {
        Vertex vertex;
        // we declare a placeholder vector since assimp uses its own vector class 
        // that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        if( mesh->HasNormals() ) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if( mesh->mTextureCoords[0] ) {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
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
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    std::vector<Texture> diffuseMaps = loadMaterialTextures( material, aiTextureType_DIFFUSE, Texture::TYPE::DIFFUSE );
    textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );

    std::vector<Texture> specularMaps = loadMaterialTextures( material, aiTextureType_SPECULAR, Texture::TYPE::SPECULAR );
    textures.insert( textures.end(), specularMaps.begin(), specularMaps.end() );

    std::vector<Texture> normalMaps = loadMaterialTextures( material, aiTextureType_HEIGHT, Texture::TYPE::HEIGHT );
    textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );

    std::vector<Texture> heightMaps = loadMaterialTextures( material, aiTextureType_AMBIENT, Texture::TYPE::AMBIENT );
    textures.insert( textures.end(), heightMaps.begin(), heightMaps.end() );

    return Mesh( vertices, indices, textures );
}
// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures( aiMaterial* mat, aiTextureType aiType, Texture::TYPE type ) {
    std::vector<Texture> textures;
    for( unsigned int i = 0; i < mat->GetTextureCount( aiType ); i++ ) {
        aiString str;
        mat->GetTexture( aiType, i, &str );
        // check if texture was loaded before and if so,
        // continue to next iteration: skip loading a new texture
        bool skip = false;
        for( unsigned int j = 0; j < m_TexturesLoaded.size(); j++ ) {
            if( std::strcmp( m_TexturesLoaded[j].m_FilePath.data(), str.C_Str() ) == 0 ) {
                textures.push_back( m_TexturesLoaded[j] );
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if( !skip ) {   // if texture hasn't been loaded already, load it
            std::string fileName = m_Directory + '/' + str.data;
            Texture texture{ fileName.c_str(), type };
            textures.push_back( texture );
            // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            m_TexturesLoaded.push_back( texture );
        }
    }
    return textures;
}
#include "pch.h"

#include "utilities/logger.h"
#include "utilities/stb_image.h"

#include "model.h"
#include "shader.h"
#include "mesh.h"

Model::Model(const std::string& path, bool gamma)
{
    m_gammaCorrection = gamma;
    loadModel(path);
}

void Model::draw(Shader* shader)
{
    for (uint32_t i = 0; i < m_meshes.size(); i++) {
        m_meshes[i].draw(shader);
    }
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR("LoadModel: Error - {}", importer.GetErrorString());
        return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture>  textures;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex    vertex;
        glm::vec3 vector;

        vector.x   = mesh->mVertices[i].x;
        vector.y   = mesh->mVertices[i].y;
        vector.z   = mesh->mVertices[i].z;
        vertex.pos = vector;

        if (mesh->HasNormals()) {
            vector.x      = mesh->mNormals[i].x;
            vector.y      = mesh->mNormals[i].y;
            vector.z      = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x            = mesh->mTextureCoords[0][i].x;
            vec.y            = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;

            vector.x       = mesh->mTangents[i].x;
            vector.y       = mesh->mTangents[i].y;
            vector.z       = mesh->mTangents[i].z;
            vertex.tangent = vector;

            vector.x         = mesh->mBitangents[i].x;
            vector.y         = mesh->mBitangents[i].y;
            vector.z         = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    Material  mat;
    aiColor3D col;
    float     value;

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, col) == AI_SUCCESS) {
        mat.diffuse = glm::vec3(col.r, col.g, col.b);
    }

    if (material->Get(AI_MATKEY_COLOR_SPECULAR, col) == AI_SUCCESS) {
        mat.specular = glm::vec3(col.r, col.g, col.b);
    }

    if (material->Get(AI_MATKEY_COLOR_AMBIENT, col) == AI_SUCCESS) {
        mat.ambient = glm::vec3(col.r, col.g, col.b);
    }

    if (material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
        mat.shininess = value;
    }

    if (material->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
        mat.transparency = value;
    }

    if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
        mat.metallic = value;
    }

    if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
        mat.roughness = value;
    }

    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, col) == AI_SUCCESS) {
        mat.emissive = glm::vec3(col.r, col.g, col.b);
    }

    // Material shit
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh(vertices, indices, textures, mat);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for (uint32_t j = 0; j < m_texturesLoaded.size(); j++) {
            if (std::strcmp(m_texturesLoaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(m_texturesLoaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.id   = textureFromFile(str.C_Str(), m_directory.c_str());
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            m_texturesLoaded.push_back(texture);
        }
    }
    return textures;
}

uint32_t Model::textureFromFile(const std::string& path, bool gamma)
{
    std::string fileName = std::string(path);
    fileName             = m_directory + '/' + fileName;

    uint32_t textureId;
    glGenTextures(1, &textureId);

    int            width, height, nrComponents;
    unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        LOG_ERROR("Texture failed to load at path: {}", path);
        stbi_image_free(data);
    }

    return textureId;
}
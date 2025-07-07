#include "pch.h"

#include "common/logger.h"
#include "common/stb_image.h"

#include "engine/renderer/geometry/model.h"
#include "engine/renderer/geometry/mesh.h"
#include "engine/renderer/shaders/shader.h"
#include "engine/renderer/resources/resource_manager.h"
#include "engine/renderer/resources/texture_resource.h"

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

    LOG_INFO("Finished loading model: {}", path);
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

    aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
    // LOG_INFO("Processing mesh: {} with material: {}", mesh->mName.C_Str(), aiMat->GetName().C_Str());

    Material mat = convertAiMaterialToPBR(aiMat);
    loadMaterialTextures(aiMat, mat, textures);

    // LOG_INFO("Mesh {} final material: A({:.2f},{:.2f},{:.2f}) M:{:.2f} R:{:.2f} Textures: A:{} M:{} R:{} N:{}", mesh->mName.C_Str(), mat.albedo.r, mat.albedo.g, mat.albedo.b, mat.metallic, mat.roughness,
    // mat.hasAlbedoTexture, mat.hasMetallicTexture, mat.hasRoughnessTexture, mat.hasNormalTexture);

    // LOG_INFO("Loaded mesh {}.", mesh->mName.C_Str());
    return Mesh(vertices, indices, textures, mat);
}

Material Model::convertAiMaterialToPBR(aiMaterial* aiMat)
{
    Material  mat;
    aiColor3D color;
    float     value;

    // Set better defaults for PBR
    mat.albedo       = glm::vec3(0.7f, 0.7f, 0.7f); // Lighter default
    mat.metallic     = 0.0f;
    mat.roughness    = 0.8f; // More diffuse by default
    mat.ao           = 1.0f;
    mat.emissive     = glm::vec3(0.0f);
    mat.transparency = 1.0f;

    // Try to get PBR properties first
    if (aiMat->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS) {
        mat.albedo = glm::vec3(color.r, color.g, color.b);
    } else if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        mat.albedo = glm::vec3(color.r, color.g, color.b);
        // Ensure albedo isn't too dark
        float brightness = (mat.albedo.r + mat.albedo.g + mat.albedo.b) / 3.0f;
        if (brightness < 0.1f) {
            mat.albedo = glm::vec3(0.5f, 0.5f, 0.5f); // Fallback to gray
        }
    }

    // Metallic factor
    if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
        mat.metallic = glm::clamp(value, 0.0f, 1.0f);
    } else if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        float specularIntensity = (color.r + color.g + color.b) / 3.0f;
        mat.metallic            = specularIntensity > 0.5f ? 0.1f : 0.0f; // Be conservative
    }

    // Roughness factor
    if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
        mat.roughness = glm::clamp(value, 0.01f, 1.0f);
    } else if (aiMat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS && value > 0) {
        // Convert shininess to roughness more conservatively
        mat.roughness = glm::clamp(1.0f - (value / 256.0f), 0.1f, 1.0f);
    }

    // Emissive
    if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
        mat.emissive = glm::vec3(color.r, color.g, color.b);
    }

    // Transparency
    if (aiMat->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
        mat.transparency = glm::clamp(value, 0.0f, 1.0f);
    }

    if (mat.albedo.r + mat.albedo.g + mat.albedo.b < 0.1f) {
        mat.albedo = glm::vec3(0.5f, 0.5f, 0.5f);
        LOG_INFO("Albedo is too low, setting to 0.5f, 0.5f, 0.5f");
    }

    if (mat.ao <= 0.0f) {
        mat.ao = 1.0f;
        LOG_INFO("AO is too low, setting to 1.0f");
    }

    // LOG_INFO("Final material - Albedo: ({:.2f}, {:.2f}, {:.2f}), Metallic: {:.2f}, Roughness: {:.2f}", mat.albedo.r, mat.albedo.g, mat.albedo.b, mat.metallic, mat.roughness);

    return mat;
}

void Model::loadMaterialTextures(aiMaterial* aiMat, Material& mat, std::vector<Texture>& textures)
{
    // Initialize all flags to false
    mat.hasAlbedoTexture    = false;
    mat.hasMetallicTexture  = false;
    mat.hasRoughnessTexture = false;
    mat.hasNormalTexture    = false;
    mat.hasAoTexture        = false;
    mat.hasEmissiveTexture  = false;
    mat.hasLegacyDiffuse    = false;
    mat.hasLegacySpecular   = false;

    // Try PBR textures first
    loadTextureType(aiMat, aiTextureType_BASE_COLOR, "texture_albedo", textures, mat.hasAlbedoTexture);
    loadTextureType(aiMat, aiTextureType_METALNESS, "texture_metallic", textures, mat.hasMetallicTexture);
    loadTextureType(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness", textures, mat.hasRoughnessTexture);
    loadTextureType(aiMat, aiTextureType_NORMALS, "texture_normal", textures, mat.hasNormalTexture);

    // Legacy fallbacks
    if (!mat.hasAlbedoTexture) {
        loadTextureType(aiMat, aiTextureType_DIFFUSE, "texture_albedo", textures, mat.hasAlbedoTexture);
        mat.hasLegacyDiffuse = mat.hasAlbedoTexture;
    }

    if (!mat.hasMetallicTexture && !mat.hasRoughnessTexture) {
        loadTextureType(aiMat, aiTextureType_SPECULAR, "texture_specular", textures, mat.hasLegacySpecular);
    }

    // Try alternative normal map types
    if (!mat.hasNormalTexture) {
        loadTextureType(aiMat, aiTextureType_HEIGHT, "texture_normal", textures, mat.hasNormalTexture);
    }

    // LOG_INFO("Texture summary - Albedo: {}, Metallic: {}, Roughness: {}, Normal: {}, LegacySpec: {}", mat.hasAlbedoTexture, mat.hasMetallicTexture, mat.hasRoughnessTexture, mat.hasNormalTexture,
    // mat.hasLegacySpecular);
}

void Model::loadTextureType(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& textures, bool& hasTexture)
{
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::string fullPath = m_directory + '/' + str.C_Str();

        // Check if we already loaded this texture
        bool found = false;
        for (const auto& texRes : m_textureResources) {
            if (texRes && texRes->getPath() == fullPath) {
                Texture texture;
                texture.id   = texRes->getTextureId();
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                hasTexture = true;
                found      = true;
                break;
            }
        }

        if (!found) {
            auto textureResource = RESOURCE_MANAGER.getTexture(fullPath);
            if (textureResource && textureResource->isLoaded()) {
                m_textureResources.push_back(textureResource);

                Texture texture;
                texture.id   = textureResource->getTextureId();
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                hasTexture = true;

                // LOG_INFO("Loaded texture via ResourceManager: {}", fullPath);
            } else {
                LOG_WARN("Failed to load texture: {}", fullPath);
            }
        }
    }
}

#ifndef ENGINE_RENDERER_MODEL_H_
#define ENGINE_RENDERER_MODEL_H_

#include <string>
#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "engine/renderer/geometry/mesh.h"

class Shader;
class TextureResource;

struct Texture;

class Model
{
  public:
    Model(const std::string& path, bool gamma = false);

    void              draw(Shader* shader);
    std::vector<Mesh> getMeshes() const { return m_meshes; }

  private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void loadMaterialTextures(aiMaterial* aiMat, Material& mat, std::vector<Texture>& textures);
    void loadTextureType(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& textures, bool& hasTexture);
    // uint32_t textureFromFile(const std::string& path, const std::string& directory);
    Material convertAiMaterialToPBR(aiMaterial* atMat);

    std::vector<std::shared_ptr<TextureResource>> m_textureResources;
    std::vector<Mesh>                             m_meshes;
    bool                                          m_gammaCorrection;
    std::string                                   m_directory;
};

#endif // ENGINE_RENDERER_MODEL_H_
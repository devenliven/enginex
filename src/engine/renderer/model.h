#ifndef ENGINE_RENDERER_MODEL_H_
#define ENGINE_RENDERER_MODEL_H_

#include <string>
#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

class Shader;

struct Texture;

class Model
{
  public:
    Model(const std::string& path, bool gamma = false);

    void draw(Shader* shader);

  private:
    void                 loadModel(const std::string& path);
    void                 processNode(aiNode* node, const aiScene* scene);
    Mesh                 processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    uint32_t             textureFromFile(const std::string& path, bool gamma);

    std::vector<Texture> m_texturesLoaded;
    std::vector<Mesh>    m_meshes;
    bool                 m_gammaCorrection;
    std::string          m_directory;
};

#endif // ENGINE_RENDERER_MODEL_H_
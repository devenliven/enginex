#ifndef ENGINE_RENDERER_MESH_H_
#define ENGINE_RENDERER_MESH_H_

#include "utilities/logger.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture {
    uint32_t    id;
    std::string type;
    std::string path;
};

struct Material {
    glm::vec3 diffuse      = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specular     = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 ambient      = glm::vec3(0.1f, 0.1f, 0.1f);
    float     shininess    = 32.0f;
    float     transparency = 1.0f;

    float     metallic  = 0.0f;
    float     roughness = 0.5f;
    glm::vec3 emissive  = glm::vec3(0.0f);
};

class Shader;

class Mesh
{
  public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures, Material material);
    ~Mesh() {}

    void            draw(Shader* shader);
    const Material& getMaterial() const { return m_material; }

  private:
    uint32_t m_vbo;
    uint32_t m_ebo;
    uint32_t m_vao;

    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Texture>  m_textures;
    Material              m_material;

    void setupMesh();
};

#endif // ENGINE_RENDERER_MESH_H_
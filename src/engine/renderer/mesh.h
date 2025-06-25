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

class Shader;

class Mesh
{
  public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
    ~Mesh() {}

    void draw(Shader* shader);

  private:
    uint32_t m_vbo;
    uint32_t m_ebo;
    uint32_t m_vao;

    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Texture>  m_textures;

    void setupMesh();
};

#endif // ENGINE_RENDERER_MESH_H_
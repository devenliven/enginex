#include "pch.h"

#include "mesh.h"

#include "engine/renderer/shaders/shader.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures, Material material)
{
    m_vertices = vertices;
    m_indices  = indices;
    m_textures = textures;
    m_material = material;

    setupMesh();
}

Mesh::~Mesh()
{
    // glDeleteVertexArrays(1, &m_vao);
    // glDeleteBuffers(1, &m_vbo);
    // glDeleteBuffers(1, &m_ebo);
}

void Mesh::draw(Shader* shader)
{
    shader->setVec3("material.albedo", m_material.albedo);
    shader->setFloat("material.metallic", m_material.metallic);
    shader->setFloat("material.roughness", m_material.roughness);
    shader->setFloat("material.ao", m_material.ao);
    shader->setVec3("material.emissive", m_material.emissive);
    shader->setFloat("material.transparency", m_material.transparency);

    shader->setBool("material.hasAlbedoTexture", m_material.hasAlbedoTexture);
    shader->setBool("material.hasMetallicTexture", m_material.hasMetallicTexture);
    shader->setBool("material.hasRoughnessTexture", m_material.hasRoughnessTexture);
    shader->setBool("material.hasNormalTexture", m_material.hasNormalTexture);
    shader->setBool("material.hasLegacySpecular", m_material.hasLegacySpecular);

    // Bind textures
    for (uint32_t i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(glGetUniformLocation(shader->getProgram(), (m_textures[i].type + "1").c_str()), i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(m_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), &m_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}
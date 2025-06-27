#include "pch.h"

#include "mesh.h"

#include "engine/renderer/shader.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures, Material material)
{
    m_vertices = vertices;
    m_indices  = indices;
    m_textures = textures;
    m_material = material;

    setupMesh();
}

void Mesh::draw(Shader* shader)
{
    if (!debugMessageSent) {
        LOG_DEBUG("Setting material diffuse: ({}, {}, {})", m_material.diffuse.x, m_material.diffuse.y, m_material.diffuse.z);
        LOG_DEBUG("Setting material ambient: ({}, {}, {})", m_material.ambient.x, m_material.ambient.y, m_material.ambient.z);
        LOG_DEBUG("Setting material specular: ({}, {}, {})", m_material.specular.x, m_material.specular.y, m_material.specular.z);
        LOG_DEBUG("Setting material shininess: {}", m_material.shininess);
        LOG_DEBUG("Setting material transparency: {}", m_material.transparency);
        debugMessageSent = true;
    }

    shader->setVec3("material.diffuse", m_material.diffuse);
    shader->setVec3("material.specular", m_material.specular);
    shader->setVec3("material.ambient", m_material.ambient);

    shader->setFloat("material.shininess", m_material.shininess);
    shader->setFloat("material.transparency", m_material.transparency);

    // LOG_DEBUG("Mesh has {} textures", m_textures.size());

    uint32_t diffuseNr = 1, specularNr = 1, normalNr = 1, heightNr = 1;

    for (uint32_t i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = m_textures[i].type;

        // LOG_DEBUG("Texture {}: type='{}', id={}", i, name, m_textures[i].id);

        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        } else if (name == "texture_normal") {
            number = std::to_string(normalNr++);
        } else if (name == "texture_height") {
            number = std::to_string(heightNr++);
        }

        std::string uniformName = name + number;
        // LOG_DEBUG("Setting uniform '{}' to texture unit {}", uniformName, i);

        glUniform1i(glGetUniformLocation(shader->getProgram(), uniformName.c_str()), i);
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
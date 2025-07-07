#include "pch.h"
#include "line_renderer.h"
#include "common/logger.h"

const char* LineRenderer::s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 viewProjection;

out vec3 vertexColor;

void main() {
    gl_Position = viewProjection * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

const char* LineRenderer::s_fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vertexColor, 1.0);
}
)";

LineRenderer::LineRenderer()
    : m_shaderProgram(0)
    , m_VAO(0)
    , m_VBO(0)
    , m_initialized(false)
{
}

LineRenderer::~LineRenderer()
{
    cleanup();
}

bool LineRenderer::initialize()
{
    if (m_initialized) {
        return true;
    }

    if (!createShaderProgram()) {
        LOG_ERROR("Failed to create line renderer shader program");
        return false;
    }

    setupGeometry();

    m_initialized = true;
    LOG_INFO("LineRenderer initialized successfully");
    return true;
}

void LineRenderer::cleanup()
{
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }

    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

    if (m_shaderProgram) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }

    m_initialized = false;
}

bool LineRenderer::compileShader(uint32_t& shader, const char* source, uint32_t type)
{
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG_ERROR("Shader compilation failed: {}", infoLog);
        return false;
    }

    return true;
}

bool LineRenderer::createShaderProgram()
{
    uint32_t vertexShader, fragmentShader;

    if (!compileShader(vertexShader, s_vertexShaderSource, GL_VERTEX_SHADER)) {
        return false;
    }

    if (!compileShader(fragmentShader, s_fragmentShaderSource, GL_FRAGMENT_SHADER)) {
        glDeleteShader(vertexShader);
        return false;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    int success;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        LOG_ERROR("Shader program linking failed: {}", infoLog);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void LineRenderer::setupGeometry()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void LineRenderer::beginFrame(const glm::mat4& viewProjection)
{
    m_viewProjection = viewProjection;
    m_lines.clear();
}

void LineRenderer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
{
    m_lines.push_back({start, end, color});
}

void LineRenderer::drawLightLines(const glm::vec3& modelCenter, const std::vector<glm::vec3>& lightPositions, const std::vector<glm::vec3>& lightColors)
{
    for (size_t i = 0; i < lightPositions.size(); ++i) {
        glm::vec3 color = (i < lightColors.size()) ? lightColors[i] : glm::vec3(1.0f, 1.0f, 1.0f);
        drawLine(modelCenter, lightPositions[i], color);
    }
}

void LineRenderer::endFrame()
{
    if (!m_initialized || m_lines.empty()) {
        return;
    }

    std::vector<float> vertices;
    vertices.reserve(m_lines.size() * 12);

    for (const auto& line : m_lines) {

        vertices.push_back(line.start.x);
        vertices.push_back(line.start.y);
        vertices.push_back(line.start.z);
        vertices.push_back(line.color.r);
        vertices.push_back(line.color.g);
        vertices.push_back(line.color.b);

        vertices.push_back(line.end.x);
        vertices.push_back(line.end.y);
        vertices.push_back(line.end.z);
        vertices.push_back(line.color.r);
        vertices.push_back(line.color.g);
        vertices.push_back(line.color.b);
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glUseProgram(m_shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "viewProjection"), 1, GL_FALSE, glm::value_ptr(m_viewProjection));

    bool depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (depthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }

    glLineWidth(2.0f);

    glDrawArrays(GL_LINES, 0, (GLsizei)m_lines.size() * 2);

    glLineWidth(1.0f);

    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }

    glBindVertexArray(0);
}
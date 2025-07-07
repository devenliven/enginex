#include "pch.h"

#include "shader.h"

#include "common/file.h"
#include "common/logger.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    m_vertexShader = compile(vertexPath);
    if (!m_vertexShader) {
        m_vertexShader = 0;
        LOG_ERROR("Failed to compile vertex shader - {}", vertexPath);
    }

    m_fragmentShader = compile(fragmentPath);
    if (!m_fragmentShader) {
        m_fragmentShader = 0;
        LOG_ERROR("Failed to compile fragment shader - {}", fragmentPath);
    }

    createProgram();

    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    LOG_INFO("Shaders {} and {} have been compiled and linked!", vertexPath, fragmentPath);
}

Shader::~Shader()
{
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

uint32_t Shader::compile(const std::string& shaderPath)
{
    std::string path               = std::format("assets/shaders/{}", shaderPath);
    auto        shaderSource       = FileSystem::readFileToString(path);
    auto        shaderTypeFriendly = path.ends_with(".fs") ? "fragment" : "vertex";

    if (!shaderSource) {
        LOG_ERROR("Failed to read {} shader!", shaderTypeFriendly);
        return 0;
    }

    auto shaderType = path.ends_with(".fs") ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;

    const char* source = shaderSource->c_str();
    uint32_t    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    if (!validate(shader, "SHADER", shaderTypeFriendly)) {
        return 0;
    }

    return shader;
}

bool Shader::createProgram()
{
    if (m_fragmentShader == 0 || m_vertexShader == 0) {
        LOG_ERROR("Failed to create shader program. Fragment or Vertex is invalid!");
        return false;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, m_vertexShader);
    glAttachShader(m_program, m_fragmentShader);
    glLinkProgram(m_program);

    if (!validate(m_program, "PROGRAM")) {
        return false;
    }
    return true;
}

bool Shader::validate(uint32_t shader, std::string errorType, const std::string& shaderTypeFriendly)
{
    int  success = 0;
    char infoLog[512];

    if (errorType == "SHADER") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            LOG_ERROR("Failed to compile {} shader! - Error: {}", shaderTypeFriendly, infoLog);
            glDeleteShader(shader);
            return false;
        }
    } else if (errorType == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            LOG_ERROR("Failed to create shader program! Error - {}", infoLog);
            glDeleteProgram(shader);
            return false;
        }
    }

    return true;
}

int Shader::getUniformLocation(const std::string& name) const
{
    auto it = m_uniformLocationCache.find(name);
    if (it != m_uniformLocationCache.end()) {
        return it->second;
    }

    int location                 = glGetUniformLocation(m_program, name.c_str());
    m_uniformLocationCache[name] = location;

    if (location == -1) {
        LOG_WARN("Uniform '{}' not found in shader", name);
    }

    return location;
}
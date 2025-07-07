#ifndef SHADER_H_
#define SHADER_H_

#include <stdint.h>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
  public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();

    uint32_t getProgram() const { return m_program; }

    void use()
    {
        if (!m_program) {
            return;
        }

        glUseProgram(m_program);
    }

    // clang-format off
    void setBool(const std::string& name, bool value) const { 
      glUniform1i(getUniformLocation(name.c_str()), (int)value); 
    }
    
    void setInt(const std::string& name, int value) const { 
      glUniform1i(getUniformLocation(name.c_str()), value); 
    }

    void setFloat(const std::string& name, float value) const { 
      glUniform1f(getUniformLocation(name.c_str()), value); 
    }

    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(getUniformLocation(name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(getUniformLocation(name.c_str()), x, y); 
    }
    
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(getUniformLocation(name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(getUniformLocation(name.c_str()), x, y, z); 
    }
    
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(getUniformLocation(name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(getUniformLocation(name.c_str()), x, y, z, w); 
    }
    
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // clang-format on

  private:
    uint32_t m_vertexShader   = 0;
    uint32_t m_fragmentShader = 0;
    uint32_t m_program        = 0;

    mutable std::unordered_map<std::string, int> m_uniformLocationCache;

    uint32_t compile(const std::string &shaderPath);
    bool     createProgram();
    bool     validate(uint32_t shader, std::string errorType, const std::string &shaderTypeFriendly = "");
    int      getUniformLocation(const std::string &name) const;
};

#endif // SHADER_H_
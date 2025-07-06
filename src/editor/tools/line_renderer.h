#ifndef EDITOR_TOOLS_LINE_RENDERER_H_
#define EDITOR_TOOLS_LINE_RENDERER_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct DebugLine {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 color;
};

class LineRenderer
{
  public:
    LineRenderer();
    ~LineRenderer();

    bool initialize();
    void cleanup();

    void beginFrame(const glm::mat4& viewProjection);
    void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    void drawLightLines(const glm::vec3& modelCenter, const std::vector<glm::vec3>& lightPositions, const std::vector<glm::vec3>& lightColors);
    void endFrame();

  private:
    bool compileShader(uint32_t& shader, const char* source, uint32_t type);
    bool createShaderProgram();
    void setupGeometry();

    uint32_t  m_shaderProgram;
    uint32_t  m_VAO, m_VBO;
    glm::mat4 m_viewProjection;

    std::vector<DebugLine> m_lines;
    bool                   m_initialized;

    static const char* s_vertexShaderSource;
    static const char* s_fragmentShaderSource;
};

#endif // EDITOR_TOOLS_LINE_RENDERER_H_
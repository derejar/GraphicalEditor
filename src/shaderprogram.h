#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    GLint getUniformLocation(std::string name) const;
    GLuint getDescriptor() const;
private:
    std::string readWholeFile(const std::string& path) const;
    GLuint compileShader(const GLenum& shaderType, const std::string& shaderCode) const;
private:
    GLuint m_descriptor;
};

#endif

#include "shaderprogram.h"
#include <fstream>
#include <sstream>
#include <exception>

ShaderProgram::ShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
    std::string vertexShaderCode = readWholeFile(vertexShaderFile);
    GLuint vertexShaderDescriptor = compileShader(GL_VERTEX_SHADER, vertexShaderCode);

    std::string fragmentShaderCode = readWholeFile(fragmentShaderFile);
    GLuint fragmentShaderDescriptor = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    m_descriptor = glCreateProgram();
    glAttachShader(m_descriptor, vertexShaderDescriptor);
    glAttachShader(m_descriptor, fragmentShaderDescriptor);
    glLinkProgram(m_descriptor);

    glDeleteShader(vertexShaderDescriptor);
    glDeleteShader(fragmentShaderDescriptor);
}

std::string ShaderProgram::readWholeFile(const std::string& path) const
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderProgram::compileShader(const GLenum& shaderType, const std::string& shaderCode) const
{
    GLuint shaderDescriptor = glCreateShader(shaderType);
    const char* source = shaderCode.c_str();
    glShaderSource(shaderDescriptor, 1, &source, nullptr);
    glCompileShader(shaderDescriptor);
    GLint successCompilation;
    glGetShaderiv(shaderDescriptor, GL_COMPILE_STATUS, &successCompilation);
    if(!successCompilation)
    {
        GLchar log[1024];
        glGetShaderInfoLog(shaderDescriptor, 1024, nullptr, log);
        throw std::runtime_error(log);
    }
    return shaderDescriptor;
}

GLint ShaderProgram::getUniformLocation(std::string name) const
{
    return 0; //Its just a stub
}

GLuint ShaderProgram::getDescriptor() const
{
    return m_descriptor;
}


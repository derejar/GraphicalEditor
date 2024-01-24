#include "logger.h"

#include <chrono>
#include <sstream>
#include <iomanip>
#include <exception>
#include <vector>

Logger Logger::m_instance   ("E:\\C++\\Ori\\GraphicalEditor\\logs\\Log");

Logger::Logger(std::string_view fileName)
{
    std::stringstream ss;
    ss << fileName << "_" << getCurrentTime("%d-%m-%y__%H-%M") << ".log";
    m_fileName = ss.str();
}

Logger::~Logger()
{}

Logger& Logger::getInstance()
{
    return m_instance;
}

void Logger::bufferData(GLenum target, std::size_t offset, std::size_t size, std::size_t countInLine)
{
    std::ofstream file(m_fileName, std::ios::app | std::ios::out);
    if(!file.is_open())
        throw std::runtime_error("File for logs didnt opened!");

    std::vector<float> buffer(size);
    glGetBufferSubData(target, offset, size*sizeof(std::vector<float>::value_type), buffer.data());
    file << getCurrentTime("%H:%M:%S") << ":\n";
    switch(target)
    {
        case GL_ARRAY_BUFFER: file << "GL_ARRAY_BUFFER data:\n"; break;
        default: file << "Unknown buffer data:\n"; break;
    }
    file.close();
    writeVector(buffer, countInLine);
}

void Logger::namedBufferData(GLuint bufferName, std::size_t offset, std::size_t size, std::size_t countInLine)
{
    std::ofstream file(m_fileName, std::ios::app | std::ios::out);
    if(!file.is_open())
        throw std::runtime_error("File for logs didnt opened!");

    std::vector<float> buffer(size);
    glGetNamedBufferSubData(bufferName, offset, size*sizeof(std::vector<float>::value_type), buffer.data());
    file << getCurrentTime("%H:%M:%S") << ":\n";
    file.close();
    writeVector(buffer, countInLine);
}

void Logger::writeVector(const std::vector<float>& vector, std::size_t countInLine)
{
    std::ofstream file(m_fileName, std::ios::app | std::ios::out);
    if(!file.is_open())
        throw std::runtime_error("File for logs didnt opened!");
    for(std::size_t i = 0; i < vector.size() / countInLine; ++i)
    {
        for(std::size_t j = 0; j < countInLine; ++j)
            file << std::setw(10) << std::setfill(' ') << vector[i*countInLine + j] << " ";
        file << "\n";
    }
}

std::string Logger::getCurrentTime(std::string_view format) const
{
    std::time_t t = std::chrono::high_resolution_clock::to_time_t(std::chrono::high_resolution_clock::now());
    std::tm time = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&time, format.data());
    return ss.str();
}

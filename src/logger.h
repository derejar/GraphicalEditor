#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <GL/glew.h>
#include <string_view>
#include <string>
#include <fstream>
#include <vector>

class Logger
{
public:
    static Logger& getInstance();
    void bufferData(GLenum target, std::size_t offset, std::size_t size, std::size_t countInLine);
    void namedBufferData(GLuint bufferName, std::size_t offset, std::size_t size, std::size_t countInLine);
    void writeVector(const std::vector<float>& vector, std::size_t countInLine);
    template <typename T> void write(const T& data);
    ~Logger();
protected:
    Logger(std::string_view fileName);
private:
    std::string getCurrentTime(std::string_view format) const;
private:
    std::string m_fileName;
    static Logger m_instance;
};

template <typename T>
void Logger::write(const T& data)
{
    std::ofstream file(m_fileName, std::ios::app | std::ios::out);
    if(!file.is_open())
        throw std::runtime_error("File for logs didnt opened");

    file << getCurrentTime("%H:%M:%S") << " - " << data << "\n";
    // file << data;
}



#endif

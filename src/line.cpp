#include "line.h"
#include "canvas.h"

#include <array>
#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include "logger.h"

ShaderProgram* Line::m_shaderProgram = nullptr;
int Line::m_count = 0;

Line::Line(glm::vec3 start, glm::vec3 end, const Canvas& canvas)
    : m_start(start), m_end(end), m_state(0), m_canvas(canvas)
{
    std::array<float, 12> vertices{
        m_start.x, m_start.y, m_start.z, 1.0f, 0.5f, 0.2f,
        m_end.x, m_end.y, m_end.z, 1.0f, 0.5f, 0.2f
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(vertices)::value_type)*vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    if(m_count == 0)
        m_shaderProgram = new ShaderProgram("E:\\C++\\Ori\\GraphicalEditor\\shaders\\vertex.glsl",
                                            "E:\\C++\\Ori\\GraphicalEditor\\shaders\\fragment.glsl");
    ++m_count;
    myNumber = m_count;
}

Line::~Line()
{
    --m_count;
    if(m_count == 0)
        delete m_shaderProgram;
}

#include <iostream>
void Line::setEnd(glm::vec3 end)
{
    m_end = end;
    std::array<float, 12> vertices{
        m_start.x, m_start.y, m_start.z, 1.0f, 0.5f, 0.2f,
        m_end.x, m_end.y, m_end.z, 1.0f, 0.5f, 0.2f
    };
    glNamedBufferSubData(m_vbo, 0, sizeof(vertices), vertices.data());

}

void Line::changeToIndexes(unsigned int start, unsigned int end)
{
    m_startIndex = start;
    m_endIndex = end;
    glBindVertexArray(m_vao2);
    glBindBuffer(GL_ARRAY_BUFFER,  m_canvas.getVBO());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_ibo);
    std::array<unsigned int, 2> indexes({start, end});
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size(), indexes.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &m_vao3);
    glBindVertexArray(m_vao3);
    glBindBuffer(GL_ARRAY_BUFFER,  m_canvas.getVBO());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)(6*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // glDeleteBuffers(1, &m_vbo);
    m_state = 1;
}

void Line::draw()
{
    glUseProgram(m_shaderProgram->getDescriptor());
    if(m_state == 0)
    {
        glBindVertexArray(m_vao);
        glDrawArrays(GL_LINES, 0, 2);
    }
    else if(m_state == 1 && !m_isSelected)
    {
        glBindVertexArray(m_vao2);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    }
    else if(m_state == 1 && m_isSelected)
    {
        glBindVertexArray(m_vao3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

#ifndef _LINE_H_
#define _LINE_H_

#include <glm/glm.hpp>
#include <utility>

#include "shaderprogram.h"
class Canvas;

class Line
{
public:
    Line(glm::vec3 start, glm::vec3 end, const Canvas& canvas);
    ~Line();
    void setEnd(glm::vec3 end);
    void changeToIndexes(unsigned int start, unsigned int end);
    void draw();
    std::pair<glm::vec3, glm::vec3> getPoints()
    {
        return std::make_pair(m_start, m_end);
    }
    std::pair<unsigned int, unsigned int> getIndexes()
    {
        return std::make_pair(m_startIndex, m_endIndex);
    }
    void setSelected(bool selection)
    {
        m_isSelected = selection;
    }
private:
    glm::vec3 m_start;
    glm::vec3 m_end;
    unsigned int m_vbo;
    unsigned int m_vao;
    static ShaderProgram* m_shaderProgram;
    static int m_count;

    int m_state;
    unsigned int m_ibo;
    unsigned int m_startIndex;
    unsigned int m_endIndex;

    unsigned int m_vao2;
    int myNumber;

    unsigned int m_vao3;
    bool m_isSelected = false;

    const Canvas& m_canvas;
};



#endif

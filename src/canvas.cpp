#include <exception>
#include <vector>
#include <iostream>
#include <memory>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "canvas.h"
#include "shaderprogram.h"
#include "logger.h"

Canvas::Canvas(wxWindow* parent,
               wxWindowID id,
               const int* attributes,
               const wxPoint& pos,
               const wxSize& size,
               int style,
               const wxString& name,
               const wxPalette& palette)
    : wxGLCanvas(parent, id, attributes, pos, size, style, name, palette),
    m_vbo(0), m_vao(0), m_context(std::make_unique<wxGLContext>(this))
{
    Bind(wxEVT_PAINT, &Canvas::onPaint, this);
    Bind(wxEVT_MOTION, &Canvas::onMouseMotion, this);
    Bind(wxEVT_LEFT_UP, &Canvas::onLMBclicked, this);
    Bind(wxEVT_LEFT_DCLICK, &Canvas::onDLMBclicked, this);
    Bind(wxEVT_LEFT_DOWN, &Canvas::onLMBDown, this);
    Bind(wxEVT_KEY_DOWN, &Canvas::onKeyDown, this);
    Bind(wxEVT_KEY_UP, &Canvas::onKeyUp, this);
    SetCurrent(*m_context);
    GLenum glewError = glewInit();
    int height, width;
    GetClientSize(&width, &height);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glViewport(0, 0, width, height);
    std::cout << "Width: " << width << " Height: " << height << "\n";
    ShaderProgram shaderProgram("E:\\C++\\Ori\\GraphicalEditor\\shaders\\vertex.glsl",
                                "E:\\C++\\Ori\\GraphicalEditor\\shaders\\fragment.glsl");
    glUseProgram(shaderProgram.getDescriptor());
    m_sp = shaderProgram.getDescriptor();
    if(glewError != GLEW_OK)
    {
        const char* errorMessage = reinterpret_cast<const char*>(glewGetErrorString(glewError));
        throw std::runtime_error(errorMessage);
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glGenVertexArrays(1, &m_vaoPoints);
    glBindVertexArray(m_vaoPoints);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)(6*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glGenVertexArrays(1, &m_vao2);
    glBindVertexArray(m_vao2);

    glGenBuffers(1, &m_vbo2);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
    std::vector<glm::vec3> axis;
    glm::vec3 color({0, 0, 0});
    axis.push_back({-width/2, 0, 0});
    axis.push_back(color);
    axis.push_back({width/2, 0, 0});
    axis.push_back(color);
    axis.push_back({0, -height/2, 0});
    axis.push_back(color);
    axis.push_back({0, height/2, 0});
    axis.push_back(color);
    glBufferData(GL_ARRAY_BUFFER, axis.size()*sizeof(glm::vec3), axis.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glLineWidth(3);
    // glPointSize(8);

    // Line* line1 = new Line({0,0,0}, {0, height/2.0f, 0}, *this);
    // m_lines.push_back(line1);
}

Canvas::~Canvas()
{
    SetCurrent(*m_context);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}


void Canvas::onKeyDown(wxKeyEvent& event)
{
    Logger logger = Logger::getInstance();
    logger.write("Key down - " + event.GetUnicodeKey());
    if(event.GetKeyCode() == WXK_SHIFT && state == 0)
    {
        state = 2;
        logger.write("Shift is down");
    }
    else if(event.GetKeyCode() == WXK_ALT && state == 0)
    {
        m_altHolded = true;
    }
}

void Canvas::onKeyUp(wxKeyEvent& event)
{
    Logger logger = Logger::getInstance();
    logger.write("Key up - " + event.GetUnicodeKey());
    if(event.GetKeyCode() == WXK_SHIFT && state == 2)
    {
        state = 0;
        logger.write("Shift in up");
    }
    else if(event.GetKeyCode() == WXK_ALT && (state == 0 || state == 1))
    {
        m_altHolded = false;
    }
    else if(event.GetKeyCode() == WXK_ESCAPE && !m_selected.empty())
    {
        for(std::size_t& index : m_selected)
        {
            m_pointColors[index] = {1.0f, 0.5f, 0.2f};
        }
        for(Line*& line : m_lines)
        {
            line->setSelected(false);
        }
        m_selected.clear();
        update();
    }
    else if(event.GetKeyCode() == WXK_DELETE && !m_selected.empty())
    {
        // for(std::size_t i = 0; i < m_lines.size(); ++i)
        // {
        //     auto indexes = m_lines[i]->getIndexes();
        //     for(std::size_t index : m_selected)
        //     {
        //         if(index == indexes.first || index == indexes.second)
        //         {
        //             delete m_lines[i];
        //             m_lines.erase(m_lines.begin() + i);
        //             --i;
        //         }
        //     }
        // }
        for(std::size_t i = 0; i < m_selected.size(); ++i)
        {
            for(std::size_t j = 0; j < m_lines.size(); ++j)
            {
                if(m_selected[i] == m_lines[j]->getIndexes().first || m_selected[i] == m_lines[j]->getIndexes().second)
                {
                    delete m_lines[j];
                    m_lines.erase(m_lines.begin() + j);
                    --j;
                }
            }
        }

        for(std::size_t index : m_selected)
            m_points[index] = {-10000, -10000, -10000};
        // for(std::size_t i = 0; i < m_points.size(); ++i)
        // {
        //     if(m_points[i] == glm::vec3(-1, -1, -1))
        //     {
        //         std::cout << "deleted\n";
        //         m_points.erase(m_points.begin() + i);
        //         m_colors.erase(m_colors.begin() + i);
        //         m_pointColors.erase(m_pointColors.begin() + i);
        //         --i;
        //         ++pointsRemoved;
        //     }
        // }
        m_selected.clear();
        update();
    }
}

glm::vec3 Canvas::translatePoint(glm::vec3 point)
{
    wxSize size = GetClientSize();
    point.x = point.x - size.GetWidth() / 2;
    point.y = -point.y + size.GetHeight() / 2;
    return point;
}

void Canvas::onMouseMotion(wxMouseEvent& event)
{
    // std::cout << "x=" << event.GetPosition().x << " y=" << event.GetPosition().y << "\n";
    glm::vec3 position = translatePoint({event.GetX(), event.GetY(), 0});
    std::string text = "x=" + std::to_string(position.x) + "; y=" + std::to_string(position.y);
    std::vector<glm::vec3> points;
    for(std::size_t i = 0; i < m_points.size(); ++i)
    {
        if((m_points[i].x >= position.x - 2 && m_points[i].x <= position.x + 2) &&
           (m_points[i].y >= position.y - 2 && m_points[i].y <= position.y + 2))
        {
            points.push_back(m_points[i]);
        }
    }
    auto it = std::max_element(points.begin(), points.end(), [](glm::vec3& a, glm::vec3& b){return a.z < b.z;});
    if(!points.empty())
        text += "; z=" + std::to_string(std::round(it->z));
    m_statusBar->SetStatusText(text);
    if(state == 1)
    {m_line->setEnd(position);}
}

void Canvas::onLMBDown(wxMouseEvent& event)
{
    if(state == 2)
    {
        std::cout << "LMB down here: " << event.GetX() << " " << event.GetY() << "\n";
        state = 3;
        glm::vec3 position = translatePoint({event.GetPosition().x, event.GetPosition().y, 0});
        m_start = wxPoint(position.x, position.y);
    }
}

void Canvas::onLMBclicked(wxMouseEvent& event)
{
    glm::vec3 position = translatePoint({event.GetX(), event.GetY(), 0});
    std::cout << "LBM clicked here: x=" << position.x << " y=" << position.y << "\n";
    if(state == 0)
    {m_line = new Line({position.x, position.y, 0}, {position.x, position.y, 0}, *this); ++state;}
    else if(state == 1)
    {
        std::pair<glm::vec3, glm::vec3> points = m_line->getPoints();
        unsigned int startIndex = addPoint(points.first);
        unsigned int endIndex = addPoint(points.second);
        m_line->changeToIndexes(startIndex, endIndex);
        m_lines.push_back(m_line);
        update();
        state = 0;
    }
    else if(state == 3)
    {
        std::cout << "LMB up here: " << event.GetX() << " " << event.GetY() << "\n";
        state = 0;
        m_end = wxPoint(position.x, position.y);
        wxPoint lower(std::min(m_start.x, m_end.x), std::min(m_start.y, m_end.y));
        wxPoint upper(std::max(m_start.x, m_end.x), std::max(m_start.y, m_end.y));
        for(std::size_t i = 0; i < m_points.size(); ++i)
        {
            if((m_points[i].x >= lower.x && m_points[i].x <= upper.x) &&
               (m_points[i].y >= lower.y && m_points[i].y <= upper.y))
            {
                bool isFind = false;
                for(std::size_t index : m_selected)
                {
                    if(index == i)
                    {
                        isFind = true;
                        break;
                    }
                }
                if(!isFind)
                    m_selected.push_back(i);
            }
        }
        for(std::size_t index : m_selected)
            m_pointColors[index] = {0.9, 0.1, 0.9};

        for(Line*& line : m_lines)
        {
            bool first = false; bool second = false;
            auto indexes = line->getIndexes();
            for(std::size_t index : m_selected)
            {
                if(index == indexes.first)
                    first = true;
                if(index == indexes.second)
                    second = true;
                if(first && second)
                    break;
            }
            if(first && second)
            {
                line->setSelected(true);
            }
        }

        update();
        Logger logger = Logger::getInstance();
        for(std::size_t i = 0; i < m_selected.size(); ++i)
            logger.write(std::to_string(i) + " :" + std::to_string(m_selected[i]));
    }
    Logger logger = Logger::getInstance();
    logger.write("LMB clicked, state=" + std::to_string(state));
}

void Canvas::onDLMBclicked(wxMouseEvent& event)
{
    glm::vec3 position = translatePoint({event.GetX(), event.GetY(), 0});
    std::cout << "double LBM clicked here: x=" << position.x << " y=" << position.y << "\n";
}

void Canvas::onPaint(wxPaintEvent& event)
{
    std::cout << "onPaint!\n";
    int height, width;
    GetClientSize(&width, &height);
    glViewport(0, 0, width, height);
    draw();
}

void Canvas::draw()
{
    SetCurrent(*m_context);
    glClearColor(0.2, 0.3, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glm::mat4 projection = glm::mat4(1.0f);
    int width, height;
    GetClientSize(&width, &height);
    glm::mat4 projection = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -1000.0f, 1000.0f);
    GLint projloc = glGetUniformLocation(m_sp, "projection");
    glUniformMatrix4fv(projloc, 1, GL_FALSE, glm::value_ptr(projection));

    if(state == 1)
        m_line->draw();
    for(Line* line : m_lines)
        line->draw();
    drawPoints();
    glLineWidth(1);
    glBindVertexArray(m_vao2);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
    glLineWidth(3);
    glFlush();
    SwapBuffers();
}

void Canvas::drawPoints()
{
    glBindVertexArray(m_vaoPoints);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glDrawArrays(GL_POINTS, 0, m_points.size());
    glBindVertexArray(0);
}

void Canvas::update()
{
    int height, width;
    GetClientSize(&width, &height);
    std::vector<glm::vec3> axis;
    glm::vec3 color({0, 0, 0});
    axis.push_back({-width/2, 0, 0});
    axis.push_back(color);
    axis.push_back({width/2, 0, 0});
    axis.push_back(color);
    axis.push_back({0, -height/2, 0});
    axis.push_back(color);
    axis.push_back({0, height/2, 0});
    axis.push_back(color);
    glNamedBufferData(m_vbo2, axis.size()*sizeof(glm::vec3), axis.data(), GL_DYNAMIC_DRAW);
    std::vector<float> vertices;
    vertices.reserve(m_points.size()*9);
    for(std::size_t i = 0; i < m_points.size(); ++i)
    {
        vertices.push_back(m_points[i].x);
        vertices.push_back(m_points[i].y);
        vertices.push_back(m_points[i].z);
        vertices.push_back(m_colors[i].r);
        vertices.push_back(m_colors[i].g);
        vertices.push_back(m_colors[i].b);
        vertices.push_back(m_pointColors[i].r);
        vertices.push_back(m_pointColors[i].g);
        vertices.push_back(m_pointColors[i].b);
    }
    glNamedBufferData(m_vbo, vertices.size()*sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
}

void Canvas::transfer(const float x, const float y, const float z)
{
    std::cout << "\n\nTRANSFER\n";
    std::cout << "cnt=" << m_selected.size() << "\n";
    std::cout << "x=" << x << "; y=" << y << "; z=" << z << "\n";
    for(std::size_t i = 0; i < m_selected.size(); ++i)
    {
        glm::vec4 point(m_points[m_selected[i]], 1.0);
        std::cout << "Before:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
        glm::mat4 transfer(1.0f);
        transfer = glm::translate(transfer, glm::vec3(x, y, z));
        point = transfer * point;
        std::cout << "After:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n\n";
        m_points[m_selected[i]] = point;
    }
    update();
}

float degreesToRadians(const float angle)
{
    const double pi = 3.141592;
    return angle*pi / 180.0;
}

float radiansToDegrees(const float r)
{
  const double pi = 3.141592;
  return r * 180.0 / pi;
}

// glm::mat4 getRotateMatrix(const float angle, const glm::vec3 axis)
// {
//     if(axis.x >= 0.99)
//     {
//         glm::mat4 rotate(
//             {1, 0, 0, 0},
//             {0, std::cos(degreesToRadians(angle)), -std::sin(degreesToRadians(angle)), 0},
//             {0, std::sin(degreesToRadians(angle)), std::cos(degreesToRadians(angle)), 0},
//             {0, 0, 0, 1}
//         );
//         return rotate;
//     }
//     else if(axis.y >= 0.99)
//     {
//         glm::mat4 rotate(
//             {std::cos(degreesToRadians(angle)), 0, std::sin(degreesToRadians(angle)), 0},
//             {0, 1, 0, 0},
//             {-std::sin(degreesToRadians(angle)), 0, std::cos(degreesToRadians(angle)), 0},
//             {0, 0, 0, 1}
//         );
//         return rotate;
//     }
//     else if(axis.z >= 0.99)
//     {
//         glm::mat4 rotate(
//             {std::cos(degreesToRadians(angle)), -std::sin(degreesToRadians(angle)), 0, 0},
//             {std::sin(degreesToRadians(angle)), std::cos(degreesToRadians(angle)) , 0, 0},
//             {0, 0, 1, 0},
//             {0, 0, 0, 1}
//         );
//         return rotate;
//     }
//     return glm::mat4(1.0);
// }

glm::mat4 getRotateMatrix(const float angle, const glm::vec3 axis)
{
    float angleRad = degreesToRadians(angle);
    if(axis.x >= 0.99)
    {
        glm::mat4 rotate(
            {1, 0, 0, 0},
            {0, std::cos(angleRad), std::sin(angleRad), 0},
            {0, -std::sin(angleRad), std::cos(angleRad), 0},
            {0, 0, 0, 1}
        );
        return rotate;
    }
    else if(axis.y >= 0.99)
    {
        glm::mat4 rotate(
            {std::cos(angleRad), 0, -std::sin(angleRad), 0},
            {0, 1, 0, 0},
            {std::sin(angleRad), 0, std::cos(angleRad), 0},
            {0, 0, 0, 1}
        );
        return rotate;
    }
    else if(axis.z >= 0.99)
    {
        glm::mat4 rotate(
            {std::cos(angleRad), std::sin(angleRad), 0, 0},
            {-std::sin(angleRad), std::cos(angleRad), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        );
        return rotate;
    }
    return glm::mat4(1.0);
}

std::tuple<float, float, float> Canvas::getAvg() const
{
    std::vector<glm::vec3> selectedPoints;
    for(std::size_t index : m_selected)
    {
        selectedPoints.push_back(m_points[index]);
    }
    auto pairX = std::minmax_element(selectedPoints.begin(), selectedPoints.end(), [](const glm::vec3& a, const glm::vec3& b){return a.x < b.x;});
    auto pairY = std::minmax_element(selectedPoints.begin(), selectedPoints.end(), [](const glm::vec3& a, const glm::vec3& b){return a.y < b.y;});
    auto pairZ = std::minmax_element(selectedPoints.begin(), selectedPoints.end(), [](const glm::vec3& a, const glm::vec3& b){return a.z < b.z;});
    float avgX = (pairX.first->x + pairX.second->x) / 2.0;
    float avgY = (pairY.first->y + pairY.second->y) / 2.0;
    float avgZ = (pairZ.first->z + pairZ.second->z) / 2.0;
    std::cout << "x min: " << pairX.first->x << " x max: " << pairX.second->x << "\n";
    std::cout << "y min: " << pairY.first->y << " y max: " << pairY.second->y << "\n";
    std::cout << "z min: " << pairZ.first->z << " x max: " << pairZ.second->z << "\n";
    return std::make_tuple(avgX, avgY, avgZ);
}

std::vector<glm::vec3> Canvas::getSelectedPoints()
{
    std::vector<glm::vec3> points;
    for(std::size_t index : m_selected)
        points.push_back(m_points[index]);
    return points;
}

void Canvas::setSelectedPoints(const std::vector<glm::vec3>& points)
{
    for(std::size_t i = 0; i < points.size(); ++i)
        m_points[m_selected[i]] = points[i];
}

glm::vec4 multiply(glm::vec4& point, const glm::mat4& matrix)
{
    glm::vec4 outPoint(0, 0, 0, 0);
    outPoint[0] = point[0]*matrix[0][0] + point[1]*matrix[1][0] + point[2]*matrix[2][0] + point[3]*matrix[3][0];
    outPoint[1] = point[0]*matrix[0][1] + point[1]*matrix[1][1] + point[2]*matrix[2][1] + point[3]*matrix[3][1];
    outPoint[2] = point[0]*matrix[0][2] + point[1]*matrix[1][2] + point[2]*matrix[2][2] + point[3]*matrix[3][2];
    outPoint[3] = point[0]*matrix[0][3] + point[1]*matrix[1][3] + point[2]*matrix[2][3] + point[3]*matrix[3][3];
    return outPoint;
}

void Canvas::rotate(const float angle, const glm::vec3 axis)
{
    if(m_selected.empty())
        return;
    std::cout << "\n\nROTATE\n";
    float avgX; float avgY; float avgZ;
    std::tie(avgX, avgY, avgZ) = getAvg();
    // transfer(-avgX, -avgY, -avgZ);
    std::cout << "\n\nROTATION\n";
    for(std::size_t i = 0; i < m_selected.size(); ++i)
    {
        glm::vec4 point(m_points[m_selected[i]], 1.0);
        // std::cout << "Before:\n";
        // std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";

        glm::mat4 rotate = getRotateMatrix(angle, axis);
        point = multiply(point, rotate);
        // point = rotate * point;

        // std::cout << "After:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
        m_points[m_selected[i]] = point;
    }
    // transfer(avgX, avgY, avgZ);
    update();
}

void Canvas::scale(const float xmul, const float ymul, const float zmul)
{
    if(m_selected.empty())
        return;
    float avgX; float avgY; float avgZ;
    std::tie(avgX, avgY, avgZ) = getAvg();
    // transfer(-avgX, -avgY, -avgZ);
    std::cout << "\n\nSCALE\n";
    for(std::size_t i = 0; i < m_selected.size(); ++i)
    {
        glm::vec4 point(m_points[m_selected[i]], 1.0);
        std::cout << "Before:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";

        glm::mat4 scale(1.0);
        scale = glm::scale(scale, glm::vec3(xmul, ymul, zmul));
        point = scale * point;

        std::cout << "After:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
        m_points[m_selected[i]] = point;
    }
    // transfer(avgX, avgY, avgZ);
    update();
}

void Canvas::mirror(const bool x, const bool y, const bool z)
{
    if(m_selected.empty())
        return;
    float avgX; float avgY; float avgZ;
    std::tie(avgX, avgY, avgZ) = getAvg();
    // transfer(-avgX, -avgY, -avgZ);
    std::cout << "\n\nSCALE\n";
    for(std::size_t i = 0; i < m_selected.size(); ++i)
    {
        glm::vec4 point(m_points[m_selected[i]], 1.0);
        std::cout << "Before:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";

        if(x)
            point.x *= -1;
        if(y)
            point.y *= -1;
        if(z)
            point.z *= -1;

        std::cout << "After:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
        m_points[m_selected[i]] = point;
    }
    // transfer(avgX, avgY, avgZ);
    update();
}

void Canvas::projection(const float p, const float q, const float r)
{
    std::cout << "\n\nPROJECTION\n";
    std::cout << "p=" << p << "; q=" << q << "; r=" << r << "\n";
    for(std::size_t i = 0; i < m_selected.size(); ++i)
    {
        glm::vec4 point(m_points[m_selected[i]], 1.0);
        std::cout << "Before:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
        glm::mat4 projection(
            {1, 0, 0, p},
            {0, 1, 0, q},
            {0, 0, 0, r},
            {0, 0, 0, 1}
        );
        point = multiply(point, projection);
        std::cout << "Before division:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
        point.x /= point.w;
        point.y /= point.w;
        point.z /= point.w;
        point.w /= point.w;

        std::cout << "\nAfter:\n";
        std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
        m_points[m_selected[i]] = point;
    }
    update();
}

void Canvas::trimetricProjection(const float a, const float o, const float z)
{
    rotate(a, glm::vec3(0.0f, 1.0f, 0.0f));
    rotate(o, glm::vec3(1.0f, 0.0f, 0.0f));
    projection(0.0f, 0.0f, -1.0f / z);
}


// void Canvas::trimetricProjection(const float a, const float o, const float z)
// {
//     std::cout << "\n\nTRIMETRIC PROJECTION\n";
//     std::cout << "a=" << a << "; o=" << o << "; z=" << z << "\n";
//     for(std::size_t i = 0; i < m_selected.size(); ++i)
//     {
//         glm::vec4 point(m_points[m_selected[i]], 1.0);

//         float fiRad = degreesToRadians(a);
//         float thetaRad = degreesToRadians(o);

//         glm::mat4 projection(
//             {std::cos(fiRad), std::sin(fiRad)*std::sin(thetaRad), 0, std::sin(fiRad)*std::cos(thetaRad)/z},
//             {0, std::cos(thetaRad), 0, -std::sin(thetaRad)/z},
//             {std::sin(fiRad), -std::cos(fiRad)*std::sin(thetaRad), 0, -std::cos(fiRad)*std::cos(thetaRad)/z},
//             {0, 0 , 0, 1}
//         );

//         point = multiply(point, projection);

//         std::cout << "Before division:\n";
//         std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n";
//         point.x /= point.w;
//         point.y /= point.w;
//         point.z /= point.w;
//         point.w /= point.w;

//         std::cout << "\nAfter:\n";
//         std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "; w=" << point.w << "\n\n\n";
//         m_points[m_selected[i]] = point;
//     }
//     update();
// }

unsigned int Canvas::addPoint(glm::vec3 point)
{
    std::vector<std::size_t> indexes;
    for(std::size_t i = 0; i < m_points.size(); ++i)
    {
        if((m_points[i].x >= point.x - 2 && m_points[i].x <= point.x + 2) &&
           (m_points[i].y >= point.y - 2 && m_points[i].y <= point.y + 2))
        {
            indexes.push_back(i);
        }
    }
    if(m_altHolded && (state == 0 || state == 1))
    {
        auto it = std::max_element(indexes.begin(), indexes.end(), [&](std::size_t& a, std::size_t& b){
            return m_points[a].z < m_points[b].z;
        });
        if(!indexes.empty())
        {
            return *it;
        }
    }
    else if(!m_altHolded && state == 1)
    {
        for(std::size_t i = 0; i < indexes.size(); ++i)
        {
            if(m_points[indexes[i]].z <= 0.9 && m_points[indexes[i]].z >= -0.9)
            {
                std::cout << "added somehow\n";
                return indexes[i];
            }
        }
    }
    unsigned int index = m_points.size();
    m_points.push_back(point);
    m_colors.push_back({1.0f, 0.5f, 0.2f});
    m_pointColors.push_back({1.0f, 0.5f, 0.2f});
    return index;
}

unsigned int Canvas::getVBO() const
{
    return m_vbo;
}

void Canvas::setStatusBar(wxStatusBar* statusBar)
{
    m_statusBar = statusBar;
}

void Canvas::saveToFile(std::string_view path)
{
    std::ofstream file(path.data(), std::ios::out);
    if(file.good())
    {
        for(std::size_t i = 0; i < m_points.size(); ++i)
            file << m_points[i].x << " " << m_points[i].y << " " << m_points[i].z << "\n";
        file << "L\n";
        for(std::size_t i = 0; i < m_lines.size(); ++i)
            file << m_lines[i]->getIndexes().first << " " << m_lines[i]->getIndexes().second << "\n";
    }
}

void Canvas::loadFromFile(std::string_view path)
{
    clear();
    std::ifstream file(path.data(), std::ios::in);
    if(file.good())
    {
        std::string data;
        bool isIndexes = false;
        while(file >> data)
        {
            if(data != "L" && !isIndexes)
            {
                float x = std::atof(data.c_str());
                file >> data;
                float y = std::atof(data.c_str());
                file >> data;
                float z = std::atof(data.c_str());
                glm::vec3 point(x, y, z);
                std::cout << "x=" << point.x << "; y=" << point.y << "; z=" << point.z << "\n";
                m_points.push_back(point);
            }
            else if(data == "L" || isIndexes)
            {
                if(data == "L")
                {
                    isIndexes = true;
                    file >> data;
                }
                unsigned int startIndex = std::stoul(data);
                file >> data;
                unsigned int endIndex = std::stoul(data);
                std::cout << "start=" << startIndex << "; end=" << endIndex << "\n";
                Line* line = new Line({0,0,0}, {0,0,0}, *this);
                line->changeToIndexes(startIndex, endIndex);
                m_lines.push_back(line);
            }
        }

        for(std::size_t i = 0; i < m_points.size(); ++i)
        {
            m_colors.push_back({1.0f, 0.5f, 0.2f});
            m_pointColors.push_back({1.0f, 0.5f, 0.2f});
        }
    }
    else
        throw std::runtime_error("File didnt opened!\n");
    update();
}


void Canvas::clear()
{
    for(Line* line : m_lines)
        delete line;
    m_lines.clear();
    m_points.clear();
    m_colors.clear();
    m_pointColors.clear();
    m_selected.clear();
}

#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <GL/glew.h>
#include <wx/glcanvas.h>
#include <wx/statusbr.h>
#include <glm/vec3.hpp>
#include <memory>
#include <tuple>

#include "line.h"
#include "shaderprogram.h"

class Canvas : public wxGLCanvas
{
public:
    Canvas(wxWindow* parent,
           wxWindowID id,
           const int* attributes,
           const wxPoint& pos,
           const wxSize& size,
           int style,
           const wxString& name,
           const wxPalette& palette);
    ~Canvas();
    void draw();
    unsigned int getVBO() const;
    void setStatusBar(wxStatusBar* status);
    void transfer(const float x, const float y, const float z);
    void rotate(const float angle, const glm::vec3 axis);
    void scale(const float xmul, const float ymul, const float zmul);
    void mirror(const bool x, const bool y, const bool z);
    void projection(const float p, const float q, const float r);
    void trimetricProjection(const float a, const float o, const float z);

    void saveToFile(std::string_view path);
    void loadFromFile(std::string_view path);

    std::vector<glm::vec3> getSelectedPoints();
    void setSelectedPoints(const std::vector<glm::vec3>& points);

    void clear();
    void update();

    glm::vec3 translatePoint(glm::vec3 point);
private:
    void onPaint(wxPaintEvent& event);
    void onMouseMotion(wxMouseEvent& event);
    void onLMBclicked(wxMouseEvent& event);
    void onLMBDown(wxMouseEvent& event);
    void onDLMBclicked(wxMouseEvent& event);

    void onKeyDown(wxKeyEvent& event);
    void onKeyUp(wxKeyEvent& event);

    unsigned int addPoint(glm::vec3 point); //Return index of added point
    void drawPoints();

    std::tuple<float, float, float> getAvg() const;
    // std::vector<glm::vec3>& getPoints();

private:
    wxPoint m_start;
    wxPoint m_end;
    std::vector<std::size_t> m_selected;

    Line* m_line = nullptr;
    unsigned int m_sp;
    int state = 0;

    unsigned int m_vbo;
    unsigned int m_vbo2;
    unsigned int m_vao2;
    unsigned int m_vao;
    unsigned int m_vaoPoints;
    std::vector<glm::vec3> m_points;
    std::vector<glm::vec3> m_colors;
    std::vector<glm::vec3> m_pointColors;
    std::unique_ptr<wxGLContext> m_context;
    std::vector<Line*> m_lines;

    wxStatusBar* m_statusBar;
    bool m_altHolded = false;
};

#endif

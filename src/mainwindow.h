#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <wx/app.h>
#include <iostream>
#include "canvas.h"

class MainWindow : public wxApp
{
public:
    virtual bool OnInit() override;
    void resized(wxSizeEvent& event);
private:
    void onIdle(wxIdleEvent& event);
    void transfer(wxCommandEvent& event);
    void rotate(wxCommandEvent& event);
    void scale(wxCommandEvent& event);
    void mirror(wxCommandEvent& event);
    void projection(wxCommandEvent& event);
    void trimetricProjection(wxCommandEvent& event);
    void saveToFile(wxCommandEvent& event);
    void loadFromFile(wxCommandEvent& event);
    void clear(wxCommandEvent& event);
    void editPoints(wxCommandEvent& event);
private:
    Canvas* m_canvas;
    wxFrame* m_frame;
};

#endif

#include "mainwindow.h"

#include <wx/frame.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/stc/stc.h>
#include <wx/filedlg.h>
#include <wx/localedefs.h>

#include <iostream>

wxIMPLEMENT_APP(MainWindow);

bool MainWindow::OnInit()
{
    if(!wxApp::OnInit())
        return false;
    //wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)
    wxLocale m_locale;
    m_locale.Init(wxLANGUAGE_RUSSIAN);


    wxMenuBar* menuBar = new wxMenuBar();
    wxMenu* operationsMenu = new wxMenu();
    operationsMenu->Append(wxID_HIGHEST + 1, wxString(L"Смещение"), "");
    operationsMenu->Append(wxID_HIGHEST + 2, wxString(L"Вращение"), "");
    operationsMenu->Append(wxID_HIGHEST + 3, wxString(L"Масштабирование"), "");
    operationsMenu->Append(wxID_HIGHEST + 4, wxString(L"Зеркалирование"), "");
    operationsMenu->Append(wxID_HIGHEST + 5, wxString(L"Проецирование"), "");
    operationsMenu->Append(wxID_HIGHEST + 6, wxString(L"Триметрическая проекция"), "");

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_HIGHEST + 7, wxString(L"Сохранить"), "");
    fileMenu->Append(wxID_HIGHEST + 8, wxString(L"Загрузить"), "");

    wxMenu* actionsMenu = new wxMenu();
    actionsMenu->Append(wxID_HIGHEST + 9, wxString(L"Очистить холст"), "");
    actionsMenu->Append(wxID_HIGHEST + 10, wxString(L"Редактировать точки"), "");

    menuBar->Append(fileMenu, wxString(L"Файл"));
    menuBar->Append(operationsMenu, wxString(L"Операции"));
    menuBar->Append(actionsMenu, wxString(L"Действия"));
    // menuBar->Insert(0, mainMenu, "title");

    m_frame = new wxFrame(nullptr, -1, "3D Editor", wxDefaultPosition, {800, 800});
    m_frame->SetMenuBar(menuBar);
    m_frame->Bind(wxEVT_MENU, &MainWindow::transfer, this, wxID_HIGHEST + 1);
    m_frame->Bind(wxEVT_MENU, &MainWindow::rotate, this, wxID_HIGHEST + 2);
    m_frame->Bind(wxEVT_MENU, &MainWindow::scale, this, wxID_HIGHEST + 3);
    m_frame->Bind(wxEVT_MENU, &MainWindow::mirror, this, wxID_HIGHEST + 4);
    m_frame->Bind(wxEVT_MENU, &MainWindow::projection, this, wxID_HIGHEST + 5);
    m_frame->Bind(wxEVT_MENU, &MainWindow::trimetricProjection, this, wxID_HIGHEST + 6);
    m_frame->Bind(wxEVT_MENU, &MainWindow::saveToFile, this, wxID_HIGHEST + 7);
    m_frame->Bind(wxEVT_MENU, &MainWindow::loadFromFile, this, wxID_HIGHEST + 8);
    m_frame->Bind(wxEVT_MENU, &MainWindow::clear, this, wxID_HIGHEST + 9);
    m_frame->Bind(wxEVT_MENU, &MainWindow::editPoints, this, wxID_HIGHEST + 10);

    wxSize size = m_frame->GetClientSize();
    std::cout << "next: " << size.GetWidth() << " height: " << size.GetHeight() << "\n";
    m_canvas = new Canvas(m_frame, -1, 0, wxDefaultPosition, {size.GetWidth(), size.GetHeight()}, 0, L"GLCANVAS", wxNullPalette);

    wxStatusBar* status = m_frame->CreateStatusBar();
    m_canvas->setStatusBar(status);

    m_frame->Layout();
    m_frame->Show();
    Bind(wxEVT_SIZE, &MainWindow::resized, this);
    Bind(wxEVT_IDLE, &MainWindow::onIdle, this);
    Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event){
        std::cout << "fwmfwemfw\n";
        event.Skip();
    });
    return true;
}

void MainWindow::editPoints(wxCommandEvent& event)
{
    wxDialog dialog(nullptr, -1, "Edit points");
    dialog.SetSize({200, 200});

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(horizontalSizer);
    std::vector<wxBoxSizer*> sizers(6);
    for(std::size_t i = 0; i < 6; ++i)
    {
        sizers[i] = new wxBoxSizer(wxVERTICAL);
        horizontalSizer->Add(sizers[i]);
    }

    std::vector<glm::vec3> points = m_canvas->getSelectedPoints();
    std::vector<wxSpinCtrl*> spins(points.size()*3);
    std::vector<wxStaticText*> staticTexts(points.size()*3);

    wxSize size = m_frame->GetClientSize();


    for(std::size_t i = 0; i < points.size(); ++i)
    {
        std::cout << "x=" << points[i].x << "; y=" << points[i].y << "; z=" << points[i].z << "\n";
        staticTexts[i + 0] = new wxStaticText(&dialog, -1, "X:");
        staticTexts[i + 1] = new wxStaticText(&dialog, -1, "Y:");
        staticTexts[i + 2] = new wxStaticText(&dialog, -1, "Z:");

        spins[i + 0] = new wxSpinCtrl(&dialog, -1, "");
        spins[i + 0]->SetMin(0);
        spins[i + 0]->SetMax(size.GetWidth());
        spins[i + 0]->SetValue(points[i].x);

        spins[i + 1] = new wxSpinCtrl(&dialog, -1, "");
        spins[i + 1]->SetMin(0);
        spins[i + 1]->SetMax(size.GetHeight());
        spins[i + 1]->SetValue(points[i].y);

        spins[i + 2] = new wxSpinCtrl(&dialog, -1, "");
        spins[i + 2]->SetMin(-1000);
        spins[i + 2]->SetMax(1000);
        spins[i + 2]->SetValue(points[i].z);

        sizers[0]->Add(staticTexts[i + 0]);
        sizers[2]->Add(staticTexts[i + 1]);
        sizers[4]->Add(staticTexts[i + 2]);

        sizers[1]->Add(spins[i + 0]);
        sizers[3]->Add(spins[i + 1]);
        sizers[5]->Add(spins[i + 2]);
    }

    wxButton* confirmButton = new wxButton(&dialog, -1, wxString(L"Подтвердить"));
    confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event){
        std::cout << "OMG SOMETHING HAPPENED\n";
        std::vector<glm::vec3> pointsOut(points.size());
        for(std::size_t i = 0; i < points.size(); ++i)
        {
            int x = spins[i + 0]->GetValue();
            int y = spins[i + 1]->GetValue();
            int z = spins[i + 2]->GetValue();
            pointsOut[i] = glm::vec3({x, y, z});
        }
        m_canvas->setSelectedPoints(pointsOut);
        m_canvas->update();
    });

    topSizer->Add(confirmButton);

    dialog.SetSizerAndFit(topSizer);
    dialog.Centre();
    dialog.Layout();
    dialog.ShowModal();
}

void MainWindow::transfer(wxCommandEvent& event)
{
    wxDialog dialog(nullptr, -1, "Transfer");
    dialog.SetSize({200, 200});

    wxSpinCtrl* spinX = new wxSpinCtrl(&dialog, -1, "0");
    spinX->SetMin(-1000);
    spinX->SetMax(1000);
    spinX->SetSize({100, 50});
    wxStaticText* textX = new wxStaticText(&dialog, -1, "X:");
    wxBoxSizer* sizerX = new wxBoxSizer(wxHORIZONTAL);
    sizerX->Add(textX);
    sizerX->Add(spinX);

    wxSpinCtrl* spinY = new wxSpinCtrl(&dialog, -1, "0");
    spinY->SetMin(-1000);
    spinY->SetMax(1000);
    spinY->SetSize({100, 50});
    wxStaticText* textY = new wxStaticText(&dialog, -1, "Y:");
    wxBoxSizer* sizerY = new wxBoxSizer(wxHORIZONTAL);
    sizerY->Add(textY);
    sizerY->Add(spinY);

    wxSpinCtrl* spinZ = new wxSpinCtrl(&dialog, -1, "0");
    spinZ->SetMin(-1000);
    spinZ->SetMax(1000);
    spinZ->SetSize({100, 50});
    wxStaticText* textZ = new wxStaticText(&dialog, -1, "Z:");
    wxBoxSizer* sizerZ = new wxBoxSizer(wxHORIZONTAL);
    sizerZ->Add(textZ);
    sizerZ->Add(spinZ);

    wxButton* confirmButton = new wxButton(&dialog, -1, wxString(L"Подтвердить"));
    confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event){
        int x = spinX->GetValue();
        int y = spinY->GetValue();
        int z = spinZ->GetValue();
        // dialog.Close();
        m_canvas->transfer(x, y, z);
    });

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(sizerX);
    topSizer->Add(sizerY);
    topSizer->Add(sizerZ);
    topSizer->Add(confirmButton);

    dialog.SetSizerAndFit(topSizer);
    dialog.Centre();
    dialog.Layout();
    dialog.ShowModal();
}

void MainWindow::rotate(wxCommandEvent& event)
{
    wxDialog dialog(nullptr, -1, "Rotate");
    dialog.SetSize({200, 200});

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    wxSpinCtrl* spinAngle = new wxSpinCtrl(&dialog, -1, "0");
    spinAngle->SetMax(360);
    spinAngle->SetMin(-360);
    wxStaticText* textAngle = new wxStaticText(&dialog, -1, wxString(L"Угол"));
    topSizer->Add(textAngle);
    topSizer->Add(spinAngle);

    wxArrayString choices;
    choices.Add(wxString(L"Ось x"));
    choices.Add(wxString(L"Ось y"));
    choices.Add(wxString(L"Ось z"));
    wxRadioBox* radioBox = new wxRadioBox(&dialog, -1, wxString(L"Выберите оси"), wxDefaultPosition, wxDefaultSize, choices, 1);
    topSizer->Add(radioBox);

    wxButton* confirmButton = new wxButton(&dialog, -1, wxString(L"Подтвердить"));
    confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event){
        std::cout << "rotation around " << radioBox->GetSelection() << " at angle " << spinAngle->GetValue() << "\n";
        glm::vec3 axis(0.0f, 0.0f, 0.0f);
        axis[radioBox->GetSelection()] = 1.0f;
        m_canvas->rotate(spinAngle->GetValue(), axis);
    });
    topSizer->Add(confirmButton);

    dialog.SetSizerAndFit(topSizer);
    dialog.Centre();
    dialog.Layout();
    dialog.ShowModal();
}

void MainWindow::scale(wxCommandEvent& event)
{
    wxDialog* dialog = new wxDialog(nullptr, -1, "Scale");
    dialog->SetSize({200, 200});

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* textX = new wxTextCtrl(dialog, -1, "1.0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    // wxStyledTextCtrl* textX = new wxStyledTextCtrl(dialog, -1);
    textX->SetSize({100, 50});
    wxStaticText* staticTextX = new wxStaticText(dialog, -1, "X:");
    wxBoxSizer* sizerX = new wxBoxSizer(wxHORIZONTAL);
    sizerX->Add(staticTextX);
    sizerX->Add(textX);

    wxTextCtrl* textY = new wxTextCtrl(dialog, -1, "1.0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    textY->SetSize({100, 50});
    wxStaticText* staticTextY = new wxStaticText(dialog, -1, "Y:");
    wxBoxSizer* sizerY = new wxBoxSizer(wxHORIZONTAL);
    sizerY->Add(staticTextY);
    sizerY->Add(textY);

    wxTextCtrl* textZ = new wxTextCtrl(dialog, -1, "1.0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    textZ->SetSize({100, 50});
    wxStaticText* staticTextZ = new wxStaticText(dialog, -1, "Z:");
    wxBoxSizer* sizerZ = new wxBoxSizer(wxHORIZONTAL);
    sizerZ->Add(staticTextZ);
    sizerZ->Add(textZ);

    wxButton* confirmButton = new wxButton(dialog, -1, wxString(L"Подтвердить"));
    confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event){
        float xmul = std::atof(textX->GetValue().ToStdString().c_str());
        float ymul = std::atof(textY->GetValue().ToStdString().c_str());
        float zmul = std::atof(textZ->GetValue().ToStdString().c_str());
        std::cout << "zmul: " << zmul << "\n";
        m_canvas->scale(xmul, ymul, zmul);
    });

    topSizer->Add(sizerX);
    topSizer->Add(sizerY);
    topSizer->Add(sizerZ);
    topSizer->Add(confirmButton);

    dialog->SetSizerAndFit(topSizer);
    dialog->Centre();
    dialog->Layout();
    dialog->ShowModal();
}

void MainWindow::mirror(wxCommandEvent& event)
{
    wxDialog dialog(nullptr, -1, "Mirror");
    dialog.SetSize({200, 200});
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

    wxCheckBox* checkboxX = new wxCheckBox(&dialog, -1, wxString(L"Отразить по X"));
    wxCheckBox* checkboxY = new wxCheckBox(&dialog, -1, wxString(L"Отразить по Y"));
    wxCheckBox* checkboxZ = new wxCheckBox(&dialog, -1, wxString(L"Отразить по Z"));

    wxButton* confirmButton = new wxButton(&dialog, -1, wxString(L"Подтвердить"));
    confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event){
        std::cout << checkboxX->IsChecked() << " " << checkboxY->IsChecked() << " " << checkboxZ->IsChecked() << "\n";
        m_canvas->mirror(checkboxX->IsChecked(), checkboxY->IsChecked(), checkboxZ->IsChecked());
    });
    topSizer->Add(checkboxX);
    topSizer->Add(checkboxY);
    topSizer->Add(checkboxZ);
    topSizer->Add(confirmButton);

    dialog.SetSizerAndFit(topSizer);
    dialog.Centre();
    dialog.Layout();
    dialog.ShowModal();
}

void MainWindow::projection(wxCommandEvent& event)
{
    wxDialog* dialog = new wxDialog(nullptr, -1, "Scale");
    dialog->SetSize({200, 200});

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* textX = new wxTextCtrl(dialog, -1, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    // wxStyledTextCtrl* textX = new wxStyledTextCtrl(dialog, -1);
    textX->SetSize({100, 50});
    wxStaticText* staticTextX = new wxStaticText(dialog, -1, "p:");
    wxBoxSizer* sizerX = new wxBoxSizer(wxHORIZONTAL);
    sizerX->Add(staticTextX);
    sizerX->Add(textX);

    wxTextCtrl* textY = new wxTextCtrl(dialog, -1, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    textY->SetSize({100, 50});
    wxStaticText* staticTextY = new wxStaticText(dialog, -1, "q:");
    wxBoxSizer* sizerY = new wxBoxSizer(wxHORIZONTAL);
    sizerY->Add(staticTextY);
    sizerY->Add(textY);

    wxTextCtrl* textZ = new wxTextCtrl(dialog, -1, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    textZ->SetSize({100, 50});
    wxStaticText* staticTextZ = new wxStaticText(dialog, -1, "r:");
    wxBoxSizer* sizerZ = new wxBoxSizer(wxHORIZONTAL);
    sizerZ->Add(staticTextZ);
    sizerZ->Add(textZ);

    wxButton* confirmButton = new wxButton(dialog, -1, wxString(L"Подтвердить"));
    confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event){
        float p = std::atof(textX->GetValue().ToStdString().c_str());
        float q = std::atof(textY->GetValue().ToStdString().c_str());
        float r = std::atof(textZ->GetValue().ToStdString().c_str());
        m_canvas->projection(p, q, r);
    });

    topSizer->Add(sizerX);
    topSizer->Add(sizerY);
    topSizer->Add(sizerZ);
    topSizer->Add(confirmButton);

    dialog->SetSizerAndFit(topSizer);
    dialog->Centre();
    dialog->Layout();
    dialog->ShowModal();
}

void MainWindow::trimetricProjection(wxCommandEvent& event)
{
    wxDialog* dialog = new wxDialog(nullptr, -1, "Trimetric projection");
    dialog->SetSize({200, 200});

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* textX = new wxTextCtrl(dialog, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    // wxStyledTextCtrl* textX = new wxStyledTextCtrl(dialog, -1);
    textX->SetSize({100, 50});
    wxStaticText* staticTextX = new wxStaticText(dialog, -1, "f:");
    wxBoxSizer* sizerX = new wxBoxSizer(wxHORIZONTAL);
    sizerX->Add(staticTextX);
    sizerX->Add(textX);

    wxTextCtrl* textY = new wxTextCtrl(dialog, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    textY->SetSize({100, 50});
    wxStaticText* staticTextY = new wxStaticText(dialog, -1, "o:");
    wxBoxSizer* sizerY = new wxBoxSizer(wxHORIZONTAL);
    sizerY->Add(staticTextY);
    sizerY->Add(textY);

    wxTextCtrl* textZ = new wxTextCtrl(dialog, -1, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    textZ->SetSize({100, 50});
    wxStaticText* staticTextZ = new wxStaticText(dialog, -1, "Zc:");
    wxBoxSizer* sizerZ = new wxBoxSizer(wxHORIZONTAL);
    sizerZ->Add(staticTextZ);
    sizerZ->Add(textZ);

    wxButton* confirmButton = new wxButton(dialog, -1, wxString(L"Подтвердить"));
    confirmButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event){
        float a = std::atof(textX->GetValue().ToStdString().c_str());
        float o = std::atof(textY->GetValue().ToStdString().c_str());
        float z = std::atof(textZ->GetValue().ToStdString().c_str());
        m_canvas->trimetricProjection(a, o, z);
    });

    topSizer->Add(sizerX);
    topSizer->Add(sizerY);
    topSizer->Add(sizerZ);
    topSizer->Add(confirmButton);

    dialog->SetSizerAndFit(topSizer);
    dialog->Centre();
    dialog->Layout();
    dialog->ShowModal();
}

void MainWindow::onIdle(wxIdleEvent& event)
{
    m_canvas->draw();
    event.RequestMore();
}

void MainWindow::resized(wxSizeEvent& event)
{
    wxSize size = m_frame->GetClientSize();
    m_canvas->SetSize(size.GetWidth(), size.GetHeight());
    m_canvas->update();
    m_canvas->Centre();
    m_frame->Layout();
}

void MainWindow::saveToFile(wxCommandEvent& event)
{
    wxFileDialog fileDialog(m_frame, "Save .edt file", "E:\\C++\\Ori\\GraphicalEditor\\edt\\",
                            "", "EDT files (*.edt)|*.edt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(fileDialog.ShowModal() == wxID_CANCEL)
        return;

    std::cout << "Path: " << fileDialog.GetPath() << "\n";
    m_canvas->saveToFile(fileDialog.GetPath().ToStdString());
}

void MainWindow::loadFromFile(wxCommandEvent& event)
{
    wxFileDialog fileDialog(m_frame, "Open .edt file", "E:\\C++\\Ori\\GraphicalEditor\\edt\\",
                        "", "EDT files (*.edt)|*.edt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(fileDialog.ShowModal() == wxID_CANCEL)
        return;

    std::cout << "Path: " << fileDialog.GetPath() << "\n";
    m_canvas->loadFromFile(fileDialog.GetPath().ToStdString());
}

void MainWindow::clear(wxCommandEvent& event)
{
    m_canvas->clear();
}

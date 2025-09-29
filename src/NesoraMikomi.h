#pragma once
#include <wx/wx.h>

#include "NesoraStyle/button/NesoraButton.h"

class MyApp : public wxApp {
    wxCustomButton* m_btn_1;
    wxCustomButton* m_btn_2;
    wxCustomButton* m_btn_3;
public:
    bool OnInit() override;
};

class MyFrame : public wxFrame {
public:
    MyFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

enum {
    ID_Hello = 1
};

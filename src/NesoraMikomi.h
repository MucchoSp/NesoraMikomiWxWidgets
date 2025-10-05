#pragma once
#include <wx/wx.h>

#include "NesoraStyle/button/NesoraButton.h"
#include "NesoraStyle/chart/NesoraChart.h"
#include "NesoraStyle//slider/NesoraSlider.h"
#include "NesoraStyle/NesoraColor.h"

class MyApp : public wxApp {
public:
    bool OnInit() override;
};

class MyFrame : public wxFrame {
public:
    MyFrame();

private:
    void MenuSetup();

    wxStaticText* text;
    nsSlider* slider;
    wxSlider* slider_sample;

    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSlide(wxCommandEvent& event);
};

enum {
    ID_Hello = 1,
    ID_EXIT,
    ID_SLIDER
};

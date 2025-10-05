#include "NesoraMikomi.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Nesora 1-0") {
    MenuSetup();

    wxPanel * panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(1000, 300));
    panel->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    text = new wxStaticText(panel, wxID_ANY, "50");
    text->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    sizer->Add(text, 0, wxALIGN_CENTER);

    slider = new nsSlider(panel, ID_SLIDER, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_LABELS);
    slider->Show();
    sizer->Add(slider, 0, wxEXPAND | wxALL, 5);

    slider_sample = new wxSlider(panel, ID_SLIDER, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_LABELS);
    slider_sample->Show();
    sizer->Add(slider_sample, 0, wxEXPAND | wxALL, 5);

    panel->SetSizer(sizer);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnExit, this, ID_EXIT);
    Bind(wxEVT_COMMAND_SLIDER_UPDATED, &MyFrame::OnSlide, this, ID_SLIDER);
}

void MyFrame::MenuSetup() {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&ファイル");
    menuBar->Append(menuHelp, "&ヘルプ");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event) {
    wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnSlide(wxCommandEvent& event) {
    text->SetLabel(std::to_string(slider->GetValue()));
    slider_sample->SetValue(slider->GetValue());
}

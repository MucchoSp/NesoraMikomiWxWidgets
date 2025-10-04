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
    panel->SetBackgroundColour(nsGetColor(nsColorType::PRIMARY));
    
    // wxButton *button = new wxButton(panel, wxID_EXIT, _("Exit"), wxPoint(20, 20));
    // button->SetFocus();
    // nsButton *nsbutton = new nsButton(panel, ID_EXIT, _("Exit"), wxPoint(20, 60));
    // nsbutton->SetFocus();

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    nsChartControl* chart = new nsChartControl(panel, wxID_ANY, wxDefaultPosition, this->FromDIP(wxSize(800, 400)));
    chart->SetTitle("Sample Chart");
    chart->SetData({ 0.34, -0.17, 0.98, 0.33 });

    sizer->Add(chart, 1, wxEXPAND | wxALL, 5);

    panel->SetSizer(sizer);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnExit, this, ID_EXIT);
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

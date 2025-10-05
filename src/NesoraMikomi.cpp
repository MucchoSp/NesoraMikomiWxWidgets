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

    globalPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(1000, 300));
    globalPanel->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    RosenbergWavePanelSetup();

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnExit, this, ID_EXIT);
    Bind(wxEVT_COMMAND_SLIDER_UPDATED, &MyFrame::OnT1Slide, this, ID_T1SLIDER);
    Bind(wxEVT_COMMAND_SLIDER_UPDATED, &MyFrame::OnT2Slide, this, ID_T2SLIDER);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnPlayButton, this, ID_PLAY_BUTTON);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnStopButton, this, ID_STOP_BUTTON);
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

void MyFrame::RosenbergWavePanelSetup() {

    source_wave.SetParamater(0.25, 0.5, 0);
    wave.resize(183);// 48000/261.6
    double throw_away;
    for (int i = 0;i < wave.size();i++)
        wave[i] = source_wave.Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* t1sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t2sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* playButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    chart = new nsChartControl(globalPanel, wxID_ANY);
    chart->SetTitle("RosenbergWave");
    chart->ShowGrid(false);
    chart->ShowLabel(false);
    chart->SetData(wave);


    sizer->Add(chart, 1, wxEXPAND | wxALL);

    t1param = new wxStaticText(globalPanel, wxID_ANY, "0.25");
    t1param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t1sliderSizer->Add(t1param, 0, wxEXPAND | wxALL, 5);
    t1slider = new nsSlider(globalPanel, ID_T1SLIDER, 25, 0, 50, wxDefaultPosition, wxSize(300, 15));
    t1slider->Show();
    t1sliderSizer->Add(t1slider, 0, wxEXPAND | wxALL, 5);
    sizer->Add(t1sliderSizer, 0, wxEXPAND | wxALL);

    t2param = new wxStaticText(globalPanel, wxID_ANY, "0.50");
    t2param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t2sliderSizer->Add(t2param, 0, wxEXPAND | wxALL, 5);
    t2slider = new nsSlider(globalPanel, ID_T2SLIDER, 50, 1, 100, wxDefaultPosition, wxSize(300, 15));
    t2slider->Show();
    t2sliderSizer->Add(t2slider, 0, wxEXPAND | wxALL, 5);
    sizer->Add(t2sliderSizer, 0, wxEXPAND | wxALL);

    playButton = new nsButton(globalPanel, ID_PLAY_BUTTON, "Play");
    stopButton = new nsButton(globalPanel, ID_STOP_BUTTON, "Stop");
    playButtonSizer->Add(playButton, 0, wxEXPAND | wxALL);
    playButtonSizer->Add(stopButton, 0, wxEXPAND | wxALL);
    sizer->Add(playButtonSizer, 0, wxEXPAND | wxALL);

    globalPanel->SetSizer(sizer);


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

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

void MyFrame::OnT1Slide(wxCommandEvent& event) {
    t1param->SetLabel(to_string_with_precision((double)t1slider->GetValue() / 100.0, 2));
    source_wave.SetParamater((double)t1slider->GetValue() / 100.0, (double)t2slider->GetValue() / 100.0, 0);
    
    double throw_away;
    for (int i = 0;i < wave.size();i++)
        wave[i] = source_wave.Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));
    chart->SetData(wave);
}

void MyFrame::OnT2Slide(wxCommandEvent& event) {
    t2param->SetLabel(to_string_with_precision((double)t2slider->GetValue() / 100.0, 2));
    t1slider->SetRange(0, t2slider->GetValue());
    OnT1Slide(event);
}

void MyFrame::OnPlayButton(wxCommandEvent& event) {

}

void MyFrame::OnStopButton(wxCommandEvent& event) {

}

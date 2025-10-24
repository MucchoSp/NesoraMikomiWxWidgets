#include "NesoraVoiceMakePanel.h"

void nsVoiceMakePlayInterfacePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Play Interface");
    playButton = new nsButton(sizer->GetStaticBox(), wxID_ANY, "Play");
    stopButton = new nsButton(sizer->GetStaticBox(), wxID_ANY, "Stop");
    statusText = new wxStaticText(sizer->GetStaticBox(), wxID_ANY, "Status: Stopped");
    statusText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    statusText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    sizer->Add(playButton, 0, wxEXPAND | wxALL);
    sizer->Add(stopButton, 0, wxEXPAND | wxALL);
    sizer->Add(statusText, 0, wxEXPAND | wxALL);

    this->SetSizer(sizer);

    playButton->Bind(wxEVT_BUTTON, &nsVoiceMakePlayInterfacePanel::OnPlayButtonClicked, this);
    stopButton->Bind(wxEVT_BUTTON, &nsVoiceMakePlayInterfacePanel::OnStopButtonClicked, this);
}

void nsVoiceMakePlayInterfacePanel::OnPlayButtonClicked(wxCommandEvent& event) {
    statusText->SetLabel("Status: Playing");
}

void nsVoiceMakePlayInterfacePanel::OnStopButtonClicked(wxCommandEvent& event) {
    statusText->SetLabel("Status: Stopped");
}




void nsVoiceMakePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sourceSoundPanel = new nsRosenbergWavePanel(this, wxID_ANY);
    filterPanel = new nsIIRFilterPanel(this, wxID_ANY);
    playInterfacePanel = new nsVoiceMakePlayInterfacePanel(this, wxID_ANY);
    sizer->Add(playInterfacePanel, 0, wxEXPAND | wxALL);
    sizer->Add(sourceSoundPanel, 0, wxEXPAND | wxALL);
    sizer->Add(filterPanel, 1, wxEXPAND | wxALL);

    this->SetSizer(sizer);
}

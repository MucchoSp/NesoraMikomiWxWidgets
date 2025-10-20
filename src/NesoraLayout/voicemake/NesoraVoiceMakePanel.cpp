#include "NesoraVoiceMakePanel.h"

void nsVoiceMakePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sourceSoundPanel = new nsRosenbergWavePanel(this, wxID_ANY);
    filterPanel = new nsFormantFilter(this, wxID_ANY);
    sizer->Add(sourceSoundPanel, 0, wxEXPAND | wxALL);
    sizer->Add(filterPanel, 1, wxEXPAND | wxALL);

    this->SetSizer(sizer);
}

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraSingPanel.h"

void nsSingPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    // wxStaticText* label = new wxStaticText(this, wxID_ANY, _("This is the Sing Panel"), wxPoint(10, 10));
    midiPanel = new NesoraMIDIPanel(this);
    // velocity = new NesoraVelocityPanel(this);

    sizer->Add(midiPanel, 1, wxEXPAND | wxALL);
    // sizer->Add(velocity, 0, wxEXPAND | wxALL);

    this->SetSizer(sizer);
}

void nsSingPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_("The process for saving the singing panel has not yet been implemented..."));
}

void nsSingPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_("The process for opening the singing panel has not yet been implemented..."));
}

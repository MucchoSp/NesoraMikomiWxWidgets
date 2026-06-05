// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraDictionalyEditPanel.h"




// MARK: NesoraDictionalyTitlePanel

void NesoraDictionalyTitlePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Title"));

    titleEdit = new wxTextCtrl(mainSizer->GetStaticBox(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    titleEdit->SetHint(_("Title"));
    mainSizer->Add(titleEdit, 1, wxEXPAND | wxALL);
    mainSizer->AddSpacer(10);
    symbolEdit = new wxTextCtrl(mainSizer->GetStaticBox(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    symbolEdit->SetHint(_("Symbol"));
    mainSizer->Add(symbolEdit, 1, wxEXPAND | wxALL);

    SetSizer(mainSizer);
}



// MARK: NesoraDictionalyTimelineControl

void NesoraDictionalyTimelineControl::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetMinSize(wxSize(-1, 24));
    
    Bind(wxEVT_PAINT, &NesoraDictionalyTimelineControl::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraDictionalyTimelineControl::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &NesoraDictionalyTimelineControl::OnLeftUp, this);
    Bind(wxEVT_MOTION, &NesoraDictionalyTimelineControl::OnMouseMove, this);
    Bind(wxEVT_RIGHT_DOWN, &NesoraDictionalyTimelineControl::OnRightDown, this);
    Bind(wxEVT_RIGHT_UP, &NesoraDictionalyTimelineControl::OnRightUp, this);
}

void NesoraDictionalyTimelineControl::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBackground(nsGetColor(nsColorType::BACKGROUND));
    dc.Clear();


    const wxDouble frontMargin = 100.0;
    
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 4));
        gc->DrawRectangle(0, 0, size.x, size.y);
        gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
        wxPoint2DDouble points[] = { {frontMargin, 0}, {frontMargin, (double)size.y} };
        gc->StrokeLines(2, points);

        delete gc;
    }
}

void NesoraDictionalyTimelineControl::OnLeftDown(wxMouseEvent& event) {

}

void NesoraDictionalyTimelineControl::OnLeftUp(wxMouseEvent& event) {

}

void NesoraDictionalyTimelineControl::OnMouseMove(wxMouseEvent& event) {

}

void NesoraDictionalyTimelineControl::OnRightDown(wxMouseEvent& event) {

}

void NesoraDictionalyTimelineControl::OnRightUp(wxMouseEvent& event) {

}

// MARK: NesoraDictionalyTimelinePanel

void NesoraDictionalyTimelinePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Timeline"));
    formantTimelineText = new wxStaticText(mainSizer->GetStaticBox(), wxID_ANY, _("Formant"));
    mainSizer->Add(formantTimelineText, 0, wxALIGN_LEFT | wxALL);
    formantTimelineControl = new NesoraDictionalyTimelineControl(mainSizer->GetStaticBox(), wxID_ANY);
    mainSizer->Add(formantTimelineControl, 0, wxEXPAND | wxALL);

    envelopeTimelineText = new wxStaticText(mainSizer->GetStaticBox(), wxID_ANY, _("Envelope"));
    mainSizer->Add(envelopeTimelineText, 0, wxALIGN_LEFT | wxALL);
    envelopeTimelineControl = new NesoraDictionalyTimelineControl(mainSizer->GetStaticBox(), wxID_ANY);
    mainSizer->Add(envelopeTimelineControl, 0, wxEXPAND | wxALL);

    SetSizer(mainSizer);
}





// MARK: NesoraDictionalyControlPointParameterPanel

void NesoraDictionalyControlPointParameterPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Parameter"));

    parameterValueEdit = new wxTextCtrl(mainSizer->GetStaticBox(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    parameterValueEdit->SetHint(_("Parameter Value"));
    mainSizer->Add(parameterValueEdit, 0, wxEXPAND | wxALL);

    SetSizer(mainSizer);
}




// MARK: NesoraDictionalyEditPanel

void NesoraDictionalyEditPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    titlePanel = new NesoraDictionalyTitlePanel(this, wxID_ANY);
    timelinePanel = new NesoraDictionalyTimelinePanel(this, wxID_ANY);
    controlPointParameterPanel = new NesoraDictionalyControlPointParameterPanel(this, wxID_ANY);
    mainSizer->Add(titlePanel, 0, wxEXPAND | wxALL);
    mainSizer->Add(timelinePanel, 0, wxEXPAND | wxALL);
    mainSizer->Add(controlPointParameterPanel, 1, wxEXPAND | wxALL);

    SetSizer(mainSizer);
}

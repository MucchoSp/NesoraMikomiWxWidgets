// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraMainFrame.h"

// MARK: nsMainFrame

nsMainFrame::nsMainFrame()
    : wxFrame(nullptr, nsID_MAIN_FRAME, _("Nesora 1-0"), wxDefaultPosition, wxSize(1280, 720)) {
    menuSetup();
    acceleratorSetup();

    toolSelectorPanel = new wxPanel(this, wxID_ANY);
    toolSelectorPanel->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    makePanel = new nsMakePanel(this, nsID_MAKE_PANEL, wxDefaultPosition, wxSize(1000, 300));
    makePanel->Hide();
    singPanel = new nsSingPanel(this, nsID_SING_PANEL, wxDefaultPosition, wxSize(1000, 300));
    singPanel->Hide();
    speakPanel = new nsSpeakPanel(this, nsID_SPEAK_PANEL, wxDefaultPosition, wxSize(1000, 300));
    speakPanel->Hide();

    voice = makePanel->GetVoice();
    singPanel->SetVoice(voice);
    speakPanel->SetVoice(voice);

    main_sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);

    makeButton = new nsToolBarButton(toolSelectorPanel, nsID_MAKE_BUTTON, _("Make"), wxPoint(230, 10), wxSize(100, 30));
    singButton = new nsToolBarButton(toolSelectorPanel, nsID_SING_BUTTON, _("Sing"), wxPoint(10, 10), wxSize(100, 30));
    speakButton = new nsToolBarButton(toolSelectorPanel, nsID_SPEAK_BUTTON, _("Talk"), wxPoint(120, 10), wxSize(100, 30));
    makeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnMakeButton, this);
    singButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnSingButton, this);
    speakButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnSpeakButton, this);
    toolbar_sizer->Add(makeButton, 0, wxALL, 5);
    toolbar_sizer->Add(singButton, 0, wxALL, 5);
    toolbar_sizer->Add(speakButton, 0, wxALL, 5);

    toolSelectorPanel->SetSizer(toolbar_sizer);

    main_sizer->Add(toolSelectorPanel, 0, wxEXPAND | wxALL, 5);
    main_sizer->Add(makePanel, 1, wxEXPAND | wxALL);
    main_sizer->Add(singPanel, 1, wxEXPAND | wxALL, 5);
    main_sizer->Add(speakPanel, 1, wxEXPAND | wxALL, 5);

    SetSizer(main_sizer);

    CreateStatusBar();
    SetStatusText("Welcome to Nesora 1-0!");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnExit, this, ID_EXIT);

    wxCommandEvent evt;
    // OnMakeButton(evt);
    // selectedToolBarType = nsToolBarType::TOOLBAR_VOICE_MAKE;
    // OnSingButton(evt);
    // selectedToolBarType = nsToolBarType::TOOLBAR_SING;
    OnMakeButton(evt);
}

void nsMainFrame::menuSetup() {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_SAVE, _("&Save\tCtrl-S"), _("Save the current file"));
    menuFile->Append(wxID_OPEN, _("&Open...\tCtrl-O"), _("Open a file"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenu* menuEdit = new wxMenu;
    // menuEdit->Append(nsID_ESCAPE, _("&Deselect\tEsc"));

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuEdit, _("&Edit"));
    menuBar->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &nsMainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &nsMainFrame::OnExit, this, wxID_EXIT);
    // Bind(wxEVT_MENU, &nsMainFrame::OnSave, this, wxID_SAVE);
    // Bind(wxEVT_MENU, &nsMainFrame::OnOpen, this, wxID_OPEN);
}

void nsMainFrame::acceleratorSetup() {
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);  //コントロールSで保存
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);
}

void nsMainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void nsMainFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("Nesora 1-0\nCopyright (c) 2026 MucchoSP", "About", wxOK);
}

void nsMainFrame::OnMakeButton(wxCommandEvent& event) {
    makeButton->SetSelected(true);
    singButton->SetSelected(false);
    speakButton->SetSelected(false);
    makePanel->PanelEnable();
    singPanel->PanelDisable();
    speakPanel->PanelDisable();
    main_sizer->Layout();
    selectedToolBarType = nsToolBarType::TOOLBAR_MAKE;
}

void nsMainFrame::OnSingButton(wxCommandEvent& event) {
    makeButton->SetSelected(false);
    singButton->SetSelected(true);
    speakButton->SetSelected(false);
    makePanel->PanelDisable();
    singPanel->PanelEnable();
    speakPanel->PanelDisable();
    main_sizer->Layout();
    selectedToolBarType = nsToolBarType::TOOLBAR_SING;
}

void nsMainFrame::OnSpeakButton(wxCommandEvent& event) {
    makeButton->SetSelected(false);
    singButton->SetSelected(false);
    speakButton->SetSelected(true);
    makePanel->PanelDisable();
    singPanel->PanelDisable();
    speakPanel->PanelEnable();
    main_sizer->Layout();
    selectedToolBarType = nsToolBarType::TOOLBAR_SPEAK;
}




// MARK: nsMakePanel

nsMakePanel::nsMakePanel(wxWindow* parent,
        wxWindowID winid,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name) : wxPanel(parent, winid, pos, size, style, name) {

    toolSelectorPanel = new wxPanel(this, wxID_ANY);
    toolSelectorPanel->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    voicePanel = new nsParametricVoiceMakePanel(this);
    voicePanel->Hide();
    characterPanel = new nsCharacterPanel(this);
    characterPanel->Hide();
    dictionalyPanel = new nsDictionalyPanel(this);
    dictionalyPanel->Hide();

    voice = voicePanel->GetVoice();
    characterPanel->SetVoice(voice);
    dictionalyPanel->SetVoice(voice);

    main_sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);

    voiceButton = new nsToolBarButton(toolSelectorPanel, nsID_VOICE_BUTTON, _("Voice"), wxPoint(10, 10), wxSize(100, 25));
    characterButton = new nsToolBarButton(toolSelectorPanel, nsID_CHARACTER_BUTTON, _("Character"), wxPoint(120, 10), wxSize(100, 25));
    dictionalyButton = new nsToolBarButton(toolSelectorPanel, nsID_DICTIONALY_BUTTON, _("Dictionaly"), wxPoint(230, 10), wxSize(100, 25));
    voiceButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMakePanel::OnVoiceButton, this);
    characterButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMakePanel::OnCharacterButton, this);
    dictionalyButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMakePanel::OnDictionalyButton, this);
    // toolbar_sizer->AddSpacer(110 * 2);
    toolbar_sizer->Add(voiceButton, 0, wxALL, 5);
    toolbar_sizer->Add(characterButton, 0, wxALL, 5);
    toolbar_sizer->Add(dictionalyButton, 0, wxALL, 5);

    toolSelectorPanel->SetSizer(toolbar_sizer);

    main_sizer->Add(toolSelectorPanel, 0, wxEXPAND | wxALL, 5);
    main_sizer->Add(voicePanel, 1, wxEXPAND | wxALL, 5);
    main_sizer->Add(characterPanel, 1, wxEXPAND | wxALL, 5);
    main_sizer->Add(dictionalyPanel, 1, wxEXPAND | wxALL, 5);

    SetSizer(main_sizer);

    selectedToolBarType = nsMakeToolBarType::TOOLBAR_VOICE;
    wxCommandEvent evt;
    OnVoiceButton(evt);
}

void nsMakePanel::SetVoice(NesoraMikomiVoice* voice) {
    this->voice = voice;

    voicePanel->SetVoice(voice);
    characterPanel->SetVoice(voice);
    dictionalyPanel->SetVoice(voice);
}

NesoraMikomiVoice* nsMakePanel::GetVoice() const {
    return this->voice;
}

void nsMakePanel::PanelEnable() {
    Show();
    main_sizer->Layout();
}

void nsMakePanel::PanelDisable() {
    Hide();
}

void nsMakePanel::OnVoiceButton(wxCommandEvent& event) {
    voiceButton->SetSelected(true);
    characterButton->SetSelected(false);
    dictionalyButton->SetSelected(false);
    voicePanel->PanelEnable();
    characterPanel->PanelDisable();
    dictionalyPanel->PanelDisable();
    main_sizer->Layout();
    selectedToolBarType = nsMakeToolBarType::TOOLBAR_VOICE;
}

void nsMakePanel::OnCharacterButton(wxCommandEvent& event) {
    voiceButton->SetSelected(false);
    characterButton->SetSelected(true);
    dictionalyButton->SetSelected(false);
    voicePanel->PanelDisable();
    characterPanel->PanelEnable();
    dictionalyPanel->PanelDisable();
    main_sizer->Layout();
    selectedToolBarType = nsMakeToolBarType::TOOLBAR_CHARACTER;
}

void nsMakePanel::OnDictionalyButton(wxCommandEvent& event) {
    voiceButton->SetSelected(false);
    characterButton->SetSelected(false);
    dictionalyButton->SetSelected(true);
    voicePanel->PanelDisable();
    characterPanel->PanelDisable();
    dictionalyPanel->PanelEnable();
    main_sizer->Layout();
    selectedToolBarType = nsMakeToolBarType::TOOLBAR_DICTIONALY;
}






// MARK: nsToolBarButton

nsToolBarButton::nsToolBarButton(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString& name)
                : wxButton(parent, id, label, pos, size, style, validator, name) {
    SetLabel(label);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    Bind(wxEVT_PAINT, &nsToolBarButton::onPaint, this);
    Bind(wxEVT_LEFT_UP, &nsToolBarButton::onMouseUp, this);
    Bind(wxEVT_LEFT_DOWN, &nsToolBarButton::onMouseDown, this);
    Bind(wxEVT_ENTER_WINDOW, &nsToolBarButton::onMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &nsToolBarButton::onMouseLeave, this);
}

void nsToolBarButton::onPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    if(isSelected) {
        if(mouseHover) {
            if(mouseDown)
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
            else
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_VARIANT)));
        }
        else
            dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
        dc.SetTextForeground(nsGetColor(nsColorType::ON_PRIMARY));
    }
    else {
        if(mouseHover) {
            if(mouseDown)
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
            else
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND_HOVER)));
        }
        else
            dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        dc.SetTextForeground(nsGetColor(nsColorType::ON_BACKGROUND));
    }
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    dc.SetFont(GetFont());
    wxCoord textWidth, textHeight;
    dc.GetTextExtent(GetLabel(), &textWidth, &textHeight);
    dc.DrawText(GetLabel(), (size.GetWidth() - textWidth) / 2, (size.GetHeight() - textHeight) / 2);
}

void nsToolBarButton::SetSelected(bool selected) {
    isSelected = selected;
    Refresh();
}

void nsToolBarButton::onMouseUp(wxMouseEvent& event) {
    mouseDown = false;
    wxWindow::Refresh();
    event.Skip();
}

void nsToolBarButton::onMouseDown(wxMouseEvent& event) {
    mouseDown = true;
    wxWindow::Refresh();
    event.Skip();
}
void nsToolBarButton::onMouseEnter(wxMouseEvent& event) {
    mouseHover = true;
    wxWindow::Refresh();
    event.Skip();
}
void nsToolBarButton::onMouseLeave(wxMouseEvent& event) {
    mouseHover = false;
    wxWindow::Refresh();
    event.Skip();
}

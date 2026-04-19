// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraSpeakPanel.h"

void nsSpeakPanel::Init() {
    wxStaticText* label = new wxStaticText(this, wxID_ANY, _("This is the Speak Panel"), wxPoint(10, 10));
}

void nsSpeakPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_("The process for saving the talking panel has not been implemented!"));
}

void nsSpeakPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_("The process for opening the talking panel has not been implemented!"));
}

void nsSpeakPanel::PanelEnable() {
    Show();
    menuSetup();
}

void nsSpeakPanel::PanelDisable() {
    Hide();
}

void nsSpeakPanel::menuSetup() {
    // nsID_MAIN_FRAME
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, _("&Hello...\tCtrl-H"), _("Help string shown in status bar for this menu item"));
    menuFile->Append(wxID_OPEN, _("&Open...\tCtrl-O"), _("Open a file"));
    menuFile->Append(wxID_SAVE, _("&Save\tCtrl-S"), _("Save the current file"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_IMPORT, _("&Import...\tCtrl-I"), _("Import a file"));
    menuFile->Append(ID_EXPORT, _("&Export...\tCtrl-E"), _("Export the current file"));
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
    
    wxFrame* mainFrame = (wxFrame*)wxWindow::FindWindowById(nsID_MAIN_FRAME);
    mainFrame->SetMenuBar(menuBar);

    mainFrame->Bind(wxEVT_MENU, &nsSpeakPanel::OnSave, this, wxID_SAVE);
    mainFrame->Bind(wxEVT_MENU, &nsSpeakPanel::OnOpen, this, wxID_OPEN);
}



// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraDictionalyPanel.h"

void nsDictionalyPanel::Init() {
    wxStaticText* label = new wxStaticText(this, wxID_ANY, _("This is the Dictionaly Panel"), wxPoint(10, 10));
}

void nsDictionalyPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_("The process for saving the dictionaly panel has not been implemented!"));
}

void nsDictionalyPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_("The process for opening the dictionaly panel has not been implemented!"));
}

void nsDictionalyPanel::PanelEnable() {
    Show();
    menuSetup();
}

void nsDictionalyPanel::PanelDisable() {
    Hide();
}

void nsDictionalyPanel::menuSetup() {
    // nsID_MAIN_FRAME
    wxMenu* menuFile = new wxMenu;
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

    mainFrame->Bind(wxEVT_MENU, &nsDictionalyPanel::OnSave, this, wxID_SAVE);
    mainFrame->Bind(wxEVT_MENU, &nsDictionalyPanel::OnOpen, this, wxID_OPEN);
}



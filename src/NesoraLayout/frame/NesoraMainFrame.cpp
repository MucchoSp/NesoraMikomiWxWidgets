#include "NesoraMainFrame.h"

// MARK: nsMainFrame

nsMainFrame::nsMainFrame()
    : wxFrame(nullptr, wxID_ANY, _("Nesora 1-0"), wxDefaultPosition, wxSize(1280, 720)) {
    menuSetup();
    acceleratorSetup();

    toolSelectorPanel = new wxPanel(this, wxID_ANY);
    toolSelectorPanel->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    singPanel = new nsSingPanel(this, nsID_SING_PANEL, wxDefaultPosition, wxSize(1000, 300));
    singPanel->Hide();
    speakPanel = new nsSpeakPanel(this, nsID_SPEAK_PANEL, wxDefaultPosition, wxSize(1000, 300));
    speakPanel->Hide();
    voiceMakePanel = new nsVoiceMakePanel(this, nsID_VOICE_MAKE_PANEL, wxDefaultPosition, wxSize(1000, 300));
    voiceMakePanel->Hide();

    main_sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);

    singButton = new nsToolBarButton(toolSelectorPanel, nsID_SING_BUTTON, _("Sing"), wxPoint(10, 10), wxSize(100, 30));
    speakButton = new nsToolBarButton(toolSelectorPanel, nsID_SPEAK_BUTTON, _("Talk"), wxPoint(120, 10), wxSize(100, 30));
    makeButton = new nsToolBarButton(toolSelectorPanel, nsID_VOICE_MAKE_BUTTON, _("Make Voice"), wxPoint(230, 10), wxSize(100, 30));
    singButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnSingButton, this);
    speakButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnSpeakButton, this);
    makeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnMakeButton, this);
    toolbar_sizer->Add(singButton, 0, wxALL, 5);
    toolbar_sizer->Add(speakButton, 0, wxALL, 5);
    toolbar_sizer->Add(makeButton, 0, wxALL, 5);

    toolSelectorPanel->SetSizer(toolbar_sizer);

    main_sizer->Add(toolSelectorPanel, 0, wxEXPAND | wxALL, 5);
    main_sizer->Add(singPanel, 1, wxEXPAND | wxALL, 5);
    main_sizer->Add(speakPanel, 1, wxEXPAND | wxALL, 5);
    main_sizer->Add(voiceMakePanel, 1, wxEXPAND | wxALL, 5);

    SetSizer(main_sizer);

    CreateStatusBar();
    SetStatusText("Welcome to Nesora 1-0!");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnExit, this, ID_EXIT);

    wxCommandEvent evt;
    OnMakeButton(evt);
    selectedToolBarType = nsToolBarType::TOOLBAR_VOICE_MAKE;
}

void nsMainFrame::menuSetup() {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
    menuFile->Append(wxID_SAVE, "&Save\tCtrl-S", "Save the current file");
    menuFile->Append(wxID_OPEN, "&Open...\tCtrl-O", "Open a file");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &nsMainFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &nsMainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &nsMainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &nsMainFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &nsMainFrame::OnOpen, this, wxID_OPEN);
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
    wxMessageBox("This is a wxWidgets' Hello world sample",
        "About Hello World", wxOK | wxICON_INFORMATION);
}

void nsMainFrame::OnHello(wxCommandEvent& event) {
    wxLogMessage("Hello world from wxWidgets!");
}

void nsMainFrame::OnSave(wxCommandEvent& event) {
    if(selectedToolBarType == nsToolBarType::TOOLBAR_SING) {
        singPanel->OnSave(event);
    }
    else if(selectedToolBarType == nsToolBarType::TOOLBAR_SPEAK) {
        speakPanel->OnSave(event);
    }
    else if(selectedToolBarType == nsToolBarType::TOOLBAR_VOICE_MAKE) {
        voiceMakePanel->OnSave(event);
    }
}

void nsMainFrame::OnOpen(wxCommandEvent& event) {
    if(selectedToolBarType == nsToolBarType::TOOLBAR_VOICE_MAKE) {
        voiceMakePanel->OnOpen(event);
    }
}

void nsMainFrame::OnSingButton(wxCommandEvent& event) {
    singButton->SetSelected(true);
    speakButton->SetSelected(false);
    makeButton->SetSelected(false);
    singPanel->Show();
    speakPanel->Hide();
    voiceMakePanel->Hide();
    main_sizer->Layout();
    selectedToolBarType = nsToolBarType::TOOLBAR_SING;
}

void nsMainFrame::OnSpeakButton(wxCommandEvent& event) {
    singButton->SetSelected(false);
    speakButton->SetSelected(true);
    makeButton->SetSelected(false);
    singPanel->Hide();
    speakPanel->Show();
    voiceMakePanel->Hide();
    main_sizer->Layout();
    selectedToolBarType = nsToolBarType::TOOLBAR_SPEAK;
}

void nsMainFrame::OnMakeButton(wxCommandEvent& event) {
    singButton->SetSelected(false);
    speakButton->SetSelected(false);
    makeButton->SetSelected(true);
    singPanel->Hide();
    speakPanel->Hide();
    voiceMakePanel->Show();
    main_sizer->Layout();
    selectedToolBarType = nsToolBarType::TOOLBAR_VOICE_MAKE;
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
    Connect(wxEVT_PAINT, wxPaintEventHandler(nsToolBarButton::onPaint));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(nsToolBarButton::onMouseUp));
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(nsToolBarButton::onMouseDown));
    Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(nsToolBarButton::onMouseEnter));
    Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(nsToolBarButton::onMouseLeave));
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

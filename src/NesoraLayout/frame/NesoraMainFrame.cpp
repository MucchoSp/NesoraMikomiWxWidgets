#include "NesoraMainFrame.h"

// MARK: nsMainFrame

nsMainFrame::nsMainFrame()
    : wxFrame(nullptr, wxID_ANY, _("Nesora 1-0"), wxDefaultPosition, wxSize(1280, 720)) {
    menuSetup();

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
    SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnExit, this, ID_EXIT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnSingButton, this, nsID_SING_BUTTON);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnSpeakButton, this, nsID_SPEAK_BUTTON);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsMainFrame::OnMakeButton, this, nsID_VOICE_MAKE_BUTTON);

    wxCommandEvent evt;
    OnMakeButton(evt);
}

void nsMainFrame::menuSetup() {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
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

void nsMainFrame::OnSingButton(wxCommandEvent& event) {
    singButton->SetSelected(true);
    speakButton->SetSelected(false);
    makeButton->SetSelected(false);
    singPanel->Show();
    speakPanel->Hide();
    voiceMakePanel->Hide();
    main_sizer->Layout();
}

void nsMainFrame::OnSpeakButton(wxCommandEvent& event) {
    singButton->SetSelected(false);
    speakButton->SetSelected(true);
    makeButton->SetSelected(false);
    singPanel->Hide();
    speakPanel->Show();
    voiceMakePanel->Hide();
    main_sizer->Layout();
}

void nsMainFrame::OnMakeButton(wxCommandEvent& event) {
    singButton->SetSelected(false);
    speakButton->SetSelected(false);
    makeButton->SetSelected(true);
    singPanel->Hide();
    speakPanel->Hide();
    voiceMakePanel->Show();
    main_sizer->Layout();
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

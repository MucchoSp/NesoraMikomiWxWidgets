#pragma once
#include <wx/wx.h>

#include "../../NesoraStyle/button/NesoraButton.h"
#include "../../NesoraStyle/NesoraColor.h"
#include "../../NesoraLayout/NesoraIDs.h"
#include "../sing/NesoraSingPanel.h"
#include "../speak/NesoraSpeakPanel.h"
#include "../voicemake/NesoraVoiceMakePanel.h"

#ifndef NESORA_MAIN_FRAME_H
#define NESORA_MAIN_FRAME_H


enum class nsToolBarType {
    TOOLBAR_SPEAK,
    TOOLBAR_SING,
    TOOLBAR_VOICE_MAKE
};

class nsToolBarButton;

// MARK: nsMainFrame

class nsMainFrame : public wxFrame {
public:
    nsMainFrame();

private:

    void menuSetup();
    void acceleratorSetup();

    wxPanel* toolSelectorPanel;
    nsToolBarType selectedToolBarType;

    nsToolBarButton* singButton;
    nsToolBarButton* speakButton;
    nsToolBarButton* makeButton;

    nsSingPanel* singPanel;
    nsSpeakPanel* speakPanel;
    nsVoiceMakePanel* voiceMakePanel;

    wxSizer* main_sizer;

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnHello(wxCommandEvent& event);
    void OnSingButton(wxCommandEvent& event);
    void OnSpeakButton(wxCommandEvent& event);
    void OnMakeButton(wxCommandEvent& event);

    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
};



// MARK: nsToolBarButton

class nsToolBarButton : public wxButton {
public:
    nsToolBarButton(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxString& label = "",
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxASCII_STR(wxControlNameStr));

    void SetSelected(bool selected);

private:
    bool mouseDown = false;
    bool mouseHover = false;
    bool isSelected = false;

    void onPaint(wxPaintEvent& event);
    void onMouseUp(wxMouseEvent& event);
    void onMouseDown(wxMouseEvent& event);
    void onMouseEnter(wxMouseEvent& event);
    void onMouseLeave(wxMouseEvent& event);
};


#endif // NESORA_MAIN_FRAME_H

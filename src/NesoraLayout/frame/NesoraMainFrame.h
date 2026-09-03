// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <wx/wx.h>

#include "../../NesoraStyle/button/NesoraButton.h"
#include "../../NesoraStyle/NesoraColor.h"
#include "../../NesoraLayout/NesoraIDs.h"
#include "../sing/NesoraSingPanel.h"
#include "../speak/NesoraSpeakPanel.h"
#include "../make/voice/parametric/NesoraParametricVoiceMakePanel.h"
#include "../make/voice/phonetic/NesoraPhoneticVoiceMakePanel.h"
#include "../make/character/NesoraCharacterPanel.h"
#include "../make/dictionaly/NesoraDictionalyPanel.h"

#ifndef NESORA_MAIN_FRAME_H
#define NESORA_MAIN_FRAME_H


enum class nsToolBarType {
    TOOLBAR_MAKE,
    TOOLBAR_SPEAK,
    TOOLBAR_SING,
};

enum class nsMakeToolBarType {
    TOOLBAR_VOICE,
    TOOLBAR_CHARACTER,
    TOOLBAR_DICTIONALY,
};

class nsToolBarButton;
class nsMakePanel;

// MARK: nsMainFrame

class nsMainFrame : public wxFrame {
public:
    nsMainFrame();

private:
    NesoraMikomiVoice* voice;

    void menuSetup();
    void acceleratorSetup();

    wxPanel* toolSelectorPanel;
    nsToolBarType selectedToolBarType;

    nsToolBarButton* makeButton;
    nsToolBarButton* singButton;
    nsToolBarButton* speakButton;

    nsMakePanel* makePanel;
    nsSingPanel* singPanel;
    nsSpeakPanel* speakPanel;

    wxSizer* main_sizer;

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnMakeButton(wxCommandEvent& event);
    void OnSingButton(wxCommandEvent& event);
    void OnSpeakButton(wxCommandEvent& event);
};


class nsMakePanel : public wxPanel {
public:
    nsMakePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr));

    void SetVoice(NesoraMikomiVoice* voice);
    NesoraMikomiVoice* GetVoice() const;

    void PanelEnable();
    void PanelDisable();

private:
    NesoraMikomiVoice* voice;

    wxPanel* toolSelectorPanel;
    nsMakeToolBarType selectedToolBarType;

    nsToolBarButton* voiceButton;
    nsToolBarButton* characterButton;
    nsToolBarButton* dictionalyButton;

    nsVoiceMakePanelBase* voicePanel;
    nsCharacterPanel* characterPanel;
    nsDictionalyPanel* dictionalyPanel;

    wxSizer* main_sizer;

    void OnVoiceButton(wxCommandEvent& event);
    void OnCharacterButton(wxCommandEvent& event);
    void OnDictionalyButton(wxCommandEvent& event);
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

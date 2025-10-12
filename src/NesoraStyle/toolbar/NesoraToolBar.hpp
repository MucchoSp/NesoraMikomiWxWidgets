#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>

#include "../../NesoraLayout/NesoraIDs.h"

#include "../NesoraColor.h"

#ifndef NESORA_TOOLBAR_H
#define NESORA_TOOLBAR_H

enum class nsToolBarType {
    TOOLBAR_SPEAK,
    TOOLBAR_SING,
    TOOLBAR_VOICE_MAKE
};

class nsToolBarButton : public wxButton {
public:
    nsToolBarButton(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxString& label = "",
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxASCII_STR(wxControlNameStr))
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

class nsToolBar : public wxPanel {
public:
    nsToolBar(wxWindow *parent,
              wxWindowID id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL,
              const wxString& name = wxASCII_STR(wxPanelNameStr))
              : wxPanel(parent, id, pos, size, style, name) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

        wxSizer* tool_sizer = new wxBoxSizer(wxHORIZONTAL);

        speakButton = new nsToolBarButton(this, nsID_TOOLBAR_SPEAK_BUTTON, "話す", wxDefaultPosition, wxSize(80, size.GetHeight()));
        singButton = new nsToolBarButton(this, nsID_TOOLBAR_SING_BUTTON, "歌う", wxDefaultPosition, wxSize(80, size.GetHeight()));
        voiceMakeButton = new nsToolBarButton(this, nsID_TOOLBAR_VOICE_MAKE_BUTTON, "作る", wxDefaultPosition, wxSize(80, size.GetHeight()));

        tool_sizer->Add(speakButton, 0, wxALL, 5);
        tool_sizer->Add(singButton, 0, wxALL, 5);
        tool_sizer->Add(voiceMakeButton, 0, wxALL, 5);

        SetSizer(tool_sizer);

        SetSelectedTool(nsToolBarType::TOOLBAR_SPEAK);
        speakButton->SetSelected(true);

        Connect(wxEVT_PAINT, wxPaintEventHandler(nsToolBar::onPaint));
        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsToolBar::onSpeakButtonClicked, this, nsID_TOOLBAR_SPEAK_BUTTON);
        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsToolBar::onSingButtonClicked, this, nsID_TOOLBAR_SING_BUTTON);
        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsToolBar::onVoiceMakeButtonClicked, this, nsID_TOOLBAR_VOICE_MAKE_BUTTON);
    }

    void SetSelectedTool(nsToolBarType tool);

    nsToolBarType GetSelectedTool() const;

private:

    nsToolBarType nowSelected = nsToolBarType::TOOLBAR_SPEAK;

    nsToolBarButton* speakButton;
    nsToolBarButton* singButton;
    nsToolBarButton* voiceMakeButton;

    void onPaint(wxPaintEvent& event);
    void onSpeakButtonClicked(wxCommandEvent& event);
    void onSingButtonClicked(wxCommandEvent& event);
    void onVoiceMakeButtonClicked(wxCommandEvent& event);
};
#endif // NESORA_TOOLBAR_H

#pragma once

#ifndef NESORA_TOOL_CHANGE_PANEL_H
#define NESORA_TOOL_CHANGE_PANEL_H

#include <wx/wx.h>

#include "../../NesoraStyle/button/NesoraButton.h"
#include "../../NesoraStyle/NesoraColor.h"


class nsVoiceMakePanel : public wxPanel {
public:
    nsVoiceMakePanel() {
        Init();
    }

    nsVoiceMakePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }

    void Init();

private:

};

enum {
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_T1SLIDER = 100,
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_T2SLIDER,
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_PLAY_BUTTON,
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_STOP_BUTTON
};

#endif // NESORA_TOOL_CHANGE_PANEL_H

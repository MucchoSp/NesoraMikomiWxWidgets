#pragma once

#ifndef NESORA_TOOL_CHANGE_PANEL_H
#define NESORA_TOOL_CHANGE_PANEL_H

#include <wx/wx.h>

#include "../../NesoraStyle/button/NesoraButton.h"
#include "../../NesoraStyle/NesoraColor.h"


class nsToolChangePanel : public wxPanel {
public:
    nsToolChangePanel() {
        Init();
    }

    nsToolChangePanel(wxWindow* parent,
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

    nsButton* speakButton;
    nsButton* makeButton;

};

#endif // NESORA_TOOL_CHANGE_PANEL_H

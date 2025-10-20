#pragma once

#ifndef NESORA_VOICE_MAKE_PANEL_H
#define NESORA_VOICE_MAKE_PANEL_H

#include <wx/wx.h>

#include <sstream>

#include "../../NesoraStyle/NesoraStyle.h"

#include "NesoraRosenbergWavePanel.h"
#include "NesoraFormantFilterPanel.h"
#include "../NesoraIDs.h"

#include "../../Nesora/source/NesoraSource.h"







// MARK:nsVoiceMakePanel

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

    std::vector<double> wave;// 48000 / 261.6

    wxPanel* sourceSoundPanel;
    wxPanel* filterPanel;

};

#endif // NESORA_VOICE_MAKE_PANEL_H

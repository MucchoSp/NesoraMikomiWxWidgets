#pragma once

#ifndef NESORA_SING_PANEL_H
#define NESORA_SING_PANEL_H

#include <wx/wx.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "../../NesoraStyle/button/NesoraButton.h"
#include "../../NesoraStyle/NesoraColor.h"
#include "../../NesoraStyle/chart/NesoraSimpleChart.h"
#include "../../NesoraStyle/slider/NesoraSlider.h"
#include "../../NesoraLayout/NesoraIDs.h"

#include "../../Nesora/source/NesoraSource.h"

class nsSingPanel : public wxPanel {
public:
    nsSingPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();
private:
};

#endif // NESORA_SING_PANEL_H
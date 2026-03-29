// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_SING_PANEL_H
#define NESORA_SING_PANEL_H

#include <wx/wx.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "../../NesoraStyle/NesoraStyle.h"
#include "../../NesoraLayout/NesoraIDs.h"

#include "../../Nesora/Nesora.h"

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
    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);

private:
};

#endif // NESORA_SING_PANEL_H
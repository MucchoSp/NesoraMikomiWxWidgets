// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_DICTIONALY_PANEL_H
#define NESORA_DICTIONALY_PANEL_H

#include <wx/wx.h>

#include "../../NesoraStyle/NesoraStyle.h"
#include "../../NesoraLayout/NesoraIDs.h"

#include "../../Nesora/Nesora.h"

class nsDictionalyPanel : public wxPanel {
public:
    nsDictionalyPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();
    void SetVoice(NesoraMikomiVoice* voice) { this->voice = voice; }
    NesoraMikomiVoice* GetVoice() const { return voice; }
    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);

    void PanelEnable();
    void PanelDisable();

private:
    NesoraMikomiVoice* voice;

    void menuSetup();
    
};

#endif // NESORA_DICTIONALY_PANEL_H
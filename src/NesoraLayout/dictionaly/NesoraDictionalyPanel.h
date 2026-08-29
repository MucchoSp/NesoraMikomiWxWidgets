// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_DICTIONALY_PANEL_H
#define NESORA_DICTIONALY_PANEL_H

#include <wx/wx.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "NesoraDictionalyIndexPanel.h"
#include "NesoraDictionalyEditPanel.h"
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
    void SetVoice(NesoraMikomiParametricVoice* voice);
    NesoraMikomiParametricVoice* GetVoice() const { return voice; }
    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);

    void PanelEnable();
    void PanelDisable();

private:
    NesoraMikomiParametricVoice* voice;

    NesoraDictionalyIndexPanel* index;
    NesoraDictionalyEditPanel* editDictionalyPanel;

    void menuSetup();
    
};

#endif // NESORA_DICTIONALY_PANEL_H
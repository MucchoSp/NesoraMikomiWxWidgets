// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_SING_PANEL_H
#define NESORA_SING_PANEL_H

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/bmpbuttn.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "NesoraMIDIPanel.h"
#include "NesoraVelocityPanel.h"
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
    void ToolBar();
    void OnStop(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnCharHook(wxKeyEvent& event);
    void OnVolumeSlide(wxCommandEvent& event);

private:
    NesoraMikomiVoice* voice;

    NesoraMIDIPanel* midiPanel;
    NesoraVelocityPanel* velocity;
    wxToolBar* toolbar;
    nsSlider* volume;
    wxStaticText* volumeText;

    bool isPlaying = false;

    void InitAudioDevice();
    void UninitAudioDevice();

    ma_device device;
    ma_device_config deviceConfig;

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

};

#endif // NESORA_SING_PANEL_H
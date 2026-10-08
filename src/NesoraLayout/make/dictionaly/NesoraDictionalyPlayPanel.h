// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_DICTIONALY_PLAY_PANEL_H
#define NESORA_DICTIONALY_PLAY_PANEL_H

#include <wx/wx.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../../lib/miniaudio_split/miniaudio.h"

#include "../../../NesoraStyle/NesoraStyle.h"
#include "../../../NesoraLayout/NesoraIDs.h"

#include "../../../Nesora/Nesora.h"

// MARK: NesoraDictionalyPlayPanel

class NesoraDictionalyIndexWord;

class NesoraDictionalyPlayPanel : public wxPanel {
public:
    NesoraDictionalyPlayPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();
    void SetVoice(NesoraMikomiVoice* voice) { this->voice = voice; }
    void SetWord(NesoraDictionalyIndexWord* word) { this->indexWord = word; }
    NesoraDictionalyIndexWord* GetWord() const { return indexWord; }

    void OnPlay(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnVolumeChange(wxCommandEvent& event);
    void OnPitchChange(wxCommandEvent& event);
    void OnLengthChange(wxCommandEvent& event);

private:
    NesoraMikomiVoice* voice;
    NesoraDictionalyIndexWord* indexWord;

    wxToolBar* toolbar;
    nsSlider* volume;
    wxStaticText* volumeText;
    nsSlider* pitch;
    wxStaticText* pitchText;
    nsSlider* length;
    wxStaticText* lengthText;

    bool isPlaying = false;

    int InitAudioDevice();
    void UninitAudioDevice();

    ma_device device;
    ma_device_config deviceConfig;

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
};



#endif // NESORA_DICTIONALY_PLAY_PANEL_H

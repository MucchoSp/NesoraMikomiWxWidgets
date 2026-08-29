// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_PHONETIC_VOICE_MAKE_PANEL_H
#define NESORA_PHONETIC_VOICE_MAKE_PANEL_H

#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../../lib/miniaudio_split/miniaudio.h"

#include "../../../NesoraStyle/NesoraStyle.h"

#include "../NesoraVoiceMakePanelBase.h"
#include "NesoraPhoneticFilterPanel.h"
#include "NesoraPhoneticLFModelPanel.h"

#include "../../NesoraIDs.h"
#include "../../../Nesora/Nesora.h"




class nsPhoneticVoiceMakePlayInterfacePanel : public wxPanel {
public:
    nsPhoneticVoiceMakePlayInterfacePanel() {
        Init();
    }

    nsPhoneticVoiceMakePlayInterfacePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }

    void Init();

    wxButton* playButton;
    wxButton* stopButton;
    wxStaticText* statusText;
    nsSlider* volume;

private:

    void OnVolumeSlide(wxCommandEvent& event);

};


// MARK:nsPhoneticVoiceMakePanel

class nsPhoneticVoiceMakePanel : public nsParametricVoiceMakePanelBase {
public:
    nsPhoneticVoiceMakePanel() {
        Init();
    }

    nsPhoneticVoiceMakePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : nsParametricVoiceMakePanelBase(parent, winid, pos, size, style, name)
    {
        Init();
    }

    ~nsPhoneticVoiceMakePanel() {
        UninitAudioDevice();
    }

    void Init();
    void SetVoice(NesoraMikomiParametricVoice* voice) { this->voice = voice; }
    NesoraMikomiParametricVoice* GetVoice() const { return voice; }
    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);

    void PanelEnable();
    void PanelDisable();

private:

    std::vector<double> wave;// 48000 / 261.6

    nsSourcePanelBase* sourceSoundPanel;
    nsParametricFilterPanelBase* filterPanel;
    nsPhoneticVoiceMakePlayInterfacePanel* playInterfacePanel;
    // nsParametricPanel* parametricPanel;

    NesoraMikomiParametricVoice* voice;
    ParametricNesoraDelta parameters;

    void menuSetup();
    
    void OnPlayButtonClicked(wxCommandEvent& event);
    void OnStopButtonClicked(wxCommandEvent& event);

    void InitAudioDevice();
    void UninitAudioDevice();

    ma_device device;
    ma_device_config deviceConfig;

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    bool isPlaying = false;

};

#endif // NESORA_PHONETIC_VOICE_MAKE_PANEL_H

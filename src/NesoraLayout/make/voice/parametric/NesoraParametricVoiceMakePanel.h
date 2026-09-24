// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_PARAMETRIC_VOICE_MAKE_PANEL_H
#define NESORA_PARAMETRIC_VOICE_MAKE_PANEL_H

#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

#include <sstream>

#include "../../../../NesoraStyle/NesoraStyle.h"

#include "../NesoraVoiceMakePanelBase.h"
#include "NesoraParametricRosenbergWavePanel.h"
#include "NesoraParametricLFModelPanel.h"
#include "NesoraParametricSOSIIRFilterPanel.h"
#include "NesoraParametricPanel.h"

#include "../../../NesoraIDs.h"
#include "../../../../Nesora/Nesora.h"




class nsParametricVoiceMakePlayInterfacePanel : public wxPanel {
public:
    nsParametricVoiceMakePlayInterfacePanel() {
        Init();
    }

    nsParametricVoiceMakePlayInterfacePanel(wxWindow* parent,
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


// MARK:nsParametricVoiceMakePanel

class nsParametricVoiceMakePanel : public nsVoiceMakePanelBase {
public:
    nsParametricVoiceMakePanel() {
        Init();
    }

    nsParametricVoiceMakePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : nsVoiceMakePanelBase(parent, winid, pos, size, style, name)
    {
        Init();
    }

    ~nsParametricVoiceMakePanel() {
        UninitAudioDevice();
    }

    void Init();
    void Update() override;
    void SetVoice(NesoraMikomiVoice* voice) override;
    NesoraMikomiVoice* GetVoice() const override { return voice; }
    void OnSave(wxCommandEvent& event) override;
    void OnOpen(wxCommandEvent& event) override;

    void PanelEnable() override;
    void PanelDisable() override;

    void SetSelectedParameterID(uint32_t ID) override;

private:

    std::vector<double> wave;

    NesoraMikomiVoice* voice;
    nsSourcePanelBase* sourcePanel;
    nsFilterPanelBase* filterPanel;
    nsParametricVoiceMakePlayInterfacePanel* playInterfacePanel;
    nsParametricPanel* parametricPanel;

    void menuSetup();
    
    void OnPlayButtonClicked(wxCommandEvent& event);
    void OnStopButtonClicked(wxCommandEvent& event);

    void InitAudioDevice();
    void UninitAudioDevice();

    ma_device device;
    ma_device_config deviceConfig;

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    bool isPlaying = false;
    uint32_t nowParameterID = 0;

};

#endif // NESORA_PARAMETRIC_VOICE_MAKE_PANEL_H



// TODO: sourcePanel と filterPanel にも NesroaMikomiVoice を渡せるようにして、その声を介してパラメーターを操作できるようにする。
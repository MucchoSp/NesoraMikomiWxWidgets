#pragma once

#ifndef NESORA_VOICE_MAKE_PANEL_H
#define NESORA_VOICE_MAKE_PANEL_H

#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

#include <sstream>

#include "../../NesoraStyle/NesoraStyle.h"

#include "NesoraRosenbergWavePanel.h"
#include "NesoraFormantFilterPanel.h"
#include "NesoraIIRFilterPanel.h"
#include "../NesoraIDs.h"

#include "../../Nesora/source/NesoraSource.h"




class nsVoiceMakePlayInterfacePanel : public wxPanel {
public:
    nsVoiceMakePlayInterfacePanel() {
        Init();
    }

    nsVoiceMakePlayInterfacePanel(wxWindow* parent,
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

    ~nsVoiceMakePanel() {
        UninitAudioDevice();
    }

    void Init();
    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);

private:

    std::vector<double> wave;// 48000 / 261.6

    nsRosenbergWavePanel* sourceSoundPanel;
    nsIIRFilterPanel* filterPanel;
    nsVoiceMakePlayInterfacePanel* playInterfacePanel;
    NesoraMikomiVoice* voice;

    void OnPlayButtonClicked(wxCommandEvent& event);
    void OnStopButtonClicked(wxCommandEvent& event);

    void InitAudioDevice();
    void UninitAudioDevice();

    ma_device device;
    ma_device_config deviceConfig;

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

};

#endif // NESORA_VOICE_MAKE_PANEL_H

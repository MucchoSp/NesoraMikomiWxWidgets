#pragma once

#ifndef NESORA_VOICE_MAKE_PANEL_H
#define NESORA_VOICE_MAKE_PANEL_H

#include <wx/wx.h>

#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "../../NesoraStyle/button/NesoraButton.h"
#include "../../NesoraStyle/chart/NesoraChart.h"
#include "../../NesoraStyle/slider/NesoraSlider.h"
#include "../../NesoraStyle/NesoraColor.h"

#include "../../Nesora/source/NesoraSource.h"


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

    void RosenbergWavePanelSetup();

private:

    nsChartControl* chart;

    wxStaticText* t1param;
    wxStaticText* t2param;
    nsSlider* t1slider;
    nsSlider* t2slider;
    nsButton* playButton;
    nsButton* stopButton;


    NesoraRosenbergWave source_wave;
    std::vector<double> wave;// 48000 / 261.6

    ma_device device;
    ma_device_config deviceConfig;


    void OnT1Slide(wxCommandEvent& event);
    void OnT2Slide(wxCommandEvent& event);

    void OnPlayButton(wxCommandEvent& event);
    void OnStopButton(wxCommandEvent& event);


    void InitAudioDevice();
    void UninitAudioDevice();

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

};

enum {
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_T1SLIDER = 100,
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_T2SLIDER,
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_PLAY_BUTTON,
    ID_VOICEMAKEPANEL_ROSENBERGWAVE_STOP_BUTTON
};

#endif // NESORA_VOICE_MAKE_PANEL_H

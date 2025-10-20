#pragma once

#ifndef NESORA_ROSENBERG_WAVE_PANEL_H
#define NESORA_ROSENBERG_WAVE_PANEL_H

#include <wx/wx.h>

#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "../../NesoraStyle/NesoraStyle.h"

#include "../NesoraIDs.h"

#include "../../Nesora/Nesora.h"



class nsRosenbergWavePanel : public wxPanel {
public:
    nsRosenbergWavePanel() {
        Init();
    }

    nsRosenbergWavePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }


    ~nsRosenbergWavePanel() {
        UninitAudioDevice();
    }

    void Init();
    
    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
private:
    nsSimpleChartControl* chart;

    wxStaticText* pitch_param;
    wxStaticText* t1param;
    wxStaticText* t2param;
    nsSlider* pitch_slider;
    nsSlider* t1slider;
    nsSlider* t2slider;
    nsButton* playButton;
    nsButton* stopButton;

    NesoraRosenbergWave source_wave;
    std::vector<double> wave;// 48000 / 261.6

    ma_device device;
    ma_device_config deviceConfig;

    void OnPitchSlide(wxCommandEvent& event);
    void OnT1Slide(wxCommandEvent& event);
    void OnT2Slide(wxCommandEvent& event);

    void OnPlayButton(wxCommandEvent& event);
    void OnStopButton(wxCommandEvent& event);

    void InitAudioDevice();
    void UninitAudioDevice();
};

#endif // NESORA_ROSENBERG_WAVE_PANEL_H

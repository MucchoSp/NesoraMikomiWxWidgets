#pragma once
#include <wx/wx.h>
#include <wx/sound.h>

#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "../lib/miniaudio.h"

#include "NesoraStyle/button/NesoraButton.h"
#include "NesoraStyle/chart/NesoraChart.h"
#include "NesoraStyle/slider/NesoraSlider.h"
#include "NesoraStyle/NesoraColor.h"

#include "Nesora/source/NesoraSource.h"

class MyApp : public wxApp {
public:
    bool OnInit() override;
};

class MyFrame : public wxFrame {
public:
    MyFrame();

private:
    void MenuSetup();
    void RosenbergWavePanelSetup();


    wxPanel* globalPanel;

    nsChartControl* chart;

    wxStaticText* t1param;
    wxStaticText* t2param;
    nsSlider* t1slider;
    nsSlider* t2slider;
    nsButton* playButton;
    nsButton* stopButton;

    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnT1Slide(wxCommandEvent& event);
    void OnT2Slide(wxCommandEvent& event);

    void OnPlayButton(wxCommandEvent& event);
    void OnStopButton(wxCommandEvent& event);


    NesoraRosenbergWave source_wave;
    std::vector<double> wave;// 48000 / 261.6

    ma_device device;
    ma_device_config deviceConfig;

    
    void InitAudioDevice();
    void UninitAudioDevice();

    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    
};

enum {
    ID_Hello = 1,
    ID_EXIT,
    ID_T1SLIDER,
    ID_T2SLIDER,
    ID_PLAY_BUTTON,
    ID_STOP_BUTTON
};

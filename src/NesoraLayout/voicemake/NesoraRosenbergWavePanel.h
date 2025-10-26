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
    }

    void Init();
    void Update();
    
    std::vector<double> GetWave() const;
    NesoraRosenbergWave* GetSource();

    double GetPitch() const;

private:
    nsSimpleChartControl* chart;

    wxStaticText* pitch_param;
    wxStaticText* t1param;
    wxStaticText* t2param;
    nsSlider* pitch_slider;
    nsSlider* t1slider;
    nsSlider* t2slider;

    NesoraRosenbergWave* source_wave;
    std::vector<double> wave;// 48000 / 261.6

    void OnPitchSlide(wxCommandEvent& event);
    void OnT1Slide(wxCommandEvent& event);
    void OnT2Slide(wxCommandEvent& event);
};

#endif // NESORA_ROSENBERG_WAVE_PANEL_H

#pragma once

#ifndef NESORA_PARAMETRIC_ROSENBERG_WAVE_PANEL_H
#define NESORA_PARAMETRIC_ROSENBERG_WAVE_PANEL_H

#include <wx/wx.h>

#include <sstream>
#include <random>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../../lib/miniaudio_split/miniaudio.h"

#include "../../../NesoraStyle/NesoraStyle.h"

#include "../../NesoraIDs.h"
#include "../../NesoraEvents.h"

#include "../../../Nesora/Nesora.h"
#include "../../../Nesora/source/NesoraParametricRosenbergWave/NesoraParametricRosenbergWave.h"



class nsParametricRosenbergWavePanel : public wxPanel {
public:
    nsParametricRosenbergWavePanel() {
        Init();
    }

    nsParametricRosenbergWavePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }


    ~nsParametricRosenbergWavePanel() {
    }

    void Init();
    void Update();
    
    std::vector<double> GetWave() const;
    NesoraParametricRosenbergWave* GetSource();

    double GetPitch() const;

private:
    nsSimpleChartControl* chart;

    wxStaticText* pitch_param;
    wxStaticText* phonetic_param;
    wxStaticText* t1param;
    wxStaticText* t2param;
    nsSlider* pitch_slider;
    nsSlider* phonetic_slider;
    nsSlider* t1slider;
    nsSlider* t2slider;

    NesoraParametricRosenbergWave* source_wave;
    std::vector<double> wave;// 48000 / 261.6

    uint32_t nowSelectedParameter = 0;
    std::map<uint32_t, double> parameters;
    
    void OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event);
    void OnChangeParameter(nsParameterChangeEvent& event);
    void OnAddParameter(nsAddParameterEvent& event);

    void OnPitchSlide(wxCommandEvent& event);
    void OnPhoneticSlide(wxCommandEvent& event);
    void OnT1Slide(wxCommandEvent& event);
    void OnT2Slide(wxCommandEvent& event);
};

#endif // NESORA_PARAMETRIC_ROSENBERG_WAVE_PANEL_H

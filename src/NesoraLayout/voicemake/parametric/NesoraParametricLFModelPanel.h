// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_PARAMETRIC_LF_MODEL_PANEL_H
#define NESORA_PARAMETRIC_LF_MODEL_PANEL_H

#include <wx/wx.h>

#include <sstream>
#include <random>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../../lib/miniaudio_split/miniaudio.h"

#include "../../../NesoraStyle/NesoraStyle.h"

#include "../NesoraVoiceMakePanelBase.h"
#include "../../NesoraIDs.h"
#include "../../NesoraEvents.h"

#include "../../../Nesora/Nesora.h"
#include "../../../Nesora/source/NesoraParametricRosenbergWave/NesoraParametricLFModel.h"


class nsParametricLFModelPanel : public nsSourcePanelBase {
public:
    nsParametricLFModelPanel() {
        Init();
    }

    nsParametricLFModelPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : nsSourcePanelBase(parent, winid, pos, size, style, name)
    {
        Init();
    }

    void Init() override;
    void Update() override;
    
    std::vector<double> GetWave() const;
    NesoraSourceBase* GetSource() override;

    double GetPitch() const override;

private:
    nsSimpleChartControl* chart;
    nsSimpleChartControl* chart_integral;

    wxStaticText* pitch_param;
    wxStaticText* phonetic_param;
    wxStaticText* tpparam;
    wxStaticText* teparam;
    wxStaticText* taparam;
    wxStaticText* eeparam;
    nsSlider* pitch_slider;
    nsSlider* phonetic_slider;
    nsSlider* tpslider;
    nsSlider* teslider;
    nsSlider* taslider;
    nsSlider* eeslider;

    double Tp, Te, Ta, Ee, noise_level;
    NesoraParametricLFModel* source_wave;
    std::vector<double> wave;// 48000 / 261.6
    std::vector<double> wave_integral;// 48000 / 261.6

    uint32_t nowSelectedParameter = 0;
    std::map<uint32_t, double> parameters;
    
    void OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event);
    void OnChangeParameter(nsParameterChangeEvent& event);
    void OnAddParameter(nsAddParameterEvent& event);

    void OnPitchSlide(wxCommandEvent& event);
    void OnPhoneticSlide(wxCommandEvent& event);
    void OnTpSlide(wxCommandEvent& event);
    void OnTeSlide(wxCommandEvent& event);
    void OnTaSlide(wxCommandEvent& event);
    void OnEeSlide(wxCommandEvent& event);

    void UpdateChart();
};

class nsParametricLFModelRdParameterPanel : public nsSourcePanelBase {
public:
    nsParametricLFModelRdParameterPanel() {
        Init();
    }

    nsParametricLFModelRdParameterPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : nsSourcePanelBase(parent, winid, pos, size, style, name)
    {
        Init();
    }

    void Init() override;
    void Update() override;
    
    std::vector<double> GetWave() const;
    NesoraSourceBase* GetSource() override;

    double GetPitch() const override;

private:
    nsSimpleChartControl* chart;

    wxStaticText* pitch_param;
    wxStaticText* phonetic_param;
    wxStaticText* rdparam;
    wxStaticText* eeparam;
    nsSlider* pitch_slider;
    nsSlider* phonetic_slider;
    nsSlider* rdslider;
    nsSlider* eeslider;

    double Rd, Ee, noise_level;
    NesoraParametricLFModelRdParameter* source_wave;
    std::vector<double> wave;// 48000 / 261.6

    uint32_t nowSelectedParameter = 0;
    std::map<uint32_t, double> parameters;
    
    void OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event);
    void OnChangeParameter(nsParameterChangeEvent& event);
    void OnAddParameter(nsAddParameterEvent& event);

    void OnPitchSlide(wxCommandEvent& event);
    void OnPhoneticSlide(wxCommandEvent& event);
    void OnRdSlide(wxCommandEvent& event);
    void OnEeSlide(wxCommandEvent& event);

    void UpdateChart();
};

#endif // NESORA_PARAMETRIC_LF_MODEL_PANEL_H

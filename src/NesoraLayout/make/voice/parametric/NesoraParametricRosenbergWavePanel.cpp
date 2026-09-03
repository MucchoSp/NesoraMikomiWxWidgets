// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraParametricRosenbergWavePanel.h"

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}



void nsParametricRosenbergWavePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    source_wave = new NesoraParametricRosenbergWave();

    source_wave->SetParamater(0.25, 0.5, -0.5, 0.1);
    source_wave->SetParamater(parameters);
    wave.resize(183);// 48000/261.6
    double throw_away;
    for (size_t i = 0;i < wave.size();i++)
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Rosenberg Wave"));
    wxSizer* pitch_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* phonetic_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t1sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t2sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sliderSizer = new wxBoxSizer(wxVERTICAL);

    pitch_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "261 Hz");
    pitch_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    pitch_sliderSizer->Add(pitch_param, 0, wxEXPAND | wxALL, 5);
    pitch_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 261, 20, 1000, wxDefaultPosition, wxSize(200, 15));
    pitch_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricRosenbergWavePanel::OnPitchSlide, this);
    pitch_slider->Show();
    pitch_sliderSizer->Add(pitch_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(pitch_sliderSizer, 0, wxEXPAND | wxALL);

    phonetic_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.100");
    phonetic_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    phonetic_sliderSizer->Add(phonetic_param, 0, wxEXPAND | wxALL, 5);
    phonetic_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 100, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    phonetic_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricRosenbergWavePanel::OnPhoneticSlide, this);
    phonetic_slider->Show();
    phonetic_sliderSizer->Add(phonetic_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(phonetic_sliderSizer, 0, wxEXPAND | wxALL);

    t1param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.250");
    t1param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t1sliderSizer->Add(t1param, 0, wxEXPAND | wxALL, 5);
    t1slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 250, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    t1slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricRosenbergWavePanel::OnT1Slide, this);
    t1slider->SetLimit(0, 500);
    t1slider->Show();
    t1sliderSizer->Add(t1slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(t1sliderSizer, 0, wxEXPAND | wxALL);

    t2param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.500");
    t2param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t2sliderSizer->Add(t2param, 0, wxEXPAND | wxALL, 5);
    t2slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 500, 1, 1000, wxDefaultPosition, wxSize(200, 15));
    t2slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricRosenbergWavePanel::OnT2Slide, this);
    t2slider->Show();
    t2sliderSizer->Add(t2slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(t2sliderSizer, 0, wxEXPAND | wxALL);

    chart = new nsSimpleChartControl(sourceSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(200, 24));
    chart->SetData(wave);

    sourceSizer->Add(sliderSizer, 1, wxEXPAND | wxALL);
    sourceSizer->Add(chart, 0, wxEXPAND | wxALL);
    this->SetSizer(sourceSizer);

    wxWindow* voiceMakeFrame = wxWindow::FindWindowById(nsID_VOICE_MAKE_PANEL);
    if (voiceMakeFrame) {
        voiceMakeFrame->Bind(nsEVT_SELECTED_PARAMETER_CHANGED, &nsParametricRosenbergWavePanel::OnChangeSelectedParameter, this);
        voiceMakeFrame->Bind(nsEVT_PARAMETER_CHANGED, &nsParametricRosenbergWavePanel::OnChangeParameter, this);
        voiceMakeFrame->Bind(nsEVT_ADD_PARAMETER, &nsParametricRosenbergWavePanel::OnAddParameter, this);
    }
}

void nsParametricRosenbergWavePanel::Update() {
    if (!source_wave) return;

    NesoraRosenbergParameter loaded_source = source_wave->GetParametricSource(nowSelectedParameter, parameters[nowSelectedParameter]);

    t1slider->SetLimit(0, static_cast<int>(loaded_source.tau2 * 1000.0));
    t1slider->SetValue(static_cast<int>(loaded_source.tau1 * 1000.0));
    t2slider->SetValue(static_cast<int>(loaded_source.tau2 * 1000.0));
    phonetic_slider->SetValue(static_cast<int>(loaded_source.noise * 1000.0));
    t1param->SetLabel(to_string_with_precision(loaded_source.tau1, 3));
    t2param->SetLabel(to_string_with_precision(loaded_source.tau2, 3));
    phonetic_param->SetLabel(to_string_with_precision(loaded_source.noise, 3));

    source_wave->SetParamater(loaded_source.tau1, loaded_source.tau2, loaded_source.a0, loaded_source.noise);
    source_wave->SetParamater(parameters);

    double throw_away;
    for (size_t i = 0; i < wave.size(); i++) {
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));
    }
    chart->SetData(wave);
}


void nsParametricRosenbergWavePanel::OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event) {
    nowSelectedParameter = event.GetID();
    Update();
}

void nsParametricRosenbergWavePanel::OnChangeParameter(nsParameterChangeEvent& event) {
    parameters[event.GetID()] = event.GetParam();
    Update();
}

void nsParametricRosenbergWavePanel::OnAddParameter(nsAddParameterEvent& event) {
    parameters[event.GetData()] = 0.0;
    nowSelectedParameter = event.GetData();
    Update();
}


void nsParametricRosenbergWavePanel::OnPitchSlide(wxCommandEvent& event) {
    pitch_param->SetLabel(to_string_with_precision((double)pitch_slider->GetValue(), 0) + " Hz");
    wave.resize(NesoraDefaultSamplingFrequency / pitch_slider->GetValue(), -0.5);
    OnT1Slide(event);
}

void nsParametricRosenbergWavePanel::OnPhoneticSlide(wxCommandEvent& event) {
    phonetic_param->SetLabel(to_string_with_precision((double)phonetic_slider->GetValue() / 1000.0, 3));
    OnT1Slide(event);
}

void nsParametricRosenbergWavePanel::OnT1Slide(wxCommandEvent& event) {
    t1param->SetLabel(to_string_with_precision((double)t1slider->GetValue() / 1000.0, 3));
    source_wave->SetParamater((double)t1slider->GetValue() / 1000.0, (double)t2slider->GetValue() / 1000.0, 0, (double)phonetic_slider->GetValue() / 1000.0);
    source_wave->SetParamater(parameters);

    double throw_away;
    for (size_t i = 0;i < wave.size();i++)
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));
    chart->SetData(wave);
}

void nsParametricRosenbergWavePanel::OnT2Slide(wxCommandEvent& event) {
    t2param->SetLabel(to_string_with_precision((double)t2slider->GetValue() / 1000.0, 3));
    t1slider->SetLimit(0, t2slider->GetValue());
    OnT1Slide(event);
}

std::vector<double> nsParametricRosenbergWavePanel::GetWave() const {
    return wave;
}

NesoraSourceBase* nsParametricRosenbergWavePanel::GetSource() {
    return source_wave;
}

double nsParametricRosenbergWavePanel::GetPitch() const {
    return (double)pitch_slider->GetValue();
}


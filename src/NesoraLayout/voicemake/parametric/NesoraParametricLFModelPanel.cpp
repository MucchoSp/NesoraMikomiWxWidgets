// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraParametricLFModelPanel.h"

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}


void nsParametricLFModelPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    source_wave = new NesoraParametricLFModel();

    source_wave->SetParamater(0.25, 0.5, 0.1, 0.1, 0.1);
    source_wave->SetParamater(parameters);
    wave.resize(183);// 48000/261.6
    double throw_away;
    for (size_t i = 0;i < wave.size();i++)
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Liljencrants-Fant Model"));
    wxSizer* pitch_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* tp_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* te_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* ta_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* ee_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* phonetic_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sliderSizer = new wxBoxSizer(wxVERTICAL);

    pitch_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("261Hz"));
    pitch_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    pitch_param->SetToolTip(_("Fundamental frequency of the voice."));
    pitch_sliderSizer->Add(pitch_param, 0, wxEXPAND | wxALL, 5);
    pitch_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 261, 20, 1000, wxDefaultPosition, wxSize(200, 15));
    pitch_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelPanel::OnPitchSlide, this);
    pitch_slider->SetToolTip(_("Fundamental frequency of the voice."));
    pitch_slider->Show();
    pitch_sliderSizer->Add(pitch_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(pitch_sliderSizer, 0, wxEXPAND | wxALL);

    phonetic_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.100");
    phonetic_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    phonetic_param->SetToolTip(_("The ratio of voiced to unvoiced sounds."));
    phonetic_sliderSizer->Add(phonetic_param, 0, wxEXPAND | wxALL, 5);
    phonetic_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 100, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    phonetic_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelPanel::OnPhoneticSlide, this);
    phonetic_slider->SetToolTip(_("The ratio of voiced to unvoiced sounds."));
    phonetic_slider->Show();
    phonetic_sliderSizer->Add(phonetic_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(phonetic_sliderSizer, 0, wxEXPAND | wxALL);

    tpparam = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("Tp: 0.400"));
    tpparam->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    tpparam->SetToolTip(_("Tp parameter for the voice. The time when the glottal flow is at its maximum."));
    tp_sliderSizer->Add(tpparam, 0, wxEXPAND | wxALL, 5);
    tpslider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 400, 1, 1000, wxDefaultPosition, wxSize(200, 15));
    tpslider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelPanel::OnTpSlide, this);
    tpslider->SetToolTip(_("Tp parameter for the voice. The time when the glottal flow is at its maximum."));
    tpslider->Show();
    tp_sliderSizer->Add(tpslider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(tp_sliderSizer, 0, wxEXPAND | wxALL);

    teparam = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("Te: 0.550"));
    teparam->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    teparam->SetToolTip(_("Te parameter for the voice. The moment just before the vocal cords close, when the change in flow velocity is greatest."));
    te_sliderSizer->Add(teparam, 0, wxEXPAND | wxALL, 5);
    teslider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 550, 1, 1000, wxDefaultPosition, wxSize(200, 15));
    teslider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelPanel::OnTeSlide, this);
    teslider->SetToolTip(_("Te parameter for the voice. The moment just before the vocal cords close, when the change in flow velocity is greatest."));
    teslider->Show();
    te_sliderSizer->Add(teslider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(te_sliderSizer, 0, wxEXPAND | wxALL);

    taparam = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("Ta: 0.050"));
    taparam->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    taparam->SetToolTip(_("Ta parameter for the voice. The duration of the afterglow until the vocal cords close completely."));
    ta_sliderSizer->Add(taparam, 0, wxEXPAND | wxALL, 5);
    taslider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 50, 1, 1000, wxDefaultPosition, wxSize(200, 15));
    taslider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelPanel::OnTaSlide, this);
    taslider->SetToolTip(_("Ta parameter for the voice. The duration of the afterglow until the vocal cords close completely."));
    taslider->Show();
    ta_sliderSizer->Add(taslider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(ta_sliderSizer, 0, wxEXPAND | wxALL);

    eeparam = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("Ee: 0.001"));
    eeparam->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    eeparam->SetToolTip(_("Ee parameter for the voice. Maximum excitation intensity."));
    ee_sliderSizer->Add(eeparam, 0, wxEXPAND | wxALL, 5);
    eeslider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 1, 1, 1000, wxDefaultPosition, wxSize(200, 15));
    eeslider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelPanel::OnEeSlide, this);
    eeslider->SetToolTip(_("Ee parameter for the voice. Maximum excitation intensity."));
    eeslider->Show();
    ee_sliderSizer->Add(eeslider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(ee_sliderSizer, 0, wxEXPAND | wxALL);

    chart = new nsSimpleChartControl(sourceSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(200, 24));
    chart->SetData(wave);

    sourceSizer->Add(sliderSizer, 1, wxEXPAND | wxALL);
    sourceSizer->Add(chart, 0, wxEXPAND | wxALL);
    SetSizer(sourceSizer);
}

void nsParametricLFModelPanel::Update() {
    // TODO: パラメータの更新
}


void nsParametricLFModelPanel::OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event) {
    nowSelectedParameter = event.GetID();
    Update();
}

void nsParametricLFModelPanel::OnChangeParameter(nsParameterChangeEvent& event) {
    parameters[event.GetID()] = event.GetParam();
    Update();
}

void nsParametricLFModelPanel::OnAddParameter(nsAddParameterEvent& event) {
    parameters[event.GetData()] = 0.0;
    nowSelectedParameter = event.GetData();
    Update();
}


void nsParametricLFModelPanel::OnPitchSlide(wxCommandEvent& event) {
    pitch_param->SetLabel(to_string_with_precision((double)pitch_slider->GetValue(), 0) + " Hz");
    wave.resize(NesoraDefaultSamplingFrequency / pitch_slider->GetValue(), -0.5);

    UpdateChart();
}

void nsParametricLFModelPanel::OnPhoneticSlide(wxCommandEvent& event) {
    phonetic_param->SetLabel(to_string_with_precision((double)phonetic_slider->GetValue() / 1000.0, 3) + " Hz");

    UpdateChart();
}

void nsParametricLFModelPanel::OnTpSlide(wxCommandEvent& event) {
    tpparam->SetLabel("Tp: " + to_string_with_precision((double)tpslider->GetValue() / 1000.0, 3));
    teslider->SetLimit(tpslider->GetValue() + 1, 1000);
    teparam->SetLabel("Te: " + to_string_with_precision((double)teslider->GetValue() / 1000.0, 3));

    UpdateChart();
}

void nsParametricLFModelPanel::OnTeSlide(wxCommandEvent& event) {
    teparam->SetLabel("Te: " + to_string_with_precision((double)teslider->GetValue() / 1000.0, 3));
    tpslider->SetLimit(0, teslider->GetValue());
    tpparam->SetLabel("Tp: " + to_string_with_precision((double)tpslider->GetValue() / 1000.0, 3));
    taslider->SetLimit(0, 1000 - teslider->GetValue());
    taparam->SetLabel("Ta: " + to_string_with_precision((double)taslider->GetValue() / 1000.0, 3));
    OnTaSlide(event);

    UpdateChart();
}

void nsParametricLFModelPanel::OnTaSlide(wxCommandEvent& event) {
    taparam->SetLabel("Ta: " + to_string_with_precision((double)taslider->GetValue() / 1000.0, 3));

    UpdateChart();
}

void nsParametricLFModelPanel::OnEeSlide(wxCommandEvent& event) {
    eeparam->SetLabel("Ee: " + to_string_with_precision((double)eeslider->GetValue() / 1000.0, 3));

    UpdateChart();
}

void nsParametricLFModelPanel::UpdateChart() {
    source_wave->SetParamater(0.001 * (double)tpslider->GetValue(), 0.001 * (double)teslider->GetValue(), 0.001 * (double)taslider->GetValue(), 0.001 * (double)eeslider->GetValue(), 0.001 * (double)phonetic_slider->GetValue());
    
    double throw_away;
    for (size_t i = 0;i < wave.size();i++) {
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away) * 2.0 * M_PI);
    }
    chart->SetData(wave);
}

std::vector<double> nsParametricLFModelPanel::GetWave() const {
    return wave;
}

NesoraParametricLFModel* nsParametricLFModelPanel::GetSource() {
    return source_wave;
}

double nsParametricLFModelPanel::GetPitch() const {
    return (double)pitch_slider->GetValue();
}










// MARK: nsParametricLFModelRdParameterPanel

void nsParametricLFModelRdParameterPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    source_wave = new NesoraParametricLFModelRdParameter();

    source_wave->SetParamater(1.0, 0.1, 0.1);
    source_wave->SetParamater(parameters);
    wave.resize(183);// 48000/261.6
    double throw_away;
    for (size_t i = 0;i < wave.size();i++)
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Liljencrants-Fant Model Rd Parameter"));
    wxSizer* pitch_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* rd_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* ee_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* phonetic_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sliderSizer = new wxBoxSizer(wxVERTICAL);

    pitch_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("Pitch"));
    pitch_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    pitch_sliderSizer->Add(pitch_param, 0, wxEXPAND | wxALL, 5);
    pitch_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 261, 20, 1000, wxDefaultPosition, wxSize(200, 15));
    pitch_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelRdParameterPanel::OnPitchSlide, this);
    pitch_slider->Show();
    pitch_sliderSizer->Add(pitch_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(pitch_sliderSizer, 0, wxEXPAND | wxALL);

    phonetic_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.100");
    phonetic_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    phonetic_sliderSizer->Add(phonetic_param, 0, wxEXPAND | wxALL, 5);
    phonetic_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 100, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    phonetic_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelRdParameterPanel::OnPhoneticSlide, this);
    phonetic_slider->Show();
    phonetic_sliderSizer->Add(phonetic_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(phonetic_sliderSizer, 0, wxEXPAND | wxALL);

    rdparam = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("Rd: 1.000"));
    rdparam->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    rd_sliderSizer->Add(rdparam, 0, wxEXPAND | wxALL, 5);
    rdslider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 1000, 300, 2700, wxDefaultPosition, wxSize(200, 15));
    rdslider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelRdParameterPanel::OnRdSlide, this);
    rdslider->Show();
    rd_sliderSizer->Add(rdslider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(rd_sliderSizer, 0, wxEXPAND | wxALL);

    eeparam = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, _("Ee: 1.000"));
    eeparam->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    ee_sliderSizer->Add(eeparam, 0, wxEXPAND | wxALL, 5);
    eeslider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 1000, 1, 1000, wxDefaultPosition, wxSize(200, 15));
    eeslider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricLFModelRdParameterPanel::OnEeSlide, this);
    eeslider->Show();
    ee_sliderSizer->Add(eeslider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(ee_sliderSizer, 0, wxEXPAND | wxALL);

    chart = new nsSimpleChartControl(sourceSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(200, 24));
    chart->SetData(wave);

    sourceSizer->Add(sliderSizer, 1, wxEXPAND | wxALL);
    sourceSizer->Add(chart, 0, wxEXPAND | wxALL);
    SetSizer(sourceSizer);
}

void nsParametricLFModelRdParameterPanel::Update() {
    // TODO: パラメータの更新
}


void nsParametricLFModelRdParameterPanel::OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event) {
    nowSelectedParameter = event.GetID();
    Update();
}

void nsParametricLFModelRdParameterPanel::OnChangeParameter(nsParameterChangeEvent& event) {
    parameters[event.GetID()] = event.GetParam();
    Update();
}

void nsParametricLFModelRdParameterPanel::OnAddParameter(nsAddParameterEvent& event) {
    parameters[event.GetData()] = 0.0;
    nowSelectedParameter = event.GetData();
    Update();
}


void nsParametricLFModelRdParameterPanel::OnPitchSlide(wxCommandEvent& event) {
    pitch_param->SetLabel(to_string_with_precision((double)pitch_slider->GetValue(), 0) + " Hz");
    wave.resize(NesoraDefaultSamplingFrequency / pitch_slider->GetValue(), -0.5);

    UpdateChart();
}

void nsParametricLFModelRdParameterPanel::OnPhoneticSlide(wxCommandEvent& event) {
    phonetic_param->SetLabel(to_string_with_precision((double)phonetic_slider->GetValue() / 1000.0, 3) + " Hz");

    UpdateChart();
}

void nsParametricLFModelRdParameterPanel::OnRdSlide(wxCommandEvent& event) {
    rdparam->SetLabel("Rd: " + to_string_with_precision((double)rdslider->GetValue() / 1000.0, 3));

    UpdateChart();
}

void nsParametricLFModelRdParameterPanel::OnEeSlide(wxCommandEvent& event) {
    eeparam->SetLabel("Ee: " + to_string_with_precision((double)eeslider->GetValue() / 1000.0, 3));

    UpdateChart();
}

void nsParametricLFModelRdParameterPanel::UpdateChart() {
    source_wave->SetParamater(0.001 * (double)rdslider->GetValue(), 0.001 * (double)eeslider->GetValue(), 0.001 * (double)phonetic_slider->GetValue());
    
    double throw_away;
    for (size_t i = 0;i < wave.size();i++) {
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away) * 2.0 * M_PI);
    }
    chart->SetData(wave);
}

std::vector<double> nsParametricLFModelRdParameterPanel::GetWave() const {
    return wave;
}

NesoraParametricLFModelRdParameter* nsParametricLFModelRdParameterPanel::GetSource() {
    return source_wave;
}

double nsParametricLFModelRdParameterPanel::GetPitch() const {
    return (double)pitch_slider->GetValue();
}


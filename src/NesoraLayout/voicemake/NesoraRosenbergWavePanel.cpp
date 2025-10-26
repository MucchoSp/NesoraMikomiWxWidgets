#include "NesoraRosenbergWavePanel.h"

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}



void nsRosenbergWavePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    source_wave = new NesoraRosenbergWave();

    source_wave->SetParamater(0.25, 0.5, -0.5);
    wave.resize(183);// 48000/261.6
    double throw_away;
    for (size_t i = 0;i < wave.size();i++)
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Rosenberg Wave"));
    wxSizer* pitch_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t1sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t2sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sliderSizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* playButtonSizer = new wxBoxSizer(wxVERTICAL);

    pitch_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "261 Hz");
    pitch_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    pitch_sliderSizer->Add(pitch_param, 0, wxEXPAND | wxALL, 5);
    pitch_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 261, 20, 1000, wxDefaultPosition, wxSize(200, 15));
    pitch_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsRosenbergWavePanel::OnPitchSlide, this);
    pitch_slider->Show();
    pitch_sliderSizer->Add(pitch_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(pitch_sliderSizer, 0, wxEXPAND | wxALL);

    t1param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.250");
    t1param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t1sliderSizer->Add(t1param, 0, wxEXPAND | wxALL, 5);
    t1slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 250, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    t1slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsRosenbergWavePanel::OnT1Slide, this);
    t1slider->SetLimit(0, 500);
    t1slider->Show();
    t1sliderSizer->Add(t1slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(t1sliderSizer, 0, wxEXPAND | wxALL);

    t2param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.500");
    t2param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t2sliderSizer->Add(t2param, 0, wxEXPAND | wxALL, 5);
    t2slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 500, 1, 1000, wxDefaultPosition, wxSize(200, 15));
    t2slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsRosenbergWavePanel::OnT2Slide, this);
    t2slider->Show();
    t2sliderSizer->Add(t2slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(t2sliderSizer, 0, wxEXPAND | wxALL);

    chart = new nsSimpleChartControl(sourceSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(200, 24));
    chart->SetData(wave);

    sourceSizer->Add(playButtonSizer, 0, wxEXPAND | wxALL);
    sourceSizer->Add(sliderSizer, 1, wxEXPAND | wxALL);
    sourceSizer->Add(chart, 0, wxEXPAND | wxALL);
    this->SetSizer(sourceSizer);
}

void nsRosenbergWavePanel::Update() {
    // Directly sync sliders/labels and regenerate waveform from source parameters
    if (!source_wave) return;

    double loaded_t1 = source_wave->GetT1();
    double loaded_t2 = source_wave->GetT2();
    double loaded_a0 = source_wave->GetA0();
    
    // Ensure limits are consistent
    t1slider->SetLimit(0, static_cast<int>(loaded_t2 * 1000.0));

    // Update slider values and labels
    t1slider->SetValue(static_cast<int>(loaded_t1 * 1000.0));
    t2slider->SetValue(static_cast<int>(loaded_t2 * 1000.0));
    t1param->SetLabel(to_string_with_precision(loaded_t1, 3));
    t2param->SetLabel(to_string_with_precision(loaded_t2, 3));

    // Apply parameters to source explicitly and regenerate waveform
    source_wave->SetParamater(loaded_t1, loaded_t2, loaded_a0);

    double throw_away;
    for (size_t i = 0; i < wave.size(); i++) {
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));
    }
    chart->SetData(wave);

    std::cout << "Rosenberg parameter: Pitch " << GetPitch() << " Hz, T1 " << loaded_t1 << ", T2 " << loaded_t2 << std::endl;
}


void nsRosenbergWavePanel::OnPitchSlide(wxCommandEvent& event) {
    pitch_param->SetLabel(to_string_with_precision((double)pitch_slider->GetValue(), 0) + " Hz");
    wave.resize(NesoraDefaultSamplingFrequency / pitch_slider->GetValue(), -0.5);
    OnT1Slide(event);
}

void nsRosenbergWavePanel::OnT1Slide(wxCommandEvent& event) {
    t1param->SetLabel(to_string_with_precision((double)t1slider->GetValue() / 1000.0, 3));
    source_wave->SetParamater((double)t1slider->GetValue() / 1000.0, (double)t2slider->GetValue() / 1000.0, 0);

    double throw_away;
    for (size_t i = 0;i < wave.size();i++)
        wave[i] = source_wave->Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));
    chart->SetData(wave);
}

void nsRosenbergWavePanel::OnT2Slide(wxCommandEvent& event) {
    t2param->SetLabel(to_string_with_precision((double)t2slider->GetValue() / 1000.0, 3));
    t1slider->SetLimit(0, t2slider->GetValue());
    OnT1Slide(event);
}

std::vector<double> nsRosenbergWavePanel::GetWave() const {
    return wave;
}

NesoraRosenbergWave* nsRosenbergWavePanel::GetSource() {
    return source_wave;
}

double nsRosenbergWavePanel::GetPitch() const {
    return (double)pitch_slider->GetValue();
}


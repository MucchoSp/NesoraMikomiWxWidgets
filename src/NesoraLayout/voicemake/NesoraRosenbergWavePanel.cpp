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

    source_wave.SetParamater(0.25, 0.5, 0);
    wave.resize(183);// 48000/261.6
    double throw_away;
    for (int i = 0;i < wave.size();i++)
        wave[i] = source_wave.Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Rosenberg Wave"));
    wxSizer* pitch_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t1sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t2sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sliderSizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* playButtonSizer = new wxBoxSizer(wxVERTICAL);

    pitch_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "220");
    pitch_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    pitch_sliderSizer->Add(pitch_param, 0, wxEXPAND | wxALL, 5);
    pitch_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 220, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    pitch_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsRosenbergWavePanel::OnPitchSlide, this);
    pitch_slider->Show();
    pitch_sliderSizer->Add(pitch_slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(pitch_sliderSizer, 0, wxEXPAND | wxALL);

    t1param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.25");
    t1param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t1sliderSizer->Add(t1param, 0, wxEXPAND | wxALL, 5);
    t1slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 25, 0, 100, wxDefaultPosition, wxSize(200, 15));
    t1slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsRosenbergWavePanel::OnT1Slide, this);
    t1slider->SetLimit(0, 50);
    t1slider->Show();
    t1sliderSizer->Add(t1slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(t1sliderSizer, 0, wxEXPAND | wxALL);

    t2param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "0.50");
    t2param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t2sliderSizer->Add(t2param, 0, wxEXPAND | wxALL, 5);
    t2slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 50, 1, 100, wxDefaultPosition, wxSize(200, 15));
    t2slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsRosenbergWavePanel::OnT2Slide, this);
    t2slider->Show();
    t2sliderSizer->Add(t2slider, 1, wxEXPAND | wxALL, 5);
    sliderSizer->Add(t2sliderSizer, 0, wxEXPAND | wxALL);

    playButton = new nsButton(sourceSizer->GetStaticBox(), wxID_ANY, _("Play"));
    stopButton = new nsButton(sourceSizer->GetStaticBox(), wxID_ANY, _("Stop"));
    playButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsRosenbergWavePanel::OnPlayButton, this);
    stopButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsRosenbergWavePanel::OnStopButton, this);
    playButtonSizer->Add(playButton, 1, wxEXPAND | wxALL);
    playButtonSizer->Add(stopButton, 1, wxEXPAND | wxALL);

    chart = new nsSimpleChartControl(sourceSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(200, 24));
    chart->SetData(wave);

    sourceSizer->Add(playButtonSizer, 0, wxEXPAND | wxALL);
    sourceSizer->Add(sliderSizer, 1, wxEXPAND | wxALL);
    sourceSizer->Add(chart, 0, wxEXPAND | wxALL);
    this->SetSizer(sourceSizer);
}


void nsRosenbergWavePanel::OnPitchSlide(wxCommandEvent& event) {
    pitch_param->SetLabel(to_string_with_precision((double)pitch_slider->GetValue(), 2));
    wave.resize(48000 / pitch_slider->GetValue());
    OnT1Slide(event);
}

void nsRosenbergWavePanel::OnT1Slide(wxCommandEvent& event) {
    t1param->SetLabel(to_string_with_precision((double)t1slider->GetValue() / 100.0, 2));
    source_wave.SetParamater((double)t1slider->GetValue() / 100.0, (double)t2slider->GetValue() / 100.0, 0);

    double throw_away;
    for (int i = 0;i < wave.size();i++)
        wave[i] = source_wave.Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));
    chart->SetData(wave);
}

void nsRosenbergWavePanel::OnT2Slide(wxCommandEvent& event) {
    t2param->SetLabel(to_string_with_precision((double)t2slider->GetValue() / 100.0, 2));
    t1slider->SetLimit(0, t2slider->GetValue());
    OnT1Slide(event);
}

void nsRosenbergWavePanel::OnPlayButton(wxCommandEvent& event) {
    InitAudioDevice();
}

void nsRosenbergWavePanel::OnStopButton(wxCommandEvent& event) {
    UninitAudioDevice();
}

void nsRosenbergWavePanel::InitAudioDevice() {
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate = 48000;
    deviceConfig.dataCallback = nsRosenbergWavePanel::data_callback;
    deviceConfig.pUserData = this;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        wxLogMessage("Failed to open playback device.");
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        wxLogMessage("Failed to start playback device.");
        ma_device_uninit(&device);
        return;
    }
}

void nsRosenbergWavePanel::UninitAudioDevice() {
    ma_device_uninit(&device);
}

void nsRosenbergWavePanel::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* out = (float*)pOutput;
    (void)pInput;

    nsRosenbergWavePanel* frame = (nsRosenbergWavePanel*)pDevice->pUserData;
    static size_t pos = 0;
    for (ma_uint32 i = 0; i < frameCount; i++) {
        if (pos >= frame->wave.size()) {
            out[i] = 0;
            pos = 0;
        }
        else {
            out[i] = (float)frame->wave[pos] * 0.5;
            pos++;
        }
    }
}


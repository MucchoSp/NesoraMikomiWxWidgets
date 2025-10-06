#include "NesoraVoiceMakePanel.h"

void nsVoiceMakePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    RosenbergWavePanelSetup();

    Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsVoiceMakePanel::OnT1Slide, this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_T1SLIDER);
    Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsVoiceMakePanel::OnT2Slide, this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_T2SLIDER);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsVoiceMakePanel::OnPlayButton, this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_PLAY_BUTTON);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &nsVoiceMakePanel::OnStopButton, this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_STOP_BUTTON);
}

void nsVoiceMakePanel::RosenbergWavePanelSetup() {

    source_wave.SetParamater(0.25, 0.5, 0);
    wave.resize(183);// 48000/261.6
    double throw_away;
    for (int i = 0;i < wave.size();i++)
        wave[i] = source_wave.Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* t1sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* t2sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* playButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    chart = new nsChartControl(this, wxID_ANY);
    chart->SetTitle("RosenbergWave");
    chart->ShowGrid(false);
    chart->ShowLabel(false);
    chart->SetData(wave);


    sizer->Add(chart, 1, wxEXPAND | wxALL);

    t1param = new wxStaticText(this, wxID_ANY, "0.25");
    t1param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t1sliderSizer->Add(t1param, 0, wxEXPAND | wxALL, 5);
    t1slider = new nsSlider(this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_T1SLIDER, 25, 0, 100, wxDefaultPosition, wxSize(300, 15));
    t1slider->Show();
    t1sliderSizer->Add(t1slider, 0, wxEXPAND | wxALL, 5);
    sizer->Add(t1sliderSizer, 0, wxEXPAND | wxALL);

    t2param = new wxStaticText(this, wxID_ANY, "0.50");
    t2param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    t2sliderSizer->Add(t2param, 0, wxEXPAND | wxALL, 5);
    t2slider = new nsSlider(this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_T2SLIDER, 50, 1, 100, wxDefaultPosition, wxSize(300, 15));
    t2slider->Show();
    t2sliderSizer->Add(t2slider, 0, wxEXPAND | wxALL, 5);
    sizer->Add(t2sliderSizer, 0, wxEXPAND | wxALL);

    playButton = new nsButton(this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_PLAY_BUTTON, "Play");
    stopButton = new nsButton(this, ID_VOICEMAKEPANEL_ROSENBERGWAVE_STOP_BUTTON, "Stop");
    playButtonSizer->Add(playButton, 0, wxEXPAND | wxALL);
    playButtonSizer->Add(stopButton, 0, wxEXPAND | wxALL);
    sizer->Add(playButtonSizer, 0, wxEXPAND | wxALL);

    this->SetSizer(sizer);
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

void nsVoiceMakePanel::OnT1Slide(wxCommandEvent& event) {
    t1param->SetLabel(to_string_with_precision((double)t1slider->GetValue() / 100.0, 2));
    source_wave.SetParamater((double)t1slider->GetValue() / 100.0, (double)t2slider->GetValue() / 100.0, 0);

    double throw_away;
    for (int i = 0;i < wave.size();i++)
        wave[i] = source_wave.Utterance(std::modf((double)i / (double)wave.size() * 2.0, &throw_away));
    chart->SetData(wave);
}

void nsVoiceMakePanel::OnT2Slide(wxCommandEvent& event) {
    t2param->SetLabel(to_string_with_precision((double)t2slider->GetValue() / 100.0, 2));
    t1slider->SetLimit(0, t2slider->GetValue());
    OnT1Slide(event);
}

void nsVoiceMakePanel::OnPlayButton(wxCommandEvent& event) {
    InitAudioDevice();
}

void nsVoiceMakePanel::OnStopButton(wxCommandEvent& event) {
    UninitAudioDevice();
}


void nsVoiceMakePanel::InitAudioDevice() {
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate = 48000;
    deviceConfig.dataCallback = nsVoiceMakePanel::data_callback;
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

void nsVoiceMakePanel::UninitAudioDevice() {
    ma_device_uninit(&device);
}

void nsVoiceMakePanel::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* out = (float*)pOutput;
    (void)pInput;

    nsVoiceMakePanel* frame = (nsVoiceMakePanel*)pDevice->pUserData;
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
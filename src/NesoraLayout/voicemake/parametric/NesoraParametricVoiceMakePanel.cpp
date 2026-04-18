// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraParametricVoiceMakePanel.h"

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

void nsParametricVoiceMakePlayInterfacePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Play Interface"));
    playButton = new nsButton(sizer->GetStaticBox(), wxID_ANY, _("Play"));
    stopButton = new nsButton(sizer->GetStaticBox(), wxID_ANY, _("Stop"));
    statusText = new wxStaticText(sizer->GetStaticBox(), wxID_ANY, "0.00 dB");
    statusText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    statusText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    volume = new nsSlider(sizer->GetStaticBox(), wxID_ANY, 100, 0, 100, wxDefaultPosition, wxSize(100, 15));
    volume->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParametricVoiceMakePlayInterfacePanel::OnVolumeSlide, this);

    sizer->Add(playButton, 0, wxEXPAND | wxALL);
    sizer->Add(stopButton, 0, wxEXPAND | wxALL);
    sizer->Add(statusText, 0, wxEXPAND | wxALL);
    sizer->Add(volume, 0, wxEXPAND | wxALL);

    this->SetSizer(sizer);
}

void nsParametricVoiceMakePlayInterfacePanel::OnVolumeSlide(wxCommandEvent& event) {
    statusText->SetLabel(to_string_with_precision(-10.0 + (double)volume->GetValue() / 10.0, 2) + " dB");
}




// MARK: nsParametricVoiceMakePanel

void nsParametricVoiceMakePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_NORMAL, WXK_ESCAPE, nsID_ESCAPE);  //Escキーでエスケープ
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* filterHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    playInterfacePanel = new nsParametricVoiceMakePlayInterfacePanel(this, wxID_ANY);
    sourceSoundPanel = new nsParametricLFModelPanel(this, wxID_ANY);
    filterPanel = new nsParametricSOSIIRFilterPanel(this, wxID_ANY);
    parametricPanel = new nsParametricPanel(this, wxID_ANY);
    voice = new NesoraMikomiVoice(sourceSoundPanel->GetSource(), filterPanel->GetFilter());

    horizontalSizer->Add(playInterfacePanel, 0, wxEXPAND | wxALL);
    horizontalSizer->Add(sourceSoundPanel, 1, wxEXPAND | wxALL);
    sizer->Add(horizontalSizer, 0, wxEXPAND | wxALL);
    filterHorizontalSizer->Add(parametricPanel, 0, wxEXPAND | wxALL);
    filterHorizontalSizer->Add(filterPanel, 1, wxEXPAND | wxALL);
    sizer->Add(filterHorizontalSizer, 1, wxEXPAND | wxALL);

    this->SetSizer(sizer);

    playInterfacePanel->playButton->Bind(wxEVT_BUTTON, &nsParametricVoiceMakePanel::OnPlayButtonClicked, this);
    playInterfacePanel->stopButton->Bind(wxEVT_BUTTON, &nsParametricVoiceMakePanel::OnStopButtonClicked, this);
}


void nsParametricVoiceMakePanel::OnPlayButtonClicked(wxCommandEvent& event) {
    InitAudioDevice();
}

void nsParametricVoiceMakePanel::OnStopButtonClicked(wxCommandEvent& event) {
    UninitAudioDevice();
}


void nsParametricVoiceMakePanel::InitAudioDevice() {
    if (isPlaying) {
        return;
    }
    isPlaying = true;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate = NesoraDefaultSamplingFrequency;
    deviceConfig.dataCallback = nsParametricVoiceMakePanel::data_callback;
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

void nsParametricVoiceMakePanel::UninitAudioDevice() {
    isPlaying = false;
    ma_device_uninit(&device);
}

void nsParametricVoiceMakePanel::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* out = (float*)pOutput;
    (void)pInput;

    nsParametricVoiceMakePanel* frame = (nsParametricVoiceMakePanel*)pDevice->pUserData;
    for (ma_uint32 i = 0; i < frameCount; i++) {
        out[i] = (float)frame->voice->Synthesize(frame->sourceSoundPanel->GetPitch(), NesoraDefaultSamplingFrequency) / (std::pow(10.0, 10.0 - (float)frame->playInterfacePanel->volume->GetValue() / 10.0));
    }
}

void nsParametricVoiceMakePanel::OnSave(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, _("Save Voice"), "", "",
        "Nesora Voice Files (*.nsvo)|*.nsvo", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxFileOutputStream output_stream(saveFileDialog.GetPath());
    if (!output_stream.IsOk()) {
        wxLogError("Cannot save current voice to file '%s'.", saveFileDialog.GetPath());
        return;
    }
    
    std::vector<unsigned char> voiceData = voice->GetVoiceData();
    output_stream.Write(voiceData.data(), voiceData.size());
    output_stream.Close();

    wxString msg;
    msg.Printf("Voice saved to '%s'.", saveFileDialog.GetPath());
    static_cast<wxFrame* >(wxGetTopLevelParent(this))->SetStatusText(msg);
    // wxLogMessage("Voice saved to '%s'.", saveFileDialog.GetPath());
}

void nsParametricVoiceMakePanel::OnOpen(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, _("Open Voice"), "", "",
        "Nesora Voice Files (*.nsvo)|*.nsvo", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk()) {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
        return;
    }

    std::vector<unsigned char> fileData;
    size_t fileSize = input_stream.GetSize();
    fileData.resize(fileSize);
    input_stream.Read(fileData.data(), fileSize);

    voice->LoadVoiceData(fileData);

    sourceSoundPanel->Update();
    if (filterPanel) {
        filterPanel->Update();
    }

    wxString msg;
    msg.Printf("Voice loaded from '%s'.", openFileDialog.GetPath());
    static_cast<wxFrame* >(wxGetTopLevelParent(this))->SetStatusText(msg);
    // wxLogMessage("Voice loaded from '%s'.", openFileDialog.GetPath());
}

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraDictionalyPlayPanel.h"
#include "NesoraDictionalyIndexPanel.h"

// MARK: NesoraDictionalyPlayPanel

void NesoraDictionalyPlayPanel::Init() {
    this->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    // SetMinSize(wxSize(-1, 50));

    wxBoxSizer* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER);
    wxBitmap stopBitmap(NESORA_STOP_ICON_24);
    toolbar->AddTool(nsID_STOP, _("Stop"), stopBitmap, wxNullBitmap, wxITEM_NORMAL, _("Stop the current word data"));
    wxBitmap playBitmap(NESORA_PLAY_ICON_24);
    toolbar->AddTool(nsID_PLAY, _("Play"), playBitmap, wxNullBitmap, wxITEM_NORMAL, _("Play the current word data"));
    toolbar->Realize();

    volumeText = new wxStaticText(this, wxID_ANY, "-2.50 dB");
    volumeText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    volumeText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    volume = new nsSlider(this, wxID_ANY, 75, 0, 100, wxDefaultPosition, wxSize(100, 15));
    volume->SetToolTip(_("Volume"));
    volume->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraDictionalyPlayPanel::OnVolumeChange, this);
    pitchText = new wxStaticText(this, wxID_ANY, "220 Hz");
    pitchText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    pitchText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    pitch = new nsSlider(this, wxID_ANY, 30, 0, 100, wxDefaultPosition, wxSize(100, 15));
    pitch->SetToolTip(_("Pitch"));
    pitch->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraDictionalyPlayPanel::OnPitchChange, this);
    lengthText = new wxStaticText(this, wxID_ANY, "1.00 s");
    lengthText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    lengthText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    length = new nsSlider(this, wxID_ANY, 20, 0, 100, wxDefaultPosition, wxSize(100, 15));
    length->SetToolTip(_("Length"));
    length->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraDictionalyPlayPanel::OnLengthChange, this);
    toolbarSizer->Add(toolbar, 1, wxEXPAND | wxALL);
    toolbarSizer->Add(volumeText, 0, wxEXPAND | wxALL);
    toolbarSizer->Add(volume, 0, wxALL);
    toolbarSizer->Add(pitchText, 0, wxEXPAND | wxALL);
    toolbarSizer->Add(pitch, 0, wxALL);
    toolbarSizer->Add(lengthText, 0, wxEXPAND | wxALL);
    toolbarSizer->Add(length, 0, wxALL);

    this->SetSizer(toolbarSizer);

    Bind(wxEVT_TOOL, &NesoraDictionalyPlayPanel::OnStop, this, nsID_STOP);
    Bind(wxEVT_TOOL, &NesoraDictionalyPlayPanel::OnPlay, this, nsID_PLAY);
}

void NesoraDictionalyPlayPanel::OnPlay(wxCommandEvent& event) {
    if (voice == nullptr) {
        wxLogMessage("Voice is not set.");
        return;
    }

    if (!isPlaying) {
        if (indexWord == nullptr || indexWord->word.symbol.empty()) {
            wxLogMessage("Word is not set.");
            return;
        }

        voice->RefreshScript();
        voice->CacheScriptWave();
        if (InitAudioDevice() == 0) {
            isPlaying = true;
        }
    }
}

void NesoraDictionalyPlayPanel::OnStop(wxCommandEvent& event) {
    if (voice == nullptr) {
        wxLogMessage("Voice is not set.");
        return;
    }
    if (isPlaying) {
        UninitAudioDevice();
        voice->SetSynthesizeScriptIndex(0);
        isPlaying = false;
    }
}

void NesoraDictionalyPlayPanel::OnVolumeChange(wxCommandEvent& event) {
    volumeText->SetLabel(wxString::Format(_("%.2f dB"), -10.0 + (double)volume->GetValue() / 10.0));
}

void NesoraDictionalyPlayPanel::OnPitchChange(wxCommandEvent& event) {
    pitchText->SetLabel(wxString::Format(_("%.0f Hz"), 100.0 + static_cast<double>(pitch->GetValue()) * 4.0));
}

void NesoraDictionalyPlayPanel::OnLengthChange(wxCommandEvent& event) {
    lengthText->SetLabel(wxString::Format(_("%.2f s"), static_cast<double>(length->GetValue()) / 20.0));
}

int NesoraDictionalyPlayPanel::InitAudioDevice() {
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate        = NesoraDefaultSamplingFrequency;
    deviceConfig.dataCallback      = NesoraDictionalyPlayPanel::data_callback;
    deviceConfig.pUserData = this;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        wxLogMessage("Failed to open playback device.");
        return -1;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        wxLogMessage("Failed to start playback device.");
        ma_device_uninit(&device);
        return -1;
    }
    return 0;
}

void NesoraDictionalyPlayPanel::UninitAudioDevice() {
    ma_device_uninit(&device);
}

void NesoraDictionalyPlayPanel::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* out = (float*)pOutput;
    (void)pInput;

    NesoraDictionalyPlayPanel* playPanel = (NesoraDictionalyPlayPanel*)pDevice->pUserData;
    double volume = 1.0 / std::pow(10.0, 10.0 - (float)playPanel->volume->GetValue() / 10.0);
    for (ma_uint32 i = 0; i < frameCount; i++) {
        out[i] = (float)playPanel->voice->GetScriptWave() * volume;
    }
}



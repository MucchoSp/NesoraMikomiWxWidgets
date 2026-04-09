// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraSingPanel.h"

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

void nsSingPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    // wxStaticText* label = new wxStaticText(this, wxID_ANY, _("This is the Sing Panel"), wxPoint(10, 10));

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);
    ToolBar();
    volumeText = new wxStaticText(this, wxID_ANY, "0.00 dB");
    volumeText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    volumeText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    volume = new nsSlider(this, wxID_ANY, 100, 0, 100, wxDefaultPosition, wxSize(100, 15));
    volume->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsSingPanel::OnVolumeSlide, this);
    toolbarSizer->Add(toolbar, 1, wxEXPAND | wxALL);
    toolbarSizer->Add(volumeText, 0, wxEXPAND | wxALL);
    toolbarSizer->Add(volume, 0, wxALL);


    midiPanel = new NesoraMIDIPanel(this);

    sizer->Add(toolbarSizer, 0, wxEXPAND | wxALL);
    sizer->Add(midiPanel, 1, wxEXPAND | wxALL);

    this->SetSizer(sizer);

    voice = new NesoraMikomiVoice();
    voice->SetSource(new NesoraSinSource());
    voice->SetFilter(new NesoraThroughFilter());

    Bind(wxEVT_CHAR_HOOK, &nsSingPanel::OnCharHook, this);
}

void nsSingPanel::ToolBar() {
    wxImage::AddHandler(new wxPNGHandler);
    toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER);
    wxBitmap stopBitmap(NESORA_STOP_ICON_24);
    toolbar->AddTool(nsID_STOP, _("Stop"), stopBitmap, wxNullBitmap, wxITEM_NORMAL, _("Stop the current singing data"));
    wxBitmap playBitmap(NESORA_PLAY_ICON_24);
    toolbar->AddTool(nsID_PLAY, _("Play"), playBitmap, wxNullBitmap, wxITEM_NORMAL, _("Play the current singing data"));
    wxBitmap pauseBitmap(NESORA_PAUSE_ICON_24);
    toolbar->AddTool(nsID_PAUSE, _("Pause"), pauseBitmap, wxNullBitmap, wxITEM_NORMAL, _("Pause the current singing data"));

    toolbar->Realize();

    Bind(wxEVT_TOOL, &nsSingPanel::OnStop, this, nsID_STOP);
    Bind(wxEVT_TOOL, &nsSingPanel::OnPlay, this, nsID_PLAY);
    Bind(wxEVT_TOOL, &nsSingPanel::OnPause, this, nsID_PAUSE);
}

void nsSingPanel::OnStop(wxCommandEvent& event) {
    UninitAudioDevice();
    midiPanel->PlayStop();
    isPlaying = false;
}

void nsSingPanel::OnPlay(wxCommandEvent& event) {
    InitAudioDevice();
    isPlaying = true;
}

void nsSingPanel::OnPause(wxCommandEvent& event) {
    UninitAudioDevice();
    isPlaying = false;
}

void nsSingPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_("The process for saving the singing panel has not yet been implemented..."));
}

void nsSingPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_("The process for opening the singing panel has not yet been implemented..."));
}

void nsSingPanel::OnCharHook(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_SPACE and midiPanel != nullptr and midiPanel->IsLyricEditing()) {
        event.Skip();
        return;
    }

    if (event.GetKeyCode() == WXK_SPACE) {
        if (isPlaying) {
            UninitAudioDevice();
            isPlaying = false;
        } else {
            InitAudioDevice();
            isPlaying = true;
        }
    }
    event.Skip();
}

void nsSingPanel::OnVolumeSlide(wxCommandEvent& event) {
    volumeText->SetLabel(to_string_with_precision(-10.0 + (double)volume->GetValue() / 10.0, 2) + " dB");
}

void nsSingPanel::InitAudioDevice() {
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate        = NesoraDefaultSamplingFrequency;
    deviceConfig.dataCallback      = nsSingPanel::data_callback;
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

void nsSingPanel::UninitAudioDevice() {
    ma_device_uninit(&device);
}

void nsSingPanel::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* out = (float*)pOutput;
    (void)pInput;

    nsSingPanel* frame = (nsSingPanel*)pDevice->pUserData;
    for (ma_uint32 i = 0; i < frameCount; i++) {
        double nowPitch = frame->midiPanel->GetPitch(NesoraDefaultSamplingFrequency);
        if (nowPitch > 0) {
            out[i] = (float)frame->voice->Synthesize(nowPitch, NesoraDefaultSamplingFrequency) / (std::pow(10.0, 10.0 - (float)frame->volume->GetValue() / 10.0));
        } else {
            out[i] = 0.0f;
        }
    }
}



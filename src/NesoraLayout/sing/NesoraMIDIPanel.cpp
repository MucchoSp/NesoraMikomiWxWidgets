// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraMIDIPanel.h"

// MARK: 座標変換関数群(参考用)
inline double TimeToPixel(double timeInMs, double pixelPerSecond) {
    return timeInMs / 1000.0 * pixelPerSecond;
}

inline double PixelToTime(double pixel, double pixelPerSecond) {
    return pixel / pixelPerSecond * 1000.0;
}

inline double PitchToPixel(double pitch) {
    return NESORA_MIDI_PANEL_A4_KEY_Y - (NESORA_MIDI_PANEL_NOTE_HEIGHT * 12) * std::log2(pitch / 440.0);
}

inline double PixelToPitch(double pixel) {
    return 440.0 * std::pow(2, (NESORA_MIDI_PANEL_A4_KEY_Y - pixel) / (NESORA_MIDI_PANEL_NOTE_HEIGHT * 12));
}

inline double BeetToPixel(double beet, double pixelPerBeet) {
    return beet * pixelPerBeet;
}

inline double PixelToBeet(double pixel, double pixelPerBeet) {
    return pixel / pixelPerBeet;
}

inline double BarToPixel(double bar, double pixelPerBeet, double timeSignatureNumerator) {
    return bar * timeSignatureNumerator * pixelPerBeet;
}

inline double PixelToBar(double pixel, double pixelPerBeet, double timeSignatureNumerator) {
    return pixel / (pixelPerBeet * timeSignatureNumerator);
}

inline double BarToBeet(double bar, double timeSignatureNumerator) {
    return bar * timeSignatureNumerator;
}

inline double BeetToBar(double beet, double timeSignatureNumerator) {
    return beet / timeSignatureNumerator;
}

inline double TimeToBeet(double timeInMs, double bpm) {
    return timeInMs / 1000.0 * (bpm / 60.0);
}

inline double BeetToTime(double beet, double bpm) {
    return beet / (bpm / 60.0) * 1000.0;
}

inline double BarToTime(double bar, double bpm, double timeSignatureNumerator) {
    return BarToBeet(bar, timeSignatureNumerator) / (bpm / 60.0) * 1000.0;
}

inline double TimeToBar(double timeInMs, double bpm, double timeSignatureNumerator) {
    return BeetToBar(TimeToBeet(timeInMs, bpm), timeSignatureNumerator);
}

inline double BeetToPixelPerSecond(double beet, double bpm) {
    return (bpm / 60.0) * (beet / TimeToBeet(1000.0, bpm));
}

inline double PixelPerSecondToBeet(double pixelPerSecond, double bpm) {
    return TimeToBeet(1000.0, bpm) * (pixelPerSecond / (bpm / 60.0));
}

// マウスポインタが一定以上動いたか
inline bool IsPointMoved(const wxPoint2DDouble& p1, const wxPoint2DDouble& p2, int threshold = 4) {
    return std::abs(p2.m_x - p1.m_x) > threshold or std::abs(p2.m_y - p1.m_y) > threshold;
}

// MIDIノートボックスのリストをMIDIノートのリストに変換する
inline std::vector<NesoraMidiNotePhoneticalInfo> MIDINoteBoxToMidiNote(std::vector<MidiNoteBox>& boxes, double pixelPerSecond) {
    std::vector<NesoraMidiNotePhoneticalInfo> notes;
    for (auto& box : boxes) {
        box.note.length = box.rect.m_width / pixelPerSecond * 1000.0; // 長さをmsに変換
        box.note.intensity = 1.0; // 仮に強さは常に1.0
        box.note.pitch = PixelToPitch(box.rect.m_y - NESORA_MIDI_PANEL_NOTE_HEIGHT / 2); // ピッチを計算（A4=69とする）
        notes.push_back(box.note);
    }
    return notes;
}

MidiNoteBox NesoraPianoRollCanvas::CreateNewMidiNoteBox(MidiNoteBox box) {
    box.note.frontOverlap = 0.0;
    box.note.backOverlap = 0.0;
    box.note.strength = 1.0;
    box.note.frontTransitionTime = 0.0;
    box.note.backTransitionTime = 0.0;

    box.note.cl_length = 0.0;
    box.note.uv_length = 0.0;
    box.note.sv_length = 0.0;

    box.note.frontPitchMoveTime = 100.0;
    box.note.frontPitchMoveTimming = 50.0;
    box.note.frontPitchMoveCurve = NesoraPitchCurveType::SINE;

    box.note.modulationStrength = 50.0;
    box.note.modulationFrequency = 10.0;
    box.note.modulationStartTime = 500.0;

    box.note.modulationFadeInTime = 100.0;
    box.note.modulationFadeOutTime = 100.0;

    box.note.overshootTime = 100.0;
    box.note.overshootPitch = 10.0;

    box.note.preparationTime = 100.0;
    box.note.preparationPitch = 10.0;

    box.controlPoints[MidiNoteBoxControlPointID::FrontPitchMoveTimmingControlPoint] = wxRect2DDouble(TimeToPixel(-box.note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                                 box.rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::OvershootPitchControlPoint] = wxRect2DDouble(TimeToPixel(box.note.frontPitchMoveTime - box.note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                          box.note.overshootPitch * NESORA_MIDI_PANEL_NOTE_HEIGHT / 100.0 + box.rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::PreparationPitchControlPoint] = wxRect2DDouble(box.rect.m_x + box.rect.m_width - TimeToPixel(box.note.preparationTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                            box.note.preparationPitch * NESORA_MIDI_PANEL_NOTE_HEIGHT / 100.0 + box.rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint] = wxRect2DDouble(TimeToPixel(box.note.modulationStartTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                      box.note.modulationStrength * NESORA_MIDI_PANEL_NOTE_HEIGHT / 100.0 + box.rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationFrequencyControlPoint] = wxRect2DDouble(TimeToPixel(box.note.modulationStartTime + 1000.0 / box.note.modulationFrequency, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                               box.rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint] = wxRect2DDouble(TimeToPixel(box.note.modulationStartTime + box.note.modulationFadeInTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                                box.rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationFadeOutTimeControlPoint] = wxRect2DDouble(TimeToPixel(box.note.length - box.note.preparationTime - box.note.modulationFadeOutTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                                 box.rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);

    return box;
}




// MARK:NesoraPhoneticalMIDINoteEditor


void NesoraPhoneticalMIDINoteEditor::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI Note"));
    wxSizer* frontOverlap_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* backOverlap_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* strength_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* frontTransitionTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* backTransitionTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* cl_length_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* uv_length_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sv_length_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* frontPitchMoveTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* frontPitchMoveTimming_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* modulationStrength_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* modulationFrequency_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* modulationStartTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* modulationFadeInTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* modulationFadeOutTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* overshootTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* overshootPitch_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* preparationTime_sliderSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* preparationPitch_sliderSizer = new wxBoxSizer(wxHORIZONTAL);

    frontOverlap_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "FO");
    frontOverlap_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    frontOverlap_param->SetToolTip(_("Amount of front overlap in ms."));
    frontOverlap_sliderSizer->Add(frontOverlap_param, 0, wxEXPAND | wxALL, 5);
    frontOverlap_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    frontOverlap_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnFrontOverlapSlide, this);
    frontOverlap_slider->SetToolTip(_("Amount of front overlap in ms."));
    frontOverlap_slider->Show();
    frontOverlap_sliderSizer->Add(frontOverlap_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(frontOverlap_sliderSizer, 0, wxEXPAND | wxALL);

    backOverlap_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "BO");
    backOverlap_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    backOverlap_param->SetToolTip(_("Amount of back overlap in ms."));
    backOverlap_sliderSizer->Add(backOverlap_param, 0, wxEXPAND | wxALL, 5);
    backOverlap_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    backOverlap_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnBackOverlapSlide, this);
    backOverlap_slider->SetToolTip(_("Amount of back overlap in ms."));
    backOverlap_slider->Show();
    backOverlap_sliderSizer->Add(backOverlap_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(backOverlap_sliderSizer, 0, wxEXPAND | wxALL);

    strength_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "ST");
    strength_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    strength_param->SetToolTip(_("Strength of the transition."));
    strength_sliderSizer->Add(strength_param, 0, wxEXPAND | wxALL, 5);
    strength_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    strength_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnStrengthSlide, this);
    strength_slider->SetToolTip(_("Strength of the transition."));
    strength_slider->Show();
    strength_sliderSizer->Add(strength_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(strength_sliderSizer, 0, wxEXPAND | wxALL);

    frontTransitionTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "FT");
    frontTransitionTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    frontTransitionTime_param->SetToolTip(_("Amount of front transition time in ms."));
    frontTransitionTime_sliderSizer->Add(frontTransitionTime_param, 0, wxEXPAND | wxALL, 5);
    frontTransitionTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    frontTransitionTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnFrontTransitionTimeSlide, this);
    frontTransitionTime_slider->SetToolTip(_("Amount of front transition time in ms."));
    frontTransitionTime_slider->Show();
    frontTransitionTime_sliderSizer->Add(frontTransitionTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(frontTransitionTime_sliderSizer, 0, wxEXPAND | wxALL);

    backTransitionTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "BT");
    backTransitionTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    backTransitionTime_param->SetToolTip(_("Amount of back transition time in ms."));
    backTransitionTime_sliderSizer->Add(backTransitionTime_param, 0, wxEXPAND | wxALL, 5);
    backTransitionTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    backTransitionTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnBackTransitionTimeSlide, this);
    backTransitionTime_slider->SetToolTip(_("Amount of back transition time in ms."));
    backTransitionTime_slider->Show();
    backTransitionTime_sliderSizer->Add(backTransitionTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(backTransitionTime_sliderSizer, 0, wxEXPAND | wxALL);

    cl_length_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "CL");
    cl_length_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    cl_length_param->SetToolTip(_("Amount of consonant length in ms."));
    cl_length_sliderSizer->Add(cl_length_param, 0, wxEXPAND | wxALL, 5);
    cl_length_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    cl_length_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnCLLengthSlide, this);
    cl_length_slider->SetToolTip(_("Amount of consonant length in ms."));
    cl_length_slider->Show();
    cl_length_sliderSizer->Add(cl_length_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(cl_length_sliderSizer, 0, wxEXPAND | wxALL);

    uv_length_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "UV");
    uv_length_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    uv_length_param->SetToolTip(_("Amount of vowel length in ms."));
    uv_length_sliderSizer->Add(uv_length_param, 0, wxEXPAND | wxALL, 5);
    uv_length_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    uv_length_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnUVLengthSlide, this);
    uv_length_slider->SetToolTip(_("Amount of vowel length in ms."));
    uv_length_slider->Show();
    uv_length_sliderSizer->Add(uv_length_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(uv_length_sliderSizer, 0, wxEXPAND | wxALL);

    sv_length_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "SV");
    sv_length_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    sv_length_param->SetToolTip(_("Amount of syllable length in ms."));
    sv_length_sliderSizer->Add(sv_length_param, 0, wxEXPAND | wxALL, 5);
    sv_length_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    sv_length_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnSVLengthSlide, this);
    sv_length_slider->SetToolTip(_("Amount of syllable length in ms."));
    sv_length_slider->Show();
    sv_length_sliderSizer->Add(sv_length_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(sv_length_sliderSizer, 0, wxEXPAND | wxALL);

    frontPitchMoveTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "FPT");
    frontPitchMoveTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    frontPitchMoveTime_param->SetToolTip(_("Amount of front pitch move time in ms."));
    frontPitchMoveTime_sliderSizer->Add(frontPitchMoveTime_param, 0, wxEXPAND | wxALL, 5);
    frontPitchMoveTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    frontPitchMoveTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnFrontPitchMoveTimeSlide, this);
    frontPitchMoveTime_slider->SetToolTip(_("Amount of front pitch move time in ms."));
    frontPitchMoveTime_slider->Show();
    frontPitchMoveTime_sliderSizer->Add(frontPitchMoveTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(frontPitchMoveTime_sliderSizer, 0, wxEXPAND | wxALL);

    frontPitchMoveTimming_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "FPTm");
    frontPitchMoveTimming_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    frontPitchMoveTimming_param->SetToolTip(_("Amount of front pitch move timming in ms."));
    frontPitchMoveTimming_sliderSizer->Add(frontPitchMoveTimming_param, 0, wxEXPAND | wxALL, 5);
    frontPitchMoveTimming_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    frontPitchMoveTimming_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnFrontPitchMoveTimmingSlide, this);
    frontPitchMoveTimming_slider->SetToolTip(_("Amount of front pitch move timming in ms."));
    frontPitchMoveTimming_slider->Show();
    frontPitchMoveTimming_sliderSizer->Add(frontPitchMoveTimming_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(frontPitchMoveTimming_sliderSizer, 0, wxEXPAND | wxALL);

    modulationStrength_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "MS");
    modulationStrength_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    modulationStrength_param->SetToolTip(_("Amount of modulation strength in ms."));
    modulationStrength_sliderSizer->Add(modulationStrength_param, 0, wxEXPAND | wxALL, 5);
    modulationStrength_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    modulationStrength_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnModulationStrengthSlide, this);
    modulationStrength_slider->SetToolTip(_("Amount of modulation strength in ms."));
    modulationStrength_slider->Show();
    modulationStrength_sliderSizer->Add(modulationStrength_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(modulationStrength_sliderSizer, 0, wxEXPAND | wxALL);

    modulationFrequency_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "MF");
    modulationFrequency_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    modulationFrequency_param->SetToolTip(_("Amount of modulation frequency in ms."));
    modulationFrequency_sliderSizer->Add(modulationFrequency_param, 0, wxEXPAND | wxALL, 5);
    modulationFrequency_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    modulationFrequency_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnModulationFrequencySlide, this);
    modulationFrequency_slider->SetToolTip(_("Amount of modulation frequency in ms."));
    modulationFrequency_slider->Show();
    modulationFrequency_sliderSizer->Add(modulationFrequency_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(modulationFrequency_sliderSizer, 0, wxEXPAND | wxALL);

    modulationStartTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "MST");
    modulationStartTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    modulationStartTime_param->SetToolTip(_("Amount of modulation start time in ms."));
    modulationStartTime_sliderSizer->Add(modulationStartTime_param, 0, wxEXPAND | wxALL, 5);
    modulationStartTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    modulationStartTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnModulationStartTimeSlide, this);
    modulationStartTime_slider->SetToolTip(_("Amount of modulation start time in ms."));
    modulationStartTime_slider->Show();
    modulationStartTime_sliderSizer->Add(modulationStartTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(modulationStartTime_sliderSizer, 0, wxEXPAND | wxALL);

    modulationFadeInTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "MFI");
    modulationFadeInTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    modulationFadeInTime_param->SetToolTip(_("Amount of modulation fade in time in ms."));
    modulationFadeInTime_sliderSizer->Add(modulationFadeInTime_param, 0, wxEXPAND | wxALL, 5);
    modulationFadeInTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    modulationFadeInTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnModulationFadeInTimeSlide, this);
    modulationFadeInTime_slider->SetToolTip(_("Amount of modulation fade in time in ms."));
    modulationFadeInTime_slider->Show();
    modulationFadeInTime_sliderSizer->Add(modulationFadeInTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(modulationFadeInTime_sliderSizer, 0, wxEXPAND | wxALL);

    modulationFadeOutTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "MFO");
    modulationFadeOutTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    modulationFadeOutTime_param->SetToolTip(_("Amount of modulation fade out time in ms."));
    modulationFadeOutTime_sliderSizer->Add(modulationFadeOutTime_param, 0, wxEXPAND | wxALL, 5);
    modulationFadeOutTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    modulationFadeOutTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnModulationFadeOutTimeSlide, this);
    modulationFadeOutTime_slider->SetToolTip(_("Amount of modulation fade out time in ms."));
    modulationFadeOutTime_slider->Show();
    modulationFadeOutTime_sliderSizer->Add(modulationFadeOutTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(modulationFadeOutTime_sliderSizer, 0, wxEXPAND | wxALL);

    overshootTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "OST");
    overshootTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    overshootTime_param->SetToolTip(_("Amount of overshoot time in ms."));
    overshootTime_sliderSizer->Add(overshootTime_param, 0, wxEXPAND | wxALL, 5);
    overshootTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    overshootTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnOvershootTimeSlide, this);
    overshootTime_slider->SetToolTip(_("Amount of overshoot time in ms."));
    overshootTime_slider->Show();
    overshootTime_sliderSizer->Add(overshootTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(overshootTime_sliderSizer, 0, wxEXPAND | wxALL);

    overshootPitch_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "OSP");
    overshootPitch_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    overshootPitch_param->SetToolTip(_("Amount of overshoot pitch in ms."));
    overshootPitch_sliderSizer->Add(overshootPitch_param, 0, wxEXPAND | wxALL, 5);
    overshootPitch_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    overshootPitch_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnOvershootPitchSlide, this);
    overshootPitch_slider->SetToolTip(_("Amount of overshoot pitch in ms."));
    overshootPitch_slider->Show();
    overshootPitch_sliderSizer->Add(overshootPitch_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(overshootPitch_sliderSizer, 0, wxEXPAND | wxALL);

    preparationTime_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "PT");
    preparationTime_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    preparationTime_param->SetToolTip(_("Amount of preparation time in ms."));
    preparationTime_sliderSizer->Add(preparationTime_param, 0, wxEXPAND | wxALL, 5);
    preparationTime_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    preparationTime_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnPreparationTimeSlide, this);
    preparationTime_slider->SetToolTip(_("Amount of preparation time in ms."));
    preparationTime_slider->Show();
    preparationTime_sliderSizer->Add(preparationTime_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(preparationTime_sliderSizer, 0, wxEXPAND | wxALL);

    preparationPitch_param = new wxStaticText(sourceSizer->GetStaticBox(), wxID_ANY, "PP");
    preparationPitch_param->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    preparationPitch_param->SetToolTip(_("Amount of preparation pitch in ms."));
    preparationPitch_sliderSizer->Add(preparationPitch_param, 0, wxEXPAND | wxALL, 5);
    preparationPitch_slider = new nsSlider(sourceSizer->GetStaticBox(), wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(200, 15));
    preparationPitch_slider->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &NesoraPhoneticalMIDINoteEditor::OnPreparationPitchSlide, this);
    preparationPitch_slider->SetToolTip(_("Amount of preparation pitch in ms."));
    preparationPitch_slider->Show();
    preparationPitch_sliderSizer->Add(preparationPitch_slider, 1, wxEXPAND | wxALL, 5);
    sourceSizer->Add(preparationPitch_sliderSizer, 0, wxEXPAND | wxALL);

    SetSizer(sourceSizer);

    Bind(wxEVT_PAINT, &NesoraPhoneticalMIDINoteEditor::OnPaint, this);
}

void NesoraPhoneticalMIDINoteEditor::EditNote(MidiNoteBox* n) {
    if (n) {
        Show();
        note = n;
        note->note.frontOverlap = std::clamp(note->note.frontOverlap, 0.0, frontOverlapMax);
        note->note.backOverlap = std::clamp(note->note.backOverlap, 0.0, backOverlapMax);
        note->note.strength = std::clamp(note->note.strength, 0.0, strengthMax);
        note->note.frontTransitionTime = std::clamp(note->note.frontTransitionTime, 0.0, transitionTimeMax);
        note->note.backTransitionTime = std::clamp(note->note.backTransitionTime, 0.0, transitionTimeMax);
        note->note.cl_length = std::clamp(note->note.cl_length, 0.0, clLengthMax);
        note->note.uv_length = std::clamp(note->note.uv_length, 0.0, uvLengthMax);
        note->note.sv_length = std::clamp(note->note.sv_length, 0.0, svLengthMax);
        note->note.frontPitchMoveTime = std::clamp(note->note.frontPitchMoveTime, 0.0, frontPitchMoveTimeMax);
        note->note.frontPitchMoveTimming = std::clamp(note->note.frontPitchMoveTimming, 0.0, frontPitchMoveTimmingMax);
        note->note.modulationStrength = std::clamp(note->note.modulationStrength, 0.0, modulationStrengthMax);
        note->note.modulationFrequency = std::clamp(note->note.modulationFrequency, 0.0, modulationFrequencyMax);
        note->note.modulationStartTime = std::clamp(note->note.modulationStartTime, 0.0, modulationStartTimeMax);
        note->note.modulationFadeInTime = std::clamp(note->note.modulationFadeInTime, 0.0, modulationFadeTimeMax);
        note->note.modulationFadeOutTime = std::clamp(note->note.modulationFadeOutTime, 0.0, modulationFadeTimeMax);
        note->note.overshootTime = std::clamp(note->note.overshootTime, 0.0, overshootTimeMax);
        note->note.overshootPitch = std::clamp(note->note.overshootPitch, 0.0, overshootPitchMax);
        note->note.preparationTime = std::clamp(note->note.preparationTime, 0.0, preparationTimeMax);
        note->note.preparationPitch = std::clamp(note->note.preparationPitch, 0.0, preparationPitchMax);
        
        frontOverlap_slider->SetValue(note->note.frontOverlap / frontOverlapMax * 1000.0);
        backOverlap_slider->SetValue(note->note.backOverlap / backOverlapMax * 1000.0);
        strength_slider->SetValue(note->note.strength / strengthMax * 1000.0);
        frontTransitionTime_slider->SetValue(note->note.frontTransitionTime / transitionTimeMax * 1000.0);
        backTransitionTime_slider->SetValue(note->note.backTransitionTime / transitionTimeMax * 1000.0);
        cl_length_slider->SetValue(note->note.cl_length / clLengthMax * 1000.0);
        uv_length_slider->SetValue(note->note.uv_length / uvLengthMax * 1000.0);
        sv_length_slider->SetValue(note->note.sv_length / svLengthMax * 1000.0);
        frontPitchMoveTime_slider->SetValue(note->note.frontPitchMoveTime / frontPitchMoveTimeMax * 1000.0);
        frontPitchMoveTimming_slider->SetValue(note->note.frontPitchMoveTimming / frontPitchMoveTimmingMax * 1000.0);
        modulationStrength_slider->SetValue(note->note.modulationStrength / modulationStrengthMax * 1000.0);
        modulationFrequency_slider->SetValue(note->note.modulationFrequency / modulationFrequencyMax * 1000.0);
        modulationStartTime_slider->SetValue(note->note.modulationStartTime / modulationStartTimeMax * 1000.0);
        modulationFadeInTime_slider->SetValue(note->note.modulationFadeInTime / modulationFadeTimeMax * 1000.0);
        modulationFadeOutTime_slider->SetValue(note->note.modulationFadeOutTime / modulationFadeTimeMax * 1000.0);
        overshootTime_slider->SetValue(note->note.overshootTime / overshootTimeMax * 1000.0);
        overshootPitch_slider->SetValue(note->note.overshootPitch / overshootPitchMax * 1000.0);
        preparationTime_slider->SetValue(note->note.preparationTime / preparationTimeMax * 1000.0);
        preparationPitch_slider->SetValue(note->note.preparationPitch / preparationPitchMax * 1000.0);
    } else {
        Hide();
    }
    parentSizer->Layout();
    Refresh();
}

void NesoraPhoneticalMIDINoteEditor::BindNoteUpdateEvent(std::function<void()> handler) {
    m_noteUpdateHandler = handler;
}

void NesoraPhoneticalMIDINoteEditor::OnPaint(wxPaintEvent& event) {

}

void NesoraPhoneticalMIDINoteEditor::OnFrontOverlapSlide(wxCommandEvent& event) {
    if (note) {
        note->note.frontOverlap = frontOverlap_slider->GetValue() / 1000.0 * frontOverlapMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnBackOverlapSlide(wxCommandEvent& event) {
    if (note) {
        note->note.backOverlap = backOverlap_slider->GetValue() / 1000.0 * backOverlapMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnStrengthSlide(wxCommandEvent& event) {
    if (note) {
        note->note.strength = strength_slider->GetValue() / 1000.0 * strengthMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnFrontTransitionTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.frontTransitionTime = frontTransitionTime_slider->GetValue() / 1000.0 * transitionTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnBackTransitionTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.backTransitionTime = backTransitionTime_slider->GetValue() / 1000.0 * transitionTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnCLLengthSlide(wxCommandEvent& event) {
    if (note) {
        note->note.cl_length = cl_length_slider->GetValue() / 1000.0 * clLengthMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnUVLengthSlide(wxCommandEvent& event) {
    if (note) {
        note->note.uv_length = uv_length_slider->GetValue() / 1000.0 * uvLengthMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnSVLengthSlide(wxCommandEvent& event) {
    if (note) {
        note->note.sv_length = sv_length_slider->GetValue() / 1000.0 * svLengthMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnFrontPitchMoveTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.frontPitchMoveTime = frontPitchMoveTime_slider->GetValue() / 1000.0 * frontPitchMoveTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnFrontPitchMoveTimmingSlide(wxCommandEvent& event) {
    if (note) {
        note->note.frontPitchMoveTimming = frontPitchMoveTimming_slider->GetValue() / 1000.0 * frontPitchMoveTimmingMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnModulationStrengthSlide(wxCommandEvent& event) {
    if (note) {
        note->note.modulationStrength = modulationStrength_slider->GetValue() / 1000.0 * modulationStrengthMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnModulationFrequencySlide(wxCommandEvent& event) {
    if (note) {
        note->note.modulationFrequency = modulationFrequency_slider->GetValue() / 1000.0 * modulationFrequencyMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnModulationStartTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.modulationStartTime = modulationStartTime_slider->GetValue() / 1000.0 * modulationStartTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnModulationFadeInTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.modulationFadeInTime = modulationFadeInTime_slider->GetValue() / 1000.0 * modulationFadeTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnModulationFadeOutTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.modulationFadeOutTime = modulationFadeOutTime_slider->GetValue() / 1000.0 * modulationFadeTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnOvershootTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.overshootTime = overshootTime_slider->GetValue() / 1000.0 * overshootTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnOvershootPitchSlide(wxCommandEvent& event) {
    if (note) {
        note->note.overshootPitch = overshootPitch_slider->GetValue() / 1000.0 * overshootPitchMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnPreparationTimeSlide(wxCommandEvent& event) {
    if (note) {
        note->note.preparationTime = preparationTime_slider->GetValue() / 1000.0 * preparationTimeMax;
    }
    m_noteUpdateHandler();
}

void NesoraPhoneticalMIDINoteEditor::OnPreparationPitchSlide(wxCommandEvent& event) {
    if (note) {
        note->note.preparationPitch = preparationPitch_slider->GetValue() / 1000.0 * preparationPitchMax;
    }
    m_noteUpdateHandler();
}









// MARK:NesoraPianoRollCanvas

void NesoraPianoRollCanvas::Init() {
    // 背景のちらつきを防ぐ設定
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    // スクロール範囲の設定(横: 4小節, 縦: 128鍵分 * 20px)
    screenWidth = pixelPerBeet * timeSignatureNumerator * 4.0;
    screenHeight = 128 * 20;
    SetScrollbars(ppux, ppuy, screenWidth / ppux, screenHeight / ppuy);

    // イベントバインド
    Bind(wxEVT_PAINT, &NesoraPianoRollCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraPianoRollCanvas::OnLeftDown, this);
    Bind(wxEVT_LEFT_DCLICK, &NesoraPianoRollCanvas::OnLeftDClick, this);
    Bind(wxEVT_LEFT_UP, &NesoraPianoRollCanvas::OnLeftUp, this);
    Bind(wxEVT_RIGHT_DOWN, &NesoraPianoRollCanvas::OnRightDown, this);
    Bind(wxEVT_RIGHT_UP, &NesoraPianoRollCanvas::OnRightUp, this);
    Bind(wxEVT_MOTION, &NesoraPianoRollCanvas::OnMouseMove, this);
    Bind(wxEVT_KEY_DOWN, &NesoraPianoRollCanvas::OnKeyDown, this);
    Bind(wxEVT_SIZE, &NesoraPianoRollCanvas::OnSize, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_LINEUP, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_LINEDOWN, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_PAGEUP, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_PAGEDOWN, &NesoraPianoRollCanvas::OnScroll, this);

    lyricEditor = new wxTextCtrl(this,
                                 wxID_ANY,
                                 wxEmptyString,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
    lyricEditor->Hide();
    lyricEditor->Bind(wxEVT_KILL_FOCUS, &NesoraPianoRollCanvas::OnLyricEditorKillFocus, this);
    lyricEditor->Bind(wxEVT_CHAR_HOOK, &NesoraPianoRollCanvas::OnLyricEditorCharHook, this);
}

void NesoraPianoRollCanvas::SetLinkedMIDINoteEditor(NesoraPhoneticalMIDINoteEditor* editor) {
    m_linkedMIDINoteEditor = editor;
    if (m_linkedMIDINoteEditor) {
        m_linkedMIDINoteEditor->BindNoteUpdateEvent([this]{
            double pixcelPerSecond = pixelPerBeet * bpm / 60.0;
            midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixcelPerSecond));
            midiScript.CalculateNoteParam(pixcelPerSecond);
            pitchLine = midiScript.GetPitchLine();
            Refresh();
        });
    }
}

// 歌詞の編集を開始する。noteIdxは編集するノートのインデックス
void NesoraPianoRollCanvas::BeginLyricEdit(int noteIdx) {
    if (noteIdx < 0 or noteIdx >= (int)notes.size() or lyricEditor == nullptr) {
        return;
    }

    editingNoteIdx = noteIdx;
    editingOriginalLyric = notes[noteIdx].note.lyric;

    int sx = 0;
    int sy = 0;
    CalcScrolledPosition(notes[noteIdx].rect.m_x, notes[noteIdx].rect.m_y, &sx, &sy);

    int editorW = std::max(40, (int)notes[noteIdx].rect.m_width);
    int editorH = std::max(18, (int)notes[noteIdx].rect.m_height);
    lyricEditor->SetSize(sx, sy, editorW, editorH);
    lyricEditor->SetValue(wxString::FromUTF8(notes[noteIdx].note.lyric));
    lyricEditor->Show();
    lyricEditor->SetFocus();
    lyricEditor->SelectAll();
}

// 編集を終了する。commitがtrueなら変更を保存、falseなら破棄
void NesoraPianoRollCanvas::EndLyricEdit(bool commit) {
    if (editingNoteIdx < 0 || editingNoteIdx >= (int)notes.size() || lyricEditor == nullptr) {
        editingNoteIdx = -1;
        return;
    }

    if (commit) {
        notes[editingNoteIdx].note.lyric = std::string(lyricEditor->GetValue().utf8_string());
        midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixelPerBeet * bpm / 60.0));
    } else {
        notes[editingNoteIdx].note.lyric = editingOriginalLyric;
    }

    lyricEditor->Hide();
    editingNoteIdx = -1;
    Refresh(false);
}

// 選択されているノートを、Xでソートして順番を返す
std::vector<int> NesoraPianoRollCanvas::GetSelectedNoteOrder() const {
    std::vector<int> selectedIndices;
    for (size_t i = 0; i < notes.size(); ++i) {
        if (notes[i].isSelected) {
            selectedIndices.push_back(i);
        }
    }

    // ソートの基準は、X座標、次にY座標、最後にID（重複しないようにソートしてあるから不要なはず）
    // std::sort(selectedIndices.begin(), selectedIndices.end(), [this](int left, int right) {
    //     const MidiNoteBox& leftNote = notes[left];
    //     const MidiNoteBox& rightNote = notes[right];
    //     if (leftNote.rect.m_x != rightNote.rect.m_x) {
    //         return leftNote.rect.m_x < rightNote.rect.m_x;
    //     }
    //     if (leftNote.rect.m_y != rightNote.rect.m_y) {
    //         return leftNote.rect.m_y < rightNote.rect.m_y;
    //     }
    //     return leftNote.id < rightNote.id;
    // });

    return selectedIndices;
}

// 現在選択されたノートの中で、currentNoteIdxの次に来るノートのインデックスを返す。見つからない場合は-1
int NesoraPianoRollCanvas::GetNextSelectedNoteIndex(int currentNoteIdx) const {
    std::vector<int> selectedIndices = GetSelectedNoteOrder();
    auto currentIt = std::find(selectedIndices.begin(), selectedIndices.end(), currentNoteIdx);
    if (currentIt == selectedIndices.end()) {
        return selectedIndices.empty() ? -1 : selectedIndices.front();
    }

    currentIt++;
    if (currentIt == selectedIndices.end()) {
        return -1;
    }

    return *currentIt;
}

wxPoint2DDouble NesoraPianoRollCanvas::GetMousePos(const wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    int x, y;
    CalcUnscrolledPosition(pos.x, pos.y, &x, &y);
    return wxPoint2DDouble(x, y);
}

// ノートの右端エリア（リサイズハンドル）の矩形を取得
wxRect2DDouble NesoraPianoRollCanvas::GetRightResizeHandleRect(const MidiNoteBox& note) {
    return wxRect2DDouble(note.rect.m_x + note.rect.m_width - NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_y,
                            NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_height);
}

// ノートの左端エリア（リサイズハンドル）の矩形を取得
wxRect2DDouble NesoraPianoRollCanvas::GetLeftResizeHandleRect(const MidiNoteBox& note) {
    return wxRect2DDouble(note.rect.m_x,
                            note.rect.m_y,
                            NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_height);
}

// 他のノートを押し出す、または整理する関数
void NesoraPianoRollCanvas::ResolveOverlaps() {
    currentX = 0.0;
    if (!notes.empty()){
        // 選択されているノートとそのほかのノートに分ける
        std::vector<MidiNoteBox> selectedNotes;
        std::vector<MidiNoteBox> nonSelectedNotes;
        
        for (const auto& n : notes) {
            if (n.isSelected)
                selectedNotes.push_back(n);
            else
                nonSelectedNotes.push_back(n);
        }

        // 両方のグループをX軸でソート
        auto sortFunc = [](const MidiNoteBox& a, const MidiNoteBox& b) {
            return a.rect.m_x < b.rect.m_x;
        };
        std::sort(selectedNotes.begin(), selectedNotes.end(), sortFunc);
        std::sort(nonSelectedNotes.begin(), nonSelectedNotes.end(), sortFunc);

        // 非選択ノートの間に選択ノートの塊を挿入する。
        double groupStartX = selectedNotes.empty() ? 0 : selectedNotes.front().rect.m_x;
        
        std::vector<MidiNoteBox> finalOrder;
        bool groupInserted = false;

        if (selectedNotes.empty()) {
            finalOrder = nonSelectedNotes;
        } else {
            for (const auto& n : nonSelectedNotes) {
                if (!groupInserted && (n.rect.m_x/* + n.rect.m_width / 2.0*/) >= groupStartX) {
                    for (const auto& sn : selectedNotes) finalOrder.push_back(sn);
                    groupInserted = true;
                }
                finalOrder.push_back(n);
            }
            if (!groupInserted) {
                for (const auto& sn : selectedNotes)
                    finalOrder.push_back(sn);
            }
        }

        // 隙間なく並べる
        for (size_t i = 0; i < finalOrder.size(); i++) {
            finalOrder[i].rect.m_x = currentX;
            currentX += finalOrder[i].rect.m_width;
        }

        notes = finalOrder;
    }

    // MIDIスクリプトのノート情報を更新
    double pixcelPerSecond = pixelPerBeet * bpm / 60.0;
    midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixcelPerSecond));
    midiScript.CalculateNoteParam(pixcelPerSecond);
    pitchLine = midiScript.GetPitchLine();
    
    // スクロール範囲の更新
    SetScrollWidth();

    // 選択されているノートが変更されたら
    ChangeSelectNote();
}

void NesoraPianoRollCanvas::PitchControlPointUpdate() {
    notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::FrontPitchMoveTimmingControlPoint] = wxRect2DDouble(TimeToPixel(-notes[selectedNoteIdx].note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                 notes[selectedNoteIdx].rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::OvershootPitchControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.frontPitchMoveTime - notes[selectedNoteIdx].note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                          notes[selectedNoteIdx].note.overshootPitch * NESORA_MIDI_PANEL_NOTE_HEIGHT / 100.0 + notes[selectedNoteIdx].rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::PreparationPitchControlPoint] = wxRect2DDouble(notes[selectedNoteIdx].rect.m_width - TimeToPixel(notes[selectedNoteIdx].note.preparationTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                            notes[selectedNoteIdx].note.preparationPitch * NESORA_MIDI_PANEL_NOTE_HEIGHT / 100.0 + notes[selectedNoteIdx].rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                      notes[selectedNoteIdx].note.modulationStrength * NESORA_MIDI_PANEL_NOTE_HEIGHT / 100.0 + notes[selectedNoteIdx].rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFrequencyControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + 1000.0 / notes[selectedNoteIdx].note.modulationFrequency, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                               notes[selectedNoteIdx].rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + notes[selectedNoteIdx].note.modulationFadeInTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                notes[selectedNoteIdx].rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
    notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFadeOutTimeControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.length - notes[selectedNoteIdx].note.preparationTime - notes[selectedNoteIdx].note.modulationFadeOutTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                 notes[selectedNoteIdx].rect.m_y - 5 + NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0, 10, 10);
}

// ピッチのコントロールポイントからピッチラインを更新する
void NesoraPianoRollCanvas::PitchLineUpdate() {
    if (selectedNoteIdx != -1 and draggingControlPointIdx != MidiNoteBoxControlPointID::None) {
        switch (draggingControlPointIdx) {
        case MidiNoteBoxControlPointID::FrontPitchMoveTimmingControlPoint: {
            notes[selectedNoteIdx].note.frontPitchMoveTimming = PixelToTime(-(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0), pixelPerBeet * bpm / 60.0);
            notes[selectedNoteIdx].note.frontPitchMoveTime = PixelToTime(notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::OvershootPitchControlPoint].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].note.frontPitchMoveTimming;
        }
        break;
        case MidiNoteBoxControlPointID::OvershootPitchControlPoint: {
            notes[selectedNoteIdx].note.frontPitchMoveTime = PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].note.frontPitchMoveTimming;
            notes[selectedNoteIdx].note.overshootPitch = (notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y - notes[selectedNoteIdx].rect.m_y + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0) * 100.0 / NESORA_MIDI_PANEL_NOTE_HEIGHT;
        }
        break;
        case MidiNoteBoxControlPointID::ModulationControlPoint: {
            notes[selectedNoteIdx].note.modulationStartTime = PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0);
            notes[selectedNoteIdx].note.modulationStrength = std::abs(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y - notes[selectedNoteIdx].rect.m_y + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0) * 100.0 / NESORA_MIDI_PANEL_NOTE_HEIGHT;
            notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFrequencyControlPoint].m_x = TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + 1000.0 / notes[selectedNoteIdx].note.modulationFrequency, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5;
            notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint].m_x = TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + notes[selectedNoteIdx].note.modulationFadeInTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5;
        }
        break;
        case MidiNoteBoxControlPointID::ModulationFrequencyControlPoint: {
            notes[selectedNoteIdx].note.modulationFrequency = 1000.0 / PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint].m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0);
        }
        break;
        case MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint: {
            notes[selectedNoteIdx].note.modulationFadeInTime = PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint].m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0);
        }
        break;
        case MidiNoteBoxControlPointID::ModulationFadeOutTimeControlPoint: {
            notes[selectedNoteIdx].note.modulationFadeOutTime = PixelToTime(-(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::PreparationPitchControlPoint].m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0), pixelPerBeet * bpm / 60.0);
        }
        break;
        case MidiNoteBoxControlPointID::PreparationPitchControlPoint: {
            notes[selectedNoteIdx].note.preparationTime = PixelToTime(-(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - (notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].rect.m_width) + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0), pixelPerBeet * bpm / 60.0);
            notes[selectedNoteIdx].note.preparationPitch = (notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y - notes[selectedNoteIdx].rect.m_y + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0) * 100.0 / NESORA_MIDI_PANEL_NOTE_HEIGHT;
        }
        default:
        break;
        }
    }
    double pixcelPerSecond = pixelPerBeet * bpm / 60.0;
    midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixcelPerSecond));
    midiScript.CalculateNoteParam(pixcelPerSecond);
    pitchLine = midiScript.GetPitchLine();
    Refresh();
}

// 選択ノートをクリア
void NesoraPianoRollCanvas::NoteSelectClear() {
    for (auto& note : notes)
        note.isSelected = false;
    
    // m_linkedMIDINoteEditor->EditNote(nullptr);
    selectedNoteIdx = -1;
    return;
}

// 選択されているノートが変更されたら
void NesoraPianoRollCanvas::ChangeSelectNote() {
    // 選択されているノートを分ける
    std::vector<int> selectedNotes;
    
    for (size_t i = 0;i < notes.size();i++) {
        if (notes[i].isSelected)
            selectedNotes.push_back(i);
    }
    
    if (selectedNotes.size() == 1) {
        // m_linkedMIDINoteEditor->EditNote(&notes[selectedNotes[0]]);
        selectedNoteIdx = selectedNotes[0];
        PitchControlPointUpdate();
    } else {
        // m_linkedMIDINoteEditor->EditNote(nullptr);
        selectedNoteIdx = -1;
    }
}

void NesoraPianoRollCanvas::SetScrollWidth() {
    // スクロール範囲の更新
    int x, y, w, h;
    GetViewStart(&x, &y);
    GetClientSize(&w, &h);
    scriptLengthInBar = std::max(PixelToBar(w, pixelPerBeet, timeSignatureNumerator), PixelToBar(currentX, pixelPerBeet, timeSignatureNumerator) + 2.0); // 2小節分の余裕を持たせる
    screenWidth = scriptLengthInBar * pixelPerBeet * timeSignatureNumerator;
    SetScrollbars(ppux, ppuy, screenWidth / ppux, screenHeight / ppuy, x, y);
    m_linkedTimeline->SetScriptLengthInBar(scriptLengthInBar);
}

// MARK: OnPaint
void NesoraPianoRollCanvas::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    DoPrepareDC(dc); // スクロール位置を計算に含める
    dc.Clear();

    wxSize size = GetClientSize();
    wxRect2DDouble visibleRect(GetViewStart().x * ppux, GetViewStart().y * ppuy, size.GetWidth(), size.GetHeight());
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        // グリッド（背景）の描画
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        for (int i = 0; i < NESORA_MIDI_PANEL_KEY_COUNT; ++i) {
            double y = i * NESORA_MIDI_PANEL_NOTE_HEIGHT;
            wxPoint2DDouble linePoints[] = {{ visibleRect.m_x, y }, { visibleRect.m_x + visibleRect.m_width, y }};
            gc->StrokeLines(2, linePoints); // 水平線
        }
        for (int x = 0; x < scriptLengthInBar * pixelPerBeet * timeSignatureNumerator; x += pixelPerBeet) {
            double drawX = x;
            if (drawX > scriptLengthInBar * pixelPerBeet * timeSignatureNumerator) break;
            wxPoint2DDouble linePoints[] = {{ drawX, visibleRect.m_y }, { drawX, visibleRect.m_y + visibleRect.m_height }};
            gc->StrokeLines(2, linePoints); // 垂直線
        }

        // ノートの描画
        for (size_t i = 0;i < notes.size();i++) {
            if (notes[i].rect.Intersects(visibleRect)) {
                if (notes[i].isSelected) {
                    if (hoverNoteIdx == i) {
                        gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_HOVER)));
                        gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY_HOVER)));
                    } else {
                        gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
                        gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY)));
                    }
                } else if (hoverNoteIdx == i) {
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
                    gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY_HOVER)));
                } else {
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
                    gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY)));
                }
                gc->DrawRectangle(notes[i].rect);

                // 歌詞の描画
                if (notes[i].isSelected) {
                    gc->SetFont(font, nsGetColor(nsColorType::ON_SECONDARY));
                } else {
                    gc->SetFont(font, nsGetColor(nsColorType::ON_PRIMARY));
                }
                std::string lyric = notes[i].note.lyric.empty() ? "<br>" : notes[i].note.lyric; // 空の歌詞は<br>として表示
                gc->DrawText(lyric, notes[i].rect.m_x + 4, notes[i].rect.m_y + 4);
            }
        }

        // 範囲選択枠の描画
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::SelectingRange) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_TRANSPARENT)));
            gc->SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));
            gc->DrawRectangle(currentSelectionRect.m_x, currentSelectionRect.m_y, currentSelectionRect.m_width, currentSelectionRect.m_height);
        }

        // ピッチラインの描画
        if (pitchLine.size() >= 2) {
            gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
            std::vector<wxPoint2DDouble> linePoints;
            for (size_t i = visibleRect.m_x; i < visibleRect.m_x + visibleRect.m_width && i < pitchLine.size(); i++) {
                linePoints.push_back(wxPoint2DDouble(i, PitchToPixel(pitchLine[i]) + NESORA_MIDI_PANEL_NOTE_HEIGHT)); // ピッチをY座標に変換
            }
            gc->StrokeLines(linePoints.size(), linePoints.data());
        }

        // 現在の再生位置ラインを描画
        const double pixelPerSecond = pixelPerBeet * bpm / 60.0;
        const double playbackTimeForDraw = std::max(0.0, playbackTimeInSec.load() - playbackVisualDelayInSec);
        const double playheadX = playbackTimeForDraw * pixelPerSecond;
        if (playheadX >= visibleRect.m_x && playheadX <= visibleRect.m_x + visibleRect.m_width) {
            gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
            gc->StrokeLine(playheadX, visibleRect.m_y, playheadX, visibleRect.m_y + visibleRect.m_height);
        }

        // ピッチのコントロールポイントを描画
        if (selectedNoteIdx != -1 && selectedNoteIdx < (int)notes.size()) {
            gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2));
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
            for (const auto& controlPoint : notes[selectedNoteIdx].controlPoints)
                gc->DrawEllipse(controlPoint.second.m_x, controlPoint.second.m_y, controlPoint.second.m_width, controlPoint.second.m_height);
        }

        delete gc;
    }
}

// MARK: マウスイベント

void NesoraPianoRollCanvas::OnMouseMove(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        event.Skip();
        return;
    }

    int oldHoverNoteIdx = hoverNoteIdx;
    wxPoint2DDouble mousePos = GetMousePos(event);
    if (IsPointMoved(startMousePos, mousePos)) {
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::AddNote) {
            mouseDragState = NesoraPianoRollCanvasMouseDragState::ResizingNote;
            
            // 既存のノートの中で、今回の開始地点の直前にあるもののリスト
            double prevEnd = 0.0;
            for (const auto& n : notes) {
                double endX = n.rect.m_x + n.rect.m_width;
                if (endX <= startMousePos.m_x && endX > prevEnd) {
                    prevEnd = endX;
                }
            }

            MidiNoteBox newNote;
            newNote.id = notes.empty() ? 1 : notes.back().id + 1;
            // 直前のノートの末尾、もしくは0から開始するように修正
            newNote.rect = wxRect2DDouble(prevEnd, std::floor(startMousePos.m_y / NESORA_MIDI_PANEL_NOTE_HEIGHT) * NESORA_MIDI_PANEL_NOTE_HEIGHT, 0, NESORA_MIDI_PANEL_NOTE_HEIGHT);
            newNote.startRectBuffer = newNote.rect;
            newNote.isSelected = true;
            notes.push_back(CreateNewMidiNoteBox(newNote));
        }
    }

    // ドラッグしている場合の処理
    if (event.Dragging() && HasCapture()) {
        // ノートをドラッグしていたら
        switch (mouseDragState) {
        case NesoraPianoRollCanvasMouseDragState::DraggingNotes: {
            double dx = mousePos.m_x - startMousePos.m_x;
            double dy = mousePos.m_y - startMousePos.m_y;

            for (auto& note : notes) {
                if (note.isSelected) {
                    note.rect.m_x = note.startRectBuffer.m_x + dx;
                    note.rect.m_y = note.startRectBuffer.m_y + dy;

                    // クオンタイズ
                    note.rect.m_x = std::floor(note.rect.m_x / 16.0) * 16.0;
                    note.rect.m_y = std::floor(note.rect.m_y / NESORA_MIDI_PANEL_NOTE_HEIGHT) * NESORA_MIDI_PANEL_NOTE_HEIGHT;
                }
            }
            tookAction = true;
        }
        break;
        // 矩形選択をしていたら
        case NesoraPianoRollCanvasMouseDragState::SelectingRange: {
            // 範囲矩形の更新
            double x = std::min(startMousePos.m_x, mousePos.m_x);
            double y = std::min(startMousePos.m_y, mousePos.m_y);
            double w = std::abs(startMousePos.m_x - mousePos.m_x);
            double h = std::abs(startMousePos.m_y - mousePos.m_y);
            currentSelectionRect = wxRect2DDouble(x, y, w, h);
            
            // リアルタイムで選択状態を更新する場合
            for (auto& note : notes) {
                if (currentSelectionRect.Intersects(note.rect)) {
                    note.isSelected = true;
                }
            }
        }
        break;
        // ノートを伸縮していたら
        case NesoraPianoRollCanvasMouseDragState::ResizingNote: {
            for (auto& note : notes) {
                if (note.isSelected) {
                    note.rect.m_width = note.startRectBuffer.m_x + std::floor((mousePos.m_x - startMousePos.m_x) / NESORA_MIDI_PANEL_QUANTIME_WIDTH) * NESORA_MIDI_PANEL_QUANTIME_WIDTH - note.startRectBuffer.m_x + note.startRectBuffer.m_width;
                    if (note.rect.m_width < 0) {
                        note.rect.m_width *= -1;
                        note.rect.m_x = note.startRectBuffer.m_x - note.rect.m_width;
                    }
                }
            }
            tookAction = true;
        }
        break;
        // コントロールポイントを編集していたら
        case NesoraPianoRollCanvasMouseDragState::ControlPointDragging: {
            if (selectedNoteIdx != -1 and draggingControlPointIdx != MidiNoteBoxControlPointID::None) {
                notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x = mousePos.m_x - notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0;
                notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y = mousePos.m_y - notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0;
            }
            PitchLineUpdate();
        }
        break;
        default: {
            // 特に何もなければ何もしない
        }
        }
    }
    // ドラッグしていない場合の処理
    else {
        // ホバーの処理
        wxPoint2DDouble mousePos = GetMousePos(event);
        bool onControlPoint = false;
        draggingControlPointIdx = MidiNoteBoxControlPointID::None;
        if (selectedNoteIdx != -1) {
            for (const auto& controlPoint : notes[selectedNoteIdx].controlPoints) {
                if (controlPoint.second.Contains(mousePos)) {
                    draggingControlPointIdx = controlPoint.first;
                    onControlPoint = true;
                    break;
                }
            }
        }
        hoverNoteIdx = -1;
        if (!onControlPoint) {
            bool onHandle = false;
            for (size_t i = 0;i < notes.size();i++) {
                if (notes[i].rect.Contains(mousePos)) {
                    hoverNoteIdx = i;
                    if (GetRightResizeHandleRect(notes[i]).Contains(mousePos) or GetLeftResizeHandleRect(notes[i]).Contains(mousePos))
                        onHandle = true;
                    break;
                }
            }
            if (onHandle) {
                SetCursor(wxCursor(wxCURSOR_SIZEWE)); // 左右矢印カーソル
            } else {
                SetCursor(wxNullCursor); // 通常カーソル
            }
        }
    }

    if ((event.Dragging() && HasCapture()) || oldHoverNoteIdx != hoverNoteIdx) {
        // if (isNotePreview) いつか作る
        //     ResolveOverlaps();
        Refresh(false);
    }

    event.Skip();
}

// MARK: 左クリック押し込み
void NesoraPianoRollCanvas::OnLeftDown(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    wxPoint mousePos = event.GetPosition();
    // 論理座標（スクロール位置を考慮した座標）を取得
    CalcUnscrolledPosition(mousePos.x, mousePos.y, &mousePos.x, &mousePos.y);
    bool hitAnyNote = false;
    startMousePos = mousePos;

    if(hoverNoteIdx == -1) {
        if (draggingControlPointIdx == MidiNoteBoxControlPointID::None) {
            // 範囲選択開始
            if (tookAction) {
                NoteSelectClear();
                tookAction = false;
            }
            mouseDragState = NesoraPianoRollCanvasMouseDragState::SelectingRange;
            currentSelectionRect = wxRect2DDouble(mousePos.x, mousePos.y, 0, 0);
        } else {
            mouseDragState = NesoraPianoRollCanvasMouseDragState::ControlPointDragging;
        }
    } else {
        if (notes[hoverNoteIdx].rect.Contains(mousePos)) {
            // Shiftが押されていなければ、他の選択を解除
            if (!event.ShiftDown() && !notes[hoverNoteIdx].isSelected) {
                NoteSelectClear();
            } else {
                for (auto& note : notes)
                    note.startRectBuffer = note.rect;
            }
            notes[hoverNoteIdx].isSelected = true;
            if (GetRightResizeHandleRect(notes[hoverNoteIdx]).Contains(mousePos)) {
                mouseDragState = NesoraPianoRollCanvasMouseDragState::ResizingNote;
            }
            else if (GetLeftResizeHandleRect(notes[hoverNoteIdx]).Contains(mousePos)) {
                notes[hoverNoteIdx].rect.m_x += notes[hoverNoteIdx].rect.m_width; // 左端をドラッグする場合、基準点を右端に変更
                notes[hoverNoteIdx].rect.m_width = -notes[hoverNoteIdx].rect.m_width;
                mouseDragState = NesoraPianoRollCanvasMouseDragState::ResizingNote;
            }
            else {
                mouseDragState = NesoraPianoRollCanvasMouseDragState::DraggingNotes;
            }
        }
        notes[hoverNoteIdx].startRectBuffer = notes[hoverNoteIdx].rect; // ドラッグ開始前の四角を保存
    }

    CaptureMouse();
    wxWindow::Refresh();
    event.Skip();
}

// MARK: 左ダブルクリック
void NesoraPianoRollCanvas::OnLeftDClick(wxMouseEvent& event) {
    wxPoint2DDouble mousePos = GetMousePos(event);

    int hitNoteIdx = -1;
    for (size_t i = 0; i < notes.size(); i++) {
        if (notes[i].rect.Contains(mousePos)) {
            hitNoteIdx = (int)(i);
            break;
        }
    }

    if (hitNoteIdx != -1) {
        if (!notes[hitNoteIdx].isSelected) {
            NoteSelectClear();
            notes[hitNoteIdx].isSelected = true;
        }

        mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
        ignoreNextLeftUp = true;
        if (HasCapture()) {
            ReleaseMouse();
        }

        BeginLyricEdit(hitNoteIdx);
        Refresh(false);
    } else {
        EndLyricEdit(true);
    }

    event.Skip(false);
}

// MARK: 左クリック離す
void NesoraPianoRollCanvas::OnLeftUp(wxMouseEvent& event) {
    if (ignoreNextLeftUp) {
        ignoreNextLeftUp = false;
        event.Skip();
        return;
    }

    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    wxPoint2DDouble mousePos = GetMousePos(event);
    // ノートの長さがフレーズよりも短かったら削除
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [](MidiNoteBox value) {
            return std::abs(value.rect.m_width) < NESORA_MIDI_PANEL_QUANTIME_WIDTH;
            }),
        notes.end()
    );
    if (IsPointMoved(startMousePos, mousePos)) {
        // ドラッグかリサイズをしていたらノートを整列させる
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes || mouseDragState == NesoraPianoRollCanvasMouseDragState::ResizingNote)
            ResolveOverlaps();
        else if (mouseDragState == NesoraPianoRollCanvasMouseDragState::ControlPointDragging) {
            PitchLineUpdate();
        }
    } else {
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes) {
            // ホバーしている時にポインタを動かさずにマウスボタンを離したら一つだけ選択するようにする
            if (!event.ShiftDown()) {
                NoteSelectClear();
            }
            notes[hoverNoteIdx].isSelected = true;
        } else if (mouseDragState == NesoraPianoRollCanvasMouseDragState::SelectingRange) {
            // ホバーしていない時にポインタを動かさずにマウスボタンを離したら全ての選択を解除する
            if (!event.ShiftDown()) {
                NoteSelectClear();
            }
        }
    }
    mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    if (HasCapture()) {
        ReleaseMouse();
    }
    ChangeSelectNote();
    wxWindow::Refresh();
    event.Skip();
}

// MARK: 右クリック押し込み
void NesoraPianoRollCanvas::OnRightDown(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    wxPoint mousePos = event.GetPosition();
    // 論理座標（スクロール位置を考慮した座標）を取得
    CalcUnscrolledPosition(mousePos.x, mousePos.y, &mousePos.x, &mousePos.y);
    bool hitAnyNote = false;
    startMousePos = mousePos;

    // ノートを追加
    if(hoverNoteIdx == -1) {
        NoteSelectClear();
        mouseDragState = NesoraPianoRollCanvasMouseDragState::AddNote;
    } else {
        notes.erase(notes.begin() + hoverNoteIdx);
        hoverNoteIdx = -1;
        ResolveOverlaps();
    }
    CaptureMouse();
    wxWindow::Refresh();
    event.Skip();
}

// MARK: 右クリック離す
void NesoraPianoRollCanvas::OnRightUp(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    // ノートの長さがフレーズよりも短かったら削除
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [](MidiNoteBox value) {
            return std::abs(value.rect.m_width) < NESORA_MIDI_PANEL_QUANTIME_WIDTH;
            }),
        notes.end()
    );
    if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes || mouseDragState == NesoraPianoRollCanvasMouseDragState::ResizingNote) {
        ResolveOverlaps();
    }
    mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    if (HasCapture()) {
        ReleaseMouse();
    }
    ChangeSelectNote();
    wxWindow::Refresh();
    event.Skip();
}

// MARK: キーボードイベント
void NesoraPianoRollCanvas::OnKeyDown(wxKeyEvent& event) {
    if (editingNoteIdx != -1) {
        event.Skip();
        return;
    }

    if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        std::vector<int> selectedOrder = GetSelectedNoteOrder();
        if (!selectedOrder.empty()) {
            BeginLyricEdit(selectedOrder.front());
            Refresh(false);
        }
        return;
    }

    if (event.GetKeyCode() == WXK_DELETE or event.GetKeyCode() == WXK_BACK) {
        // 選択されているノートを削除
        notes.erase(std::remove_if(notes.begin(), notes.end(), [](const MidiNoteBox& note) {
            return note.isSelected;
        }), notes.end());
        hoverNoteIdx = -1;
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == WXK_UP) {
        // 選択されているノートを半音上げる
        for (auto& note : notes) {
            if (note.isSelected) {
                note.rect.m_y -= NESORA_MIDI_PANEL_NOTE_HEIGHT;
            }
        }
        ResolveOverlaps();
        Refresh();
        return;
    }
    if (event.GetKeyCode() == WXK_DOWN) {
        // 選択されているノートを半音下げる
        for (auto& note : notes) {
            if (note.isSelected) {
                note.rect.m_y += NESORA_MIDI_PANEL_NOTE_HEIGHT;
            }
        }
        ResolveOverlaps();
        Refresh();
        return;
    }
    if (event.GetKeyCode() == WXK_LEFT) {
        // 選択されているノートを左に移動
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == WXK_RIGHT) {
        // 選択されているノートを右に移動
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == 'A' && event.ControlDown()) {
        // 全てのノートを選択
        for (auto& note : notes) {
            note.isSelected = true;
        }
        hoverNoteIdx = -1;
        Refresh();
    }
    if (event.GetKeyCode() == 'D' && event.ControlDown()) {
        // 選択されているノートを複製
        std::vector<MidiNoteBox> newNotes;
        for (const auto& note : notes) {
            if (note.isSelected) {
                MidiNoteBox newNote = note;
                newNote.id = notes.empty() ? 1 : notes.back().id + 1;
                newNote.rect.m_x += 16; // 少し右にずらして複製
                newNotes.push_back(newNote);
            }
        }
        notes.insert(notes.end(), newNotes.begin(), newNotes.end());
        ResolveOverlaps();
        Refresh();
    }

    event.Skip();
}

void NesoraPianoRollCanvas::OnSize(wxSizeEvent& event) {
    SetScrollWidth();
    event.Skip();
}

// MARK: 歌詞の編集関係

// 歌詞の編集を確定
void NesoraPianoRollCanvas::AdvanceLyricEdit(bool keepEditing) {
    if (editingNoteIdx < 0) {
        return;
    }

    const int nextNoteIdx = keepEditing ? GetNextSelectedNoteIndex(editingNoteIdx) : -1;
    EndLyricEdit(true);

    if (nextNoteIdx != -1) {
        BeginLyricEdit(nextNoteIdx);
    }
}

// 歌詞の編集を確定
void NesoraPianoRollCanvas::OnLyricEditorEnter(wxCommandEvent& event) {
    AdvanceLyricEdit(true);
    event.Skip(false);
}

// 歌詞の編集を確定してエディタを閉じる
void NesoraPianoRollCanvas::OnLyricEditorKillFocus(wxFocusEvent& event) {
    EndLyricEdit(true);
    event.Skip();
}

// 歌詞の編集中に特定のキーが押されたときの処理
void NesoraPianoRollCanvas::OnLyricEditorCharHook(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_ESCAPE) {
        EndLyricEdit(false);
        SetFocus();
        return;
    }
    if (event.GetKeyCode() == WXK_TAB || event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        AdvanceLyricEdit(true);
        return;
    }
    event.Skip();
}

void NesoraPianoRollCanvas::OnScroll(wxScrollWinEvent& event) {
    event.Skip(); // デフォルトのスクロール処理を実行

    static int lastScrollX = 0, lastScrollY = 0;
    int x, y;
    GetViewStart(&x, &y);
    int ppux, ppuy;
    GetScrollPixelsPerUnit(&ppux, &ppuy);

    // これしないとちょっとズレる
    int dx = x - lastScrollX;
    int dy = y - lastScrollY;

    if (m_linkedTimeline) m_linkedTimeline->SetScrollOffset((x + dx) * ppux);
    if (m_linkedKeys) m_linkedKeys->SetScrollOffset((y + dy) * ppuy);
    lastScrollX = x;
    lastScrollY = y;
}

double NesoraPianoRollCanvas::GetPitch(double t) {
    playbackTimeInSec.store(t);
    return midiScript.GetPitch(t);
}

void NesoraPianoRollCanvas::ClearPlaybackLine() {
    playbackTimeInSec.store(0.0);
    Refresh(false);
}

bool NesoraPianoRollCanvas::IsLyricEditing() const {
    return editingNoteIdx != -1;
}


// MARK:NesoraPianoKeys

void NesoraPianoKeys::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    wxSize size = GetClientSize();
    for (int i = 0; i < 128; ++i) {
        double y = i * NESORA_MIDI_PANEL_NOTE_HEIGHT - m_yOffset;
        
        // 画面外の描画をスキップ
        if (y + NESORA_MIDI_PANEL_NOTE_HEIGHT < 0 || y > size.GetHeight()) continue;

        int noteInOctave = (127 - i) % 12;
        bool isBlackKey = (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || noteInOctave == 8 || noteInOctave == 10);

        if (isBlackKey) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PIANOKEY_BLACK)));
        } else {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PIANOKEY_WHITE)));
        }
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->DrawRectangle(0, y, size.GetWidth(), NESORA_MIDI_PANEL_NOTE_HEIGHT);

        // Cの音のラベル
        if (noteInOctave == 0) {
            int octave = (127 - i) / 12 - 1;
            gc->SetFont(GetFont(), nsGetColor(nsColorType::PIANOKEY_BLACK));
            gc->DrawText(wxString::Format("C%d", octave), 5, y + 2);
        }
    }
    delete gc;
}

// MARK:NesoraTimeline

void NesoraTimeline::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    wxSize size = GetClientSize();
    gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    gc->SetFont(GetFont(), nsGetColor(nsColorType::ON_BACKGROUND));

    for (int x = 0; x < scriptLengthInBar * m_barWidth; x += m_barWidth) {
        double drawX = x - m_xOffset;
        if (drawX + m_barWidth < 0 || drawX > size.GetWidth()) continue;

        gc->StrokeLine(drawX, size.GetHeight() - 10, drawX, size.GetHeight());
        gc->DrawText(wxString::Format("%d", x / m_barWidth + 1), drawX + 2, 2);
    }
    delete gc;
}


// MARK:NesoraMIDIPanel

void NesoraMIDIPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* MIDIEditorSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI Editor"));

    // ピアノロール（中央）を先に作成して、リンクさせる
    pianoRoll = new NesoraPianoRollCanvas(MIDIEditorSizer->GetStaticBox());
    
    // タイムライン（上部）
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->AddSpacer(60); // ピアノ鍵盤の幅分のスペースを確保
    NesoraTimeline* timeline = new NesoraTimeline(MIDIEditorSizer->GetStaticBox());
    timeline->SetMinSize(wxSize(-1, 30));
    topSizer->Add(timeline, 1, wxEXPAND);

    wxBoxSizer* middleSizer = new wxBoxSizer(wxHORIZONTAL);

    // ピアノ鍵盤（左側）
    NesoraPianoKeys* keys = new NesoraPianoKeys(MIDIEditorSizer->GetStaticBox());
    keys->SetMinSize(wxSize(60, -1));
    middleSizer->Add(keys, 0, wxEXPAND);

    middleSizer->Add(pianoRoll, 1, wxEXPAND);
    MIDIEditorSizer->Add(topSizer, 0, wxEXPAND);
    MIDIEditorSizer->Add(middleSizer, 1, wxEXPAND);
    
    NesoraPhoneticalMIDINoteEditor* MIDINoteEditor = new NesoraPhoneticalMIDINoteEditor(this);
    MIDINoteEditor->Hide();
    
    mainSizer->Add(MIDIEditorSizer, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(MIDINoteEditor, 0, wxEXPAND | wxALL, 5);
    
    this->SetSizer(mainSizer);
    
    // 同期設定
    pianoRoll->SetLinkedPianoKeys(keys);
    pianoRoll->SetLinkedTimeline(timeline);
    pianoRoll->SetLinkedMIDINoteEditor(MIDINoteEditor);
    MIDINoteEditor->SetParentSizer(mainSizer);

    playbackLineTimer.SetOwner(this);
    Bind(wxEVT_TIMER, &NesoraMIDIPanel::OnPlaybackTimer, this, playbackLineTimer.GetId());
    playbackLineTimer.Start(16);
}

double NesoraMIDIPanel::GetPitch(double samplingFrequency) {
    const double nowPitch = pianoRoll->GetPitch(nowPlayTime);
    nowPlayTime += 1.0 / samplingFrequency;
    return nowPitch;
}

bool NesoraMIDIPanel::IsLyricEditing() const {
    return pianoRoll != nullptr and pianoRoll->IsLyricEditing();
}

void NesoraMIDIPanel::PlayStop() {
    nowPlayTime = 0.0;
    pianoRoll->ClearPlaybackLine();
    return;
}

void NesoraMIDIPanel::OnPlaybackTimer(wxTimerEvent& event) {
    if (pianoRoll) {
        pianoRoll->Refresh(false);
    }
    event.Skip();
}


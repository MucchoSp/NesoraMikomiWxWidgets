// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "NesoraScript.h"
#include <iostream>

#ifndef NESORA_MIDI_H
#define NESORA_MIDI_H

struct MidiNote {
    int pitch;      // 0-127 (MIDIノート番号)
    int position;   // 時間（X座標）
    int duration;   // 長さ
};

// MARK: NesoraMIDISplineScript


class NesoraSpline {
public:
    NesoraSpline() {}
    void AddPoint(double y);
    void SetDx(double dx);

    double GetValue(double x) const;        // 線形補間
    double GetCubicValue(double x) const;   // 3次スプライン補完
    std::vector<double> GetCubicValues(const double step) const; // 3次スプライン補完で、stepごとに値を取得
    void CalculateCoefficients();
private:
    std::vector<double> points;
    double dx = 0.0;
    std::vector<double> a, b, c, d;
};

class NesoraMIDISplineScript : public NesoraScriptBase {
public:
    NesoraMIDISplineScript() {}
    NesoraMIDISplineScript(const std::vector<NesoraMidiNote>& notes) : notes(notes) {}

    double DeltaRadian(double t) override;
    double Volume(double t) override;
    ParametricNesoraParameter Vowel(double t) override;

    void SetNotes(const std::vector<NesoraMidiNote>& in_notes);
    const std::vector<NesoraMidiNote>& GetNotes() const;
    std::vector<NesoraMidiNote>& GetNotes();

    void CalculateNoteParam(); // ピッチラインの計算とエンベロープの計算
    std::vector<double> GetPitchLine() const { return pitchLine; }
    std::vector<double> GetEnvelope() const { return envelope; }
    std::vector<double> GetPitchLinePerSample(double sampleRate) const;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;
private:
    std::vector<NesoraMidiNote> notes;
    std::vector<double> pitchLine;
    std::vector<double> envelope;
    int splineResolution = 2205; // スプラインの解像度（サンプル数）
};



// MARK: NesoraMIDIPhoneticalScript

class NesoraMIDIPhoneticalScript : public NesoraScriptBase {
public:
    NesoraMIDIPhoneticalScript() {}
    NesoraMIDIPhoneticalScript(const std::vector<NesoraMidiNotePhoneticalInfo>& notes) : notes(notes) {}

    double DeltaRadian(double t) override;
    double Volume(double t) override;
    ParametricNesoraParameter Vowel(double t) override;

    void SetNotes(const std::vector<NesoraMidiNotePhoneticalInfo>& in_notes);
    const std::vector<NesoraMidiNotePhoneticalInfo>& GetNotes() const;
    std::vector<NesoraMidiNotePhoneticalInfo>& GetNotes();

    void CalculateNoteParam(double sampleRate); // ピッチラインの計算とエンベロープの計算
    std::vector<double> GetPitchLine() const { return pitchLine; }
    std::vector<double> GetEnvelope() const { return envelope; }

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;
private:

    std::vector<NesoraMidiNotePhoneticalInfo> notes;
    std::vector<double> pitchLine;
    std::vector<double> envelope;

    double A4Pitch = 440.0; // A4の周波数
};


#endif // NESORA_MIDI_H
// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraMIDI.h"

// MARK: NesoraMIDISplineScript

void NesoraSpline::AddPoint(double y) {
    points.push_back(y);
}

void NesoraSpline::SetDx(double dx) {
    this->dx = dx;
}

void NesoraSpline::CalculateCoefficients() {
    size_t n = points.size();
    a = points;
    b.resize(n - 1);
    c.resize(n);
    d.resize(n - 1);

    std::vector<double> h(n - 1);
    for (size_t i = 0; i < n - 1; ++i) {
        h[i] = dx;
    }

    std::vector<double> alpha(n - 1);
    for (size_t i = 1; i < n - 1; ++i) {
        alpha[i] = (3 / h[i]) * (a[i + 1] - a[i]) - (3 / h[i - 1]) * (a[i] - a[i - 1]);
    }

    std::vector<double> l(n), mu(n), z(n);
    l[0] = 1.0;
    mu[0] = z[0] = 0.0;

    for (size_t i = 1; i < n - 1; ++i) {
        l[i] = 2 * (h[i - 1] + h[i]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    l[n - 1] = 1.0;
    z[n - 1] = c[n - 1] = 0.0;

    for (size_t j = n - 2; j < n; --j) {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] = (a[j + 1] - a[j]) / h[j] - h[j] * (c[j + 1] + 2 * c[j]) / 3;
        d[j] = (c[j + 1] - c[j]) / (3 * h[j]);
    }
}

double NesoraSpline::GetValue(double x) const {
    if (points.empty()) return 0.0;
    if (x <= 0) return points.front();
    if (x >= static_cast<double>(points.size() - 1) * dx) return points.back();

    size_t i = static_cast<size_t>(x / dx);
    double t = (x - i * dx) / dx;

    return (1 - t) * points[i] + t * points[i + 1];
    return 0.0;
}

double NesoraSpline::GetCubicValue(double x) const {
    if (points.size() < 4) return GetValue(x); // 点が4つ未満なら線形補間

    for (size_t i = 1; i < points.size() - 2; ++i) {
        if (x >= i * dx && x <= (i + 1) * dx) {
            double t = (x - i * dx) / dx;
            double p0 = points[i - 1];
            double p1 = points[i];
            double p2 = points[i + 1];
            double p3 = points[i + 2];

            // Catmull-Romスプラインの式
            return 0.5 * ((2 * p1) +
                            (-p0 + p2) * t +
                            (2*p0 - 5*p1 + 4*p2 - p3) * t*t +
                            (-p0 + 3*p1 - 3*p2 + p3) * t*t*t);
        }
    }
    return GetValue(x); // 範囲外は線形補間
}

std::vector<double> NesoraSpline::GetCubicValues(const double step) const {
    std::vector<double> values;
    
    for (double x = 0; x <= (points.size() - 1) * dx; x += step) {
        int i = static_cast<int>(x / dx);
        if (i < 1) i = 1;
        if (i > static_cast<int>(points.size() - 3)) i = static_cast<int>(points.size() - 3);
        values.push_back(a[i] + (b[i] + (c[i] + d[i] * (x - i * dx)) * (x - i * dx)) * (x - i * dx));
    }
    return values;
}






double NesoraMIDISplineScript::DeltaRadian(double t) {
    return 0.0;
}

double NesoraMIDISplineScript::Volume(double t) {
    return 0.0;
}

ParametricNesoraParameter NesoraMIDISplineScript::Vowel(double t) {
    return ParametricNesoraParameter();
}

void NesoraMIDISplineScript::SetNotes(const std::vector<NesoraMidiNote>& in_notes) {
    notes = in_notes;
}

void NesoraMIDISplineScript::CalculateNoteParam() {
    NesoraSpline pitchSpline;
    NesoraSpline envelopeSpline;
    pitchSpline.SetDx(splineResolution);
    envelopeSpline.SetDx(splineResolution);

    
    double remainder = 0.0;
    for (const auto& note : notes) {
        double length = note.length * samplingFrequency / 1000.0;
        remainder += fmod(length, splineResolution);
        if (remainder >= splineResolution) {
            length += splineResolution;
            remainder -= splineResolution;
        }
        for (size_t i = 0; i < static_cast<size_t>(length / splineResolution); i++) {
            pitchSpline.AddPoint(note.pitch);
            envelopeSpline.AddPoint(note.intensity);
        }
    }

    pitchLine.clear();
    envelope.clear();
    for (auto& note : notes) {
        double length = note.length * samplingFrequency / 1000.0;
        for (size_t i = 0; i < static_cast<size_t>(length); i++) {
            pitchLine.push_back(pitchSpline.GetCubicValue(i));
            envelope.push_back(envelopeSpline.GetCubicValue(i));
        }
    }
}

std::vector<double> NesoraMIDISplineScript::GetPitchLinePerSample(double sampleRate) const {
    std::vector<double> perSamplePitchLine;
    NesoraSpline pitchSpline;
    pitchSpline.SetDx(splineResolution); // サンプルごとに点を追加

    double remainder = 0.0;
    double NotesLength = 0.0;
    for (const auto& note : notes) {
        double length = note.length * samplingFrequency / 1000.0;
        remainder += fmod(length, splineResolution);
        if (remainder >= splineResolution) {
            length += splineResolution;
            remainder -= splineResolution;
        }
        for (size_t i = 0; i < static_cast<size_t>(length / splineResolution); i++) {
            pitchSpline.AddPoint(note.pitch);
        }
        NotesLength += length;
    }

    // for (size_t i = 0; i < static_cast<size_t>(NotesLength); i+=sampleRate) {
    //     perSamplePitchLine.push_back(pitchSpline.GetCubicValue(i));
    // }
    pitchSpline.CalculateCoefficients();
    perSamplePitchLine = pitchSpline.GetCubicValues(sampleRate);
    return perSamplePitchLine;
}

const std::vector<NesoraMidiNote>& NesoraMIDISplineScript::GetNotes() const {
    return notes;
}

std::vector<NesoraMidiNote>& NesoraMIDISplineScript::GetNotes() {
    return notes;
}

std::vector<unsigned char> NesoraMIDISplineScript::SaveData() {
    return std::vector<unsigned char>();
}

void NesoraMIDISplineScript::LoadData(const std::vector<unsigned char>& data) {
}






// ピッチカーブの計算
double CalculatePitchLineValue(NesoraPitchCurveType curveType, double t) {
    switch (curveType) {
        case NesoraPitchCurveType::LINEAR:
            return t;
        case NesoraPitchCurveType::SINE:
            return 0.5 * (1.0 - cos(t * M_PI)); // 0から1への正弦カーブ
        case NesoraPitchCurveType::SIGMOID:
            return 1.0 / (1.0 + exp(-10.0 * (t - 0.5))); // シグモイド関数
        default:
            return t;
    }
}

// MARK: NesoraMIDIPhoneticalScript

double NesoraMIDIPhoneticalScript::DeltaRadian(double t) {
    return 0.0;
}

double NesoraMIDIPhoneticalScript::Volume(double t) {
    return 0.0;
}

ParametricNesoraParameter NesoraMIDIPhoneticalScript::Vowel(double t) {
    return ParametricNesoraParameter();
}

void NesoraMIDIPhoneticalScript::SetNotes(const std::vector<NesoraMidiNotePhoneticalInfo>& in_notes) {
    notes = in_notes;
}

const std::vector<NesoraMidiNotePhoneticalInfo>& NesoraMIDIPhoneticalScript::GetNotes() const {
    return notes;
}

std::vector<NesoraMidiNotePhoneticalInfo>& NesoraMIDIPhoneticalScript::GetNotes() {
    return notes;
}

void NesoraMIDIPhoneticalScript::CalculateNoteParam(double sampleRate) {
    pitchLine.clear();
    envelope.clear();

    double currentTime = 0.0; // 現在の時間(ms)
    for (size_t i = 0; i < notes.size(); i++) {
        const auto& note = notes[i];
        if (i > 0) {
            // ピッチカーブを計算
            const auto& prevNote = notes[i - 1];
            for (size_t j = 0; j < (size_t)(note.frontPitchMoveTime * sampleRate / 1000.0); j++) {
                double t = (double)j / (note.frontPitchMoveTime * sampleRate / 1000.0);
                double curveValue = CalculatePitchLineValue(note.frontPitchMoveCurve, t);
                pitchLine.push_back(prevNote.pitch + curveValue * (note.pitch - prevNote.pitch));
            }
        } else {
            for (size_t j = 0; j < (size_t)((note.frontPitchMoveTime - note.frontPitchMoveTimming) * sampleRate / 1000.0); j++) {
                pitchLine.push_back(note.pitch);
            }
        }
        for (size_t j = 0; j < (size_t)std::min((note.modulationStartTime - (note.frontPitchMoveTime - note.frontPitchMoveTimming)) * sampleRate / 1000.0, (double)(note.length * sampleRate / 1000.0)); j++) {
            pitchLine.push_back(note.pitch);
        }
        double nextNotePitchCurveStartTimming = (i < notes.size() - 1) ? (note.length - notes[i + 1].frontPitchMoveTimming) : note.length;
        for (size_t j = 0; j < (size_t)((nextNotePitchCurveStartTimming - note.modulationStartTime) * sampleRate / 1000.0); j++) {
            double modulationStrength = note.pitch * (1.0 - std::pow(2.0, note.modulationStrength / 1200.0 / 2.0)); // centを周波数に変換
            double modulationValue = modulationStrength * sin(2.0 * M_PI * note.modulationFrequency * j / sampleRate);
            pitchLine.push_back(note.pitch + modulationValue);
        }

        for (size_t j = 0; j < (size_t)(note.length * sampleRate / 1000.0); j++) {
            envelope.push_back(note.intensity);
        }


        currentTime += note.length;
        if (std::abs((int)pitchLine.size() - (((currentTime - note.frontPitchMoveTimming) + 1) * sampleRate / 1000.0)) > 1) {
            pitchLine.resize((size_t)((currentTime - note.frontPitchMoveTimming) * sampleRate / 1000.0));
        }
        if (std::abs((int)envelope.size() - ((currentTime + 1) * sampleRate / 1000.0)) > 1) {
            envelope.resize((size_t)(currentTime * sampleRate / 1000.0));
        }
    }
}

std::vector<unsigned char> NesoraMIDIPhoneticalScript::SaveData() {
    // TODO: いつかやる
    return std::vector<unsigned char>();
}

void NesoraMIDIPhoneticalScript::LoadData(const std::vector<unsigned char>& data) {
    // TODO: いつかやる
}


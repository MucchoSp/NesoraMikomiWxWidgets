// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma ones

#include <map>

#ifndef PARAMETRIC_NESORA_DEFINES
#define PARAMETRIC_NESORA_DEFINES

struct ParametricNesoraIIRFilterParameter {
    double delta_r;
    double delta_theta;
};

struct ParametricNesoraRosenbergWaveParameter {
    double delta_tau1;
    double delta_tau2;
    double delta_noise;
    double delta_a0;
};

struct ParametricNesoraParameter {
    std::vector<ParametricNesoraIIRFilterParameter> filter;
    std::vector<ParametricNesoraRosenbergWaveParameter> source;
    double a0;
};

struct NesoraMidiNote {
    double pitch;       // 0-127 (MIDIノート番号)
    double intensity;   // 強弱（0.0-1.0）
    double length;      // 長さ(ms)
    double modulation;  // モジュレーション(ピッチの揺れ)
    std::string lyric;  // 歌詞

    std::vector<std::pair<uint32_t, double>> parameterDeltas;   // パラメータの変化（IDと変化量のペア）
    std::vector<double> pitchLine;      // ピッチラインの値のリスト
    std::vector<double> envelope;       // エンベロープのリスト
};

#endif //PARAMETRIC_NESORA_DEFINES
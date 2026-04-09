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

    // std::vector<std::pair<uint32_t, double>> parameterDeltas;   // パラメータの変化（IDと変化量のペア）
    // std::vector<double> pitchLine;      // ピッチラインの値のリスト
    // std::vector<double> envelope;       // エンベロープのリスト
};

enum class NesoraPitchCurveType {
    LINEAR,     // 直線
    SINE,       // 曲線
    SIGMOID     // シグモイド曲線
};

struct NesoraMidiNotePhoneticalInfo {
    double pitch;       // 0-127 (MIDIノート番号)
    double intensity;   // 強弱（0.0-1.0）
    double length;      // 長さ(ms)
    std::string phoneticSymbol;     // 発音記号
    std::string lyric;  // 歌詞

    double frontOverlap;            // 前の音との重なり時間(ms)
    double backOverlap;             // 後の音との重なり時間(ms)

    double strength;                // 強さ(0.0-1.0)

    double frontTransitionTime;     // 前の音からの移行時間(ms)
    double backTransitionTime;      // 後の音への移行時間(ms)

    double cl_length;               // 破裂音の閉鎖区間の長さ(ms)
    double uv_length;               // 声帯振動終了後に母音フォルマントが続く時間(ms)
    double sv_length;               // 母音終了後に声帯振動が続いている時間(ms)

    double frontPitchMoveTime;      // ピッチが変わる時間(ms)
    double frontPitchMoveTimming;   // ピッチが変わるタイミング(ms)
    NesoraPitchCurveType frontPitchMoveCurve;      // ピッチが変わるときのカーブの種類

    double modulationStrength;      // モジュレーションの強さ(cent)
    double modulationFrequency;     // モジュレーションの周波数(Hz)
    double modulationStartTime;     // モジュレーションの開始時間(ms)
    double modulationFadeInTime;    // モジュレーションのフェードイン時間(ms)
    double modulationFadeOutTime;   // モジュレーションのフェードアウト時間(ms)

    double overshootTime;           // オーバーシュートの時間(ms)
    double overshootPitch;          // オーバーシュートのピッチ(cent)

    double preparationTime;         // プレパレーション時間(ms)
    double preparationPitch;        // プレパレーションのピッチ(cent)
};




#endif //PARAMETRIC_NESORA_DEFINES
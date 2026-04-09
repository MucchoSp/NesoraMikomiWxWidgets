// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

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







// MARK: SaveData関数群

inline std::vector<unsigned char> SaveData(const ParametricNesoraIIRFilterParameter& param) {
    std::vector<unsigned char> data;
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&param.delta_r), reinterpret_cast<const unsigned char*>(&param.delta_r) + sizeof(param.delta_r));
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&param.delta_theta), reinterpret_cast<const unsigned char*>(&param.delta_theta) + sizeof(param.delta_theta));
    return data;
}

inline std::vector<unsigned char> SaveData(const ParametricNesoraRosenbergWaveParameter& param) {
    std::vector<unsigned char> data;
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&param.delta_tau1), reinterpret_cast<const unsigned char*>(&param.delta_tau1) + sizeof(param.delta_tau1));
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&param.delta_tau2), reinterpret_cast<const unsigned char*>(&param.delta_tau2) + sizeof(param.delta_tau2));
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&param.delta_noise), reinterpret_cast<const unsigned char*>(&param.delta_noise) + sizeof(param.delta_noise));
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&param.delta_a0), reinterpret_cast<const unsigned char*>(&param.delta_a0) + sizeof(param.delta_a0));
    return data;
}

inline std::vector<unsigned char> SaveData(const ParametricNesoraParameter& param) {
    std::vector<unsigned char> data;
    size_t filterSize = param.filter.size();
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&filterSize), reinterpret_cast<const unsigned char*>(&filterSize) + sizeof(size_t));
    for(const auto& filterParam : param.filter) {
        std::vector<unsigned char> filterData = SaveData(filterParam);
        size_t filterDataSize = filterData.size();
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&filterDataSize), reinterpret_cast<const unsigned char*>(&filterDataSize) + sizeof(size_t));
        data.insert(data.end(), filterData.begin(), filterData.end());
    }
    size_t sourceSize = param.source.size();
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&sourceSize), reinterpret_cast<const unsigned char*>(&sourceSize) + sizeof(size_t));
    for(const auto& sourceParam : param.source) {
        std::vector<unsigned char> sourceData = SaveData(sourceParam);
        size_t sourceDataSize = sourceData.size();
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&sourceDataSize), reinterpret_cast<const unsigned char*>(&sourceDataSize) + sizeof(size_t));
        data.insert(data.end(), sourceData.begin(), sourceData.end());
    }
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&param.a0), reinterpret_cast<const unsigned char*>(&param.a0) + sizeof(param.a0));
    return data;
}

inline std::vector<unsigned char> SaveData(const std::map<uint32_t, ParametricNesoraParameter>& parameters) {
    std::vector<unsigned char> data;
    size_t paramSize = parameters.size();
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&paramSize), reinterpret_cast<const unsigned char*>(&paramSize) + sizeof(size_t));
    for(const auto& [paramID, param] : parameters) {
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&paramID), reinterpret_cast<const unsigned char*>(&paramID) + sizeof(uint32_t));
        std::vector<unsigned char> paramData = SaveData(param);
        size_t paramDataSize = paramData.size();
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&paramDataSize), reinterpret_cast<const unsigned char*>(&paramDataSize) + sizeof(size_t));
        data.insert(data.end(), paramData.begin(), paramData.end());
    }
    return data;
}

#endif //PARAMETRIC_NESORA_DEFINES
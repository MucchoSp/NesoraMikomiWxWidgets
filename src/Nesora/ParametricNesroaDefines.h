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

    std::vector<unsigned char> SaveData() const {
        std::vector<unsigned char> data;
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&delta_r), reinterpret_cast<const unsigned char*>(&delta_r) + sizeof(delta_r));
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&delta_theta), reinterpret_cast<const unsigned char*>(&delta_theta) + sizeof(delta_theta));
        return data;
    }
    void LoadData(const std::vector<unsigned char>& data) {
        if (data.size() < sizeof(delta_r) + sizeof(delta_theta)) {
            // データが不足している場合の処理
            return;
        }
        size_t offset = 0;
        std::memcpy(&delta_r, &data[offset], sizeof(delta_r));
        offset += sizeof(delta_r);
        std::memcpy(&delta_theta, &data[offset], sizeof(delta_theta));
        offset += sizeof(delta_theta);
    }
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

struct ParametricNesoraDeltaValue{
    uint32_t ID;
    double delta;
    ParametricNesoraDeltaValue(uint32_t in_ID, double in_delta) : ID(in_ID), delta(in_delta) {}
};

typedef std::vector<ParametricNesoraDeltaValue> ParametricNesoraDelta;



//MARK: MIDI

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





// MARK: 辞書

struct ParametricNesoraDictionalyWordDeltaAndTime {
    double parameterDeltas;                 // パラメータの変化（IDと変化量のペア）
    double time;                            // 変化が起こるタイミング(ms)
};

struct ParametricNesoraDictionalyEnvelopePoint {
    double time;   // 時間(ms)
    double value;  // 値
};

struct ParametricNesoraDictionalyWord {
    std::string word;
    std::string symbol;
    std::map<uint32_t, std::vector<ParametricNesoraDictionalyWordDeltaAndTime>> parameterDeltas;   // パラメータの変化
    std::vector<ParametricNesoraDictionalyEnvelopePoint> envelope;             // エンベロープのリスト


    /*

        -------fixedTime-------><--(dynamic)--><------blankTime-------
    ----0-----------------s-----+-------------+-----------------------+----
        |           c     |     |      a      |          lc           |
    ----0-----------------s-----+-------------+-----------------------+----
        <---overlapTime---                          <---fadeoutTime---

    それぞれの変数の役割
    overlapTimeは、基準からどのくらい先んじるかの時間
    fixedTimeは、前方の、発声が固定されている時間
    dynamicは長さが可変の時間
    blankTimeは、後方の、発声が固定されている時間
    fadeoutTimeはフェードアウト時間

    blankTimeの発音は後続の音素によって異なるのでちょっと扱いめんどいね
    場合によっては、後続がある場合はblankTime部分を消し飛ばして後続のfixedTimeで処理させるのも手か
    */

    double overlapTime;     // 前の単語との重なり時間(ms)
    double fixedTime;       // 固定時間長(ms)
    double blankTime;       // 終了時間長(ms)
    double fadeoutTime;     // フェードアウト時間(ms)
};

typedef std::map<std::string, ParametricNesoraDictionalyWord> ParametricNesoraDictionary;







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
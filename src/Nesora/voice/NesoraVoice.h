// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <iostream>

#include "../Nesora.h"

#ifndef NESORA_VOICE_H
#define NESORA_VOICE_H


class NesoraVoice {
public:
    NesoraVoice() {}
    ~NesoraVoice() {}
private:
};

class NesoraMikomiVoice : public NesoraVoice {
public:
    NesoraMikomiVoice() {}
    NesoraMikomiVoice(NesoraSourceBase* src, NesoraFilterBase* flt) : source(src), filter(flt) {}
    ~NesoraMikomiVoice() {}

    void SetSource(NesoraSourceBase* src);
    void SetFilter(NesoraFilterBase* flt);
    void SetDictionaly(NesoraDictionalyBase* dic);
    NesoraSourceBase* GetSource() const { return source; }
    NesoraFilterBase* GetFilter() const { return filter; }
    NesoraDictionalyBase* GetDictionaly() const { return dictionaly; }

    // リアルタイム合成
    double Synthesize(double radian);
    double Synthesize(double frequency, double samplingFrequency);
    double GetRadian() const { return radian; }
    void SetRadian(double r) { radian = r; }
    
    void UpdateParameters(ParametricNesoraParameterValue* parameters);
    ParametricNesoraParameterValue* GetCurrentParameters() const { return currentParameters; }

    // スクリプト合成
    void SetScript(NesoraScriptBase* scr);
    NesoraScriptBase* GetScript() const { return script; }
    void RefreshScript();
    double SynthesizeScript();
    void SetSynthesizeScriptIndex(size_t idx);

    void CacheScriptWave();
    std::vector<double> GetScriptWave();
    double GetScriptWave(size_t idx);

    // ファイル操作
    void SaveVoiceData(const std::string& filename);
    void LoadVoiceData(const std::string& filename);

    std::vector<unsigned char> GetVoiceData();
    void LoadVoiceData(const std::vector<unsigned char>& fileData);

private:

    NesoraSourceBase* source;
    NesoraFilterBase* filter;

    NesoraDictionalyBase* dictionaly;
    NesoraScriptBase* script;
    size_t script_idx;
    std::vector<double> currentScriptWave;

    double radian = 0.0;
    ParametricNesoraParameterValue* currentParameters;

};

#endif // NESORA_VOICE_H
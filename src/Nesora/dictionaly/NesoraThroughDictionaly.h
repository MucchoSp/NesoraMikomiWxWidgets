// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
# pragma once

#include "NesoraDictionaly.h"

#ifndef NESORA_THROUGH_DICTIONALY_H
#define NESORA_THROUGH_DICTIONALY_H

class NesoraThroughDictionaly : public NesoraDictionalyBase {
public:
    NesoraThroughDictionaly(){}

    // パラメーターは空で、そのほかはスクリプトからそのまま流す
    int GetWord(int idx, ParametricNesoraParameterValue* outputParameters, double* outputPitch, double* outputEnvelope) override {
        *outputParameters = ParametricNesoraParameterValue();
        *outputPitch = outputPitchList[idx];
        *outputEnvelope = outputEnvelopeList[idx];
        return 0;
    }

    // スクリプトを設定
    int SetScript(NesoraScriptBase* script) override {
        this->script = script;
        std::cout << "script->GetSamplingFrequency() = " << script->GetSamplingFrequency() <<std::endl;
        return 0;
    }
    // 現在のスクリプトを取得
    NesoraScriptBase* GetScript() override {
        return script;
    }
    // キャッシュを現在のスクリプトで更新する
    int RefreshCache() override {
        outputPitchList.clear();
        int listSize = 0;
        double samplingFrequency = script->GetSamplingFrequency();
        while(1) {
            double pitch = script->GetPitch((double)listSize / samplingFrequency);
            if(pitch == 0.0)break;
            outputPitchList.push_back(script->GetPitch((double)listSize / samplingFrequency));
            listSize++;
        }

        outputEnvelopeList.resize(listSize);
        for(int i = 0;i < listSize;i++) {
            outputEnvelopeList[i] = script->GetEnvelope((double)i / samplingFrequency);
        }

        return 0;
    }

    // データがないので空のベクターを返す
    std::vector<unsigned char> SaveData() override {
        return std::vector<unsigned char>();
    }
    // データがないので特に何もしない
    void LoadData(const std::vector<unsigned char>& data) override {
    }

private:
    std::vector<double> outputPitchList;
    std::vector<double> outputEnvelopeList;
};

#endif // NESORA_THROUGH_DICTIONALY_H

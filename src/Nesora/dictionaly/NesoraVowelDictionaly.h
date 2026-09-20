// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <list>

#include "NesoraDictionaly.h"

#ifndef NESORA_VOWEL_DICTIONALY_H
#define NESORA_VOWEL_DICTIONALY_H

class NesoraVowelDictionaly : public NesoraDictionalyBase {
public:
    NesoraVowelDictionaly(){}

    int GetWord(int idx, ParametricNesoraParameterValue* outputParameters, double* outputPitch, double* outputEnvelope) override;

    int SetScript(NesoraScriptBase* script) override;
    NesoraScriptBase* GetScript() override;
    int RefreshCache() override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

    void AddWord(const std::string& word, const std::map<uint32_t, std::vector<ParametricNesoraDictionalyWordDeltaAndTime>>& parameterDeltas);
    ParametricNesoraDictionalyWord GetWord(const std::string& word) const;
    ParametricNesoraDictionalyWord& GetWord(const std::string& word);

private:
    ParametricNesoraDictionary parameters; // パラメータの辞書

    std::vector<ParametricNesoraParameterValue> outputParameterList;
    std::vector<double> outputPitchList;
    std::vector<double> outputEnvelopeList;

};

#endif //NESORA_VOWEL_DICTIONALY_H

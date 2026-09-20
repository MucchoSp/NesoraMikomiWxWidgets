// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraVowelDictionaly.h"

// MARK: NesoraVowelDictionaly

int NesoraVowelDictionaly::GetWord(int idx, ParametricNesoraParameterValue* outputParameters, double* outputPitch, double* outputEnvelope) {
    // ParametricNesoraDictionalyWord param = parameters[script->GetWord(t)];

    
    
    return 0;
}

int NesoraVowelDictionaly::SetScript(NesoraScriptBase* script) {
    this->script = script;
    return 0;
}

int NesoraVowelDictionaly::RefreshCache() {
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


NesoraScriptBase* NesoraVowelDictionaly::GetScript() {
    return script;
}

void NesoraVowelDictionaly::AddWord(const std::string& word, const std::map<uint32_t, std::vector<ParametricNesoraDictionalyWordDeltaAndTime>>& parameterDeltas) {
    ParametricNesoraDictionalyWord newWord;
    newWord.word = word;
    newWord.parameterDeltas = parameterDeltas;
    parameters[word] = newWord;
}

ParametricNesoraDictionalyWord NesoraVowelDictionaly::GetWord(const std::string& word) const {
    auto it = parameters.find(word);
    if (it != parameters.end()) {
        return it->second;
    } else {
        return ParametricNesoraDictionalyWord(); // 見つからない場合は空の単語を返す
    }
}

ParametricNesoraDictionalyWord& NesoraVowelDictionaly::GetWord(const std::string& word) {
    return parameters[word]; // 存在しない場合は新しい単語が作成される
}


std::vector<unsigned char> NesoraVowelDictionaly::SaveData() {
    
}

void NesoraVowelDictionaly::LoadData(const std::vector<unsigned char>& data) {

}


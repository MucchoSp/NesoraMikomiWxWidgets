// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraVowelDictionaly.h"

#include <algorithm>

// 線形補完
double LinerInterpolationOfParameter(const std::vector<ParametricNesoraDictionalyValueAndTime>& values, double time) {
    if (values.empty()) {
        return 0.0; // 値がない場合は0を返す
    }

    // 時間が範囲外の場合は端の値を返す
    if (time <= values.front().time) {
        return values.front().value;
    }
    if (time >= values.back().time) {
        return values.back().value;
    }

    // 線形補間
    auto it = std::lower_bound(values.begin(), values.end(), time, [](const ParametricNesoraDictionalyValueAndTime& v, double t) {
        return v.time < t;
    });

    if (it != values.end() && it != values.begin()) {
        auto prev = it - 1;
        double t1 = prev->time;
        double t2 = it->time;
        double v1 = prev->value;
        double v2 = it->value;

        double ratio = (time - t1) / (t2 - t1);
        return v1 + ratio * (v2 - v1);
    }

    return 0.0; // 補間できなかった場合は0を返す
}





// MARK: NesoraVowelDictionaly

int NesoraVowelDictionaly::GetWord(int idx, ParametricNesoraParameterValue* outputParameters, double* outputPitch, double* outputEnvelope) {
    if (idx < 0 || idx >= static_cast<int>(outputPitchList.size())) {
        *outputParameters = ParametricNesoraParameterValue();
        *outputPitch = 0.0;
        *outputEnvelope = 0.0;
        return -1; // インデックスが範囲外の場合はエラーコードを返す
    }
    
    *outputParameters = outputParameterList[idx];
    *outputPitch = outputPitchList[idx];
    *outputEnvelope = outputEnvelopeList[idx];

    return 0;
}

int NesoraVowelDictionaly::SetScript(NesoraScriptBase* script) {
    this->script = script;
    return 0;
}

int NesoraVowelDictionaly::RefreshCache() {
    outputParameterList.clear();
    outputPitchList.clear();
    outputEnvelopeList.clear();

    if (script == nullptr) {
        return -1;
    }

    int listSize = 0;
    double samplingFrequency = script->GetSamplingFrequency();
    while(1) {
        double pitch = script->GetPitch((double)listSize / samplingFrequency);
        if(pitch == 0.0)break;
        outputPitchList.push_back(script->GetPitch((double)listSize / samplingFrequency));
        listSize++;
    }

    std::vector<std::string> wordList;
    std::vector<std::string> overlapWordList;
    for(int i = 0;i < listSize;i++) {
        wordList.push_back(script->GetWord((double)i / samplingFrequency));

        overlapWordList.push_back(script->GetWord((double)i / samplingFrequency));
        if(i > 0 && overlapWordList[i] != overlapWordList[i-1]) {
            // オーバーラップ時間（秒）をサンプリング周波数で変換
            auto wordIt = parameters.find(overlapWordList[i]);
            if (wordIt != parameters.end()) {
                int overlapCount = static_cast<int>(wordIt->second.overlapTime * samplingFrequency);
                for(int j = 0;j < overlapCount;j++) {
                    if(i - j - 1 >= 0) {
                        overlapWordList[i - j - 1] = overlapWordList[i];
                    }
                }
            }
        }
    }

    outputParameterList.resize(listSize);
    for(int i = 0;i < listSize;i++) {
        auto wordIt = parameters.find(overlapWordList[i]);
        if(wordIt != parameters.end()) {
            const auto& word = wordIt->second;
            for(const auto& param : word.fixedParameterValues) {
                outputParameterList[i][param.first] = LinerInterpolationOfParameter(param.second, (double)i / samplingFrequency);
            }
            for(const auto& param : word.blankParameterValues) {
                outputParameterList[i][param.first] = LinerInterpolationOfParameter(param.second, (double)i / samplingFrequency - (word.fixedTime - word.overlapTime));
            }
        }
    }
    
    outputEnvelopeList.resize(listSize);
    for(int i = 0;i < listSize;i++) {
        auto wordIt = parameters.find(overlapWordList[i]);
        if (wordIt == parameters.end()) {
            outputEnvelopeList[i] = 0.0;
            continue;
        }

        const auto& word = wordIt->second;
        outputEnvelopeList[i] = script->GetEnvelope((double)i / samplingFrequency)
            * LinerInterpolationOfParameter(word.fixedEnvelope, (double)i / samplingFrequency)
            * LinerInterpolationOfParameter(word.blankEnvelope, (double)i / samplingFrequency - (word.fixedTime - word.overlapTime));
    }

    return 0;
}


NesoraScriptBase* NesoraVowelDictionaly::GetScript() {
    return script;
}

void NesoraVowelDictionaly::AddWord(const std::string& symbol, const std::map<std::string, std::vector<ParametricNesoraDictionalyValueAndTime>>& parameterValues) {
    ParametricNesoraDictionalyWord newWord;
    newWord.symbol = symbol;
    newWord.fixedParameterValues = parameterValues;
    parameters[symbol] = newWord;
}

ParametricNesoraDictionalyWord NesoraVowelDictionaly::GetWord(const std::string& symbol) const {
    auto it = parameters.find(symbol);
    if (it != parameters.end()) {
        return it->second;
    } else {
        return ParametricNesoraDictionalyWord(); // 見つからない場合は空の単語を返す
    }
}

ParametricNesoraDictionalyWord& NesoraVowelDictionaly::GetWord(const std::string& symbol) {
    return parameters[symbol]; // 存在しない場合は新しい単語が作成される
}

ParametricNesoraDictionalyWord* NesoraVowelDictionaly::GetWordPtr(const std::string& symbol) {
    auto it = parameters.find(symbol);
    if (it != parameters.end()) {
        return &(it->second);
    } else {
        return nullptr; // 見つからない場合はnullptrを返す
    }
}


std::vector<unsigned char> NesoraVowelDictionaly::SaveData() {
    return std::vector<unsigned char>();
}

void NesoraVowelDictionaly::LoadData(const std::vector<unsigned char>& data) {

}

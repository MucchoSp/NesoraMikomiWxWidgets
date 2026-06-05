// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>
#include <map>

#include "../ParametricNesroaDefines.h"

#ifndef NESORA_DICTIONALY_H
#define NESORA_DICTIONALY_H

class NesoraDictionalyBase {
public:
    NesoraDictionalyBase(){}

    virtual std::map<uint32_t, ParametricNesoraParameter> Vowel(double t) = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

private:
};


class NesoraVowelDictionaly : public NesoraDictionalyBase {
public:
    NesoraVowelDictionaly(){}

    std::map<uint32_t, ParametricNesoraParameter> Vowel(double t) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

    void AddWord(const std::string& word, const std::map<uint32_t, std::vector<ParametricNesoraDictionalyWordDeltaAndTime>>& parameterDeltas) {
        ParametricNesoraDictionalyWord newWord;
        newWord.word = word;
        newWord.parameterDeltas = parameterDeltas;
        parameters[word] = newWord;
    }
    ParametricNesoraDictionalyWord GetWord(const std::string& word) const {
        auto it = parameters.find(word);
        if (it != parameters.end()) {
            return it->second;
        } else {
            return ParametricNesoraDictionalyWord(); // 見つからない場合は空の単語を返す
        }
    }
    ParametricNesoraDictionalyWord& GetWord(const std::string& word) {
        return parameters[word]; // 存在しない場合は新しい単語が作成される
    }

private:
    ParametricNesoraDictionary parameters; // パラメータの辞書
};



#endif //NESORA_DICTIONALY_H

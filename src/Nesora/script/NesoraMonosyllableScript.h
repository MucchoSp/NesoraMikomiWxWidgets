// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "NesoraScript.h"

#ifndef NESORA_MONOSYLLABLE_SCRIPT_H
#define NESORA_MONOSYLLABLE_SCRIPT_H


class NesoraMonosyllableScript : public NesoraScriptBase {
public:
    NesoraMonosyllableScript() {}
    NesoraMonosyllableScript(double pitch, double envelope, const std::string& word, double length) : pitch(pitch), envelope(envelope), word(word), length(length) {}

    double GetPitch(double t) override {
        if (t >= 0 && t <= length)
            return pitch;
        return 0.0;
    }
    double GetEnvelope(double t) override {
        if (t >= 0 && t <= length)
            return envelope;
        return 0.0;
    }
    std::string GetWord(double t) override {
        if (t >= 0 && t <= length)
            return word;
        return "";
    }
    void SetPitch(double p) { pitch = p; }
    void SetEnvelope(double e) { envelope = e; }
    void SetWord(const std::string& w) { word = w; }
    void SetLength(double l) { length = l; }
    double GetLength() const { return length; }

    std::vector<unsigned char> SaveData() override { return {}; }
    void LoadData(const std::vector<unsigned char>& data) override {}
private:
    double pitch = 0.0;
    double envelope = 0.0;
    std::string word;
    double length = 0.0;
};



#endif // NESORA_MONOSYLLABLE_SCRIPT_H

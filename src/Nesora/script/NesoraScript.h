// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>
#include "../ParametricNesroaDefines.h"

#ifndef NESORA_SCRIPT_H
#define NESORA_SCRIPT_H

class NesoraScriptBase {
public:
    NesoraScriptBase(){}

    virtual double GetPitch(double t) = 0;
    virtual double GetEnvelope(double t) = 0;
    virtual ParametricNesoraParameter Vowel(double t) = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

    void SetSamplingFrequency(double smpl) { samplingFrequency = smpl; }
    double GetSamplingFrequency() {return samplingFrequency; }
protected:

    double samplingFrequency = 48000;

private:
};

#endif //NESORA_SCRIPT_H

#pragma once
#include <vector>

#include "../filter/NesoraVowelParam.h"

#ifndef NESORA_SCRIPT_H
#define NESORA_SCRIPT_H

class NesoraScriptBase {
public:
    NesoraScriptBase(){}

    virtual double DeltaRadian(double t) = 0;
    virtual double Volume(double t) = 0;
    virtual nesoraVowelVector Vowel(double t) = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

    void SetSamplingFrequency(double smpl) { samplingFrequency = smpl; }
    double GetSamplingFrequency() {return samplingFrequency; }
protected:

    double samplingFrequency = 48000;

private:
};

#endif //NESORA_SCRIPT_H

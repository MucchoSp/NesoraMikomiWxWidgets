#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <random>

#include "NesoraScript.h"

#ifndef NESORA_SCRIPT_ONE_TONE_H
#define NESORA_SCRIPT_ONE_TONE_H

class NesoraScriptOneTone : public NesoraScriptBase {
public:
    NesoraScriptOneTone() {}

    double DeltaRadian(double t) override;
    double Volume(double t) override;
    nesoraVowelVector Vowel(double t) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;
};

#endif // NESORA_SCRIPT_ONE_TONE_H
#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <random>

#include "NesoraScript.h"
#include "../NesoraDefines.h"

#ifndef NESORA_SCRIPT_PLAY_H
#define NESORA_SCRIPT_PLAY_H

class NesoraScriptPlay : public NesoraScriptBase {
public:
    NesoraScriptPlay() {}

    double DeltaRadian(double t) override;
    double Volume(double t) override;
    nesoraVowelVector Vowel(double t) override;

    void SetPitch(double pitch);
    void SetVolume(double volume);
    void SetVowel(nesoraVowelVector vowel);

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double pitch = 261.63;      // [Hz]
    double volume = 1.0;
    nesoraVowelVector vowel = { 0 };
    uint64_t time = 0.0;        // 最大1000万年まで
};

#endif // NESORA_SCRIPT_PLAY_H
#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <random>

#include "NesoraSource.h"

#ifndef NESORA_ROSENBERG_WAVE_H
#define NESORA_ROSENBERG_WAVE_H

class NesoraRosenbergWave : public NesoraSourceBase {
public:

    NesoraRosenbergWave(){}
    NesoraRosenbergWave(double tau1, double tau2, double a0, double noise_level = 0.0) {
        SetParamater(tau1, tau2, a0, noise_level);
    }

    void SetParamater(double tau1, double tau2, double a0, double noise_level);
    double GetT1() const;
    double GetT2() const;
    double GetA0() const;
    double GetNoise() const;

    double Utterance(double radian) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double t1 = 0.0, t2 = 0.0, a0 = -0.5, noise = 0.0;

};

#endif // NESORA_ROSENBERG_WAVE_H
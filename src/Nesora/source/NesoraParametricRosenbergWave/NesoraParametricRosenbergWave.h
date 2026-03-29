// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <map>

#include "../NesoraSource.h"

#include "../../ParametricNesroaDefines.h"

#ifndef NESORA_PARAMETRIC_ROSENBERG_WAVE_H
#define NESORA_PARAMETRIC_ROSENBERG_WAVE_H

struct NesoraRosenbergParameter {
    double tau1;
    double tau2;
    double a0;
    double noise; 
};

class NesoraParametricRosenbergWave : public NesoraSourceBase {
public:

    NesoraParametricRosenbergWave(){}
    NesoraParametricRosenbergWave(double tau1, double tau2, double a0, double noise_level = 0.0) {
        SetParamater(tau1, tau2, a0, noise_level);
    }

    void SetParamater(double tau1, double tau2, double a0, double noise_level);
    void SetParamater(const std::map<uint32_t, double>& parameters);
    void SetDelta(const std::map<uint32_t, ParametricNesoraRosenbergWaveParameter>& in_delta);
    void AddDelta(uint32_t in_delta_ID, ParametricNesoraRosenbergWaveParameter in_delta_value);
    void SetA0(double a0);
    double GetT1() const;
    double GetT2() const;
    double GetA0() const;
    double GetNoise() const;

    double Utterance(double radian) override;

    const NesoraRosenbergParameter GetParametricSource(const std::map<uint32_t, double>& parameters) const;
    const NesoraRosenbergParameter GetParametricSource(const uint32_t parameterID, const double delta) const;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double normal_t1 = 0.0, normal_t2 = 0.0, normal_a0 = -0.5, normal_noise = 0.0;
    double t1 = 0.0, t2 = 0.0, a0 = -0.5, noise = 0.0;
    std::map<uint32_t, ParametricNesoraRosenbergWaveParameter> delta;

};

#endif // NESORA_PARAMETRIC_ROSENBERG_WAVE_H
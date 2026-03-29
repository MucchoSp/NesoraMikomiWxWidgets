// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma ones

#include <map>

#ifndef PARAMETRIC_NESORA_DEFINES
#define PARAMETRIC_NESORA_DEFINES

struct ParametricNesoraIIRFilterParameter {
    double delta_r;
    double delta_theta;
};

struct ParametricNesoraRosenbergWaveParameter {
    double delta_tau1;
    double delta_tau2;
    double delta_noise;
    double delta_a0;
};

struct ParametricNesoraParameter {
    std::vector<ParametricNesoraIIRFilterParameter> filter;
    std::vector<ParametricNesoraRosenbergWaveParameter> source;
    double a0;
};

#endif //PARAMETRIC_NESORA_DEFINES
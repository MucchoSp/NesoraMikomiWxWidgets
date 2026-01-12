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
};

struct ParametricNesoraParameter {
    std::vector<ParametricNesoraIIRFilterParameter> filter;
    std::vector<ParametricNesoraRosenbergWaveParameter> source;
};

#endif //PARAMETRIC_NESORA_DEFINES
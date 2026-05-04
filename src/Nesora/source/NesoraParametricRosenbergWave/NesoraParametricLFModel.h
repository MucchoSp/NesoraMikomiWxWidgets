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
#include "../../NesoraDefines.h"

#ifndef NESORA_PARAMETRIC_LF_MODEL_H
#define NESORA_PARAMETRIC_LF_MODEL_H

struct NesoraLFModelParameter {
    double Tp;      // Peak time(波形がプラスのピークに達するまでの時間。)
    double Te;      // End time(波形がマイナスのピークに達するまでの時間。)
    double Ta;      // Attack time(波形がピークに達するまでの時間。)
    double Ee;      // End energy(波形の終了時のエネルギー。)
    double noise;   // Noise level(ノイズレベル。)
};

class NesoraParametricLFModel : public NesoraSourceBase {
public:

    NesoraParametricLFModel(){}
    NesoraParametricLFModel(double Tp, double Te, double Ta, double Ee, double noise_level = 0.0) {
        SetParamater(Tp, Te, Ta, Ee, noise_level);
    }

    void SetParamater(double Tp, double Te, double Ta, double Ee, double noise_level);
    void SetParamater(const std::map<uint32_t, double>& parameters) {}
    void SetDelta(const std::map<uint32_t, ParametricNesoraRosenbergWaveParameter>& in_delta) {}
    void AddDelta(uint32_t in_delta_ID, ParametricNesoraRosenbergWaveParameter in_delta_value) {}
    double GetTp() const;
    double GetTe() const;
    double GetTa() const;
    double GetEe() const;
    double GetNoise() const;

    double Utterance(double radian) override;

    const NesoraLFModelParameter GetParametricSource(const std::map<uint32_t, double>& parameters) const { return NesoraLFModelParameter(); }
    const NesoraLFModelParameter GetParametricSource(const uint32_t parameterID, const double delta) const { return NesoraLFModelParameter(); }

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double normal_tp = 0.0, normal_te = 0.0, normal_ta = 0.0, normal_ee = 0.0, normal_noise = 0.0;
    double tp = 0.0, te = 0.0, ta = 0.0, ee = 0.0, noise = 0.0;
    std::map<uint32_t, ParametricNesoraRosenbergWaveParameter> delta;

    double phi_e = 0.0, omega_g = 0.0, alpha = 0.0, beta = 0.0, E0 = 0.0, constVal = 0.0;
    double solve_epsilon(double Te, double Ta, double Tc);
    double solve_a(double Tp, double Te, double Ta, double Ee, double Tc, double epsilon);
    double calculate_I1(double a, double Tp, double Te, double Ee, double omega_g);
    double calculate_I2(double Te, double Ta, double Tc, double Ee, double epsilon);

};

class NesoraParametricLFModelRdParameter : public NesoraSourceBase {
public:

    NesoraParametricLFModelRdParameter(){}
    NesoraParametricLFModelRdParameter(double Rd, double Ee, double noise_level = 0.0) {
        SetParamater(Rd, Ee, noise_level);
    }

    void SetParamater(double Rd,  double Ee, double noise_level);
    void SetParamater(const std::map<uint32_t, double>& parameters) {}
    void SetDelta(const std::map<uint32_t, ParametricNesoraRosenbergWaveParameter>& in_delta) {}
    void AddDelta(uint32_t in_delta_ID, ParametricNesoraRosenbergWaveParameter in_delta_value) {}
    double GetRd() const;
    double GetEe() const;
    double GetNoise() const;

    double Utterance(double radian) override;

    const NesoraLFModelParameter GetParametricSource(const std::map<uint32_t, double>& parameters) const { return NesoraLFModelParameter(); }
    const NesoraLFModelParameter GetParametricSource(const uint32_t parameterID, const double delta) const { return NesoraLFModelParameter(); }

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double normal_rd = 0.0, normal_ee = 0.0, normal_noise = 0.0;
    double rd = 0.0, tp = 0.0, te = 0.0, ta = 0.0, ee = 0.0, noise = 0.0;
    std::map<uint32_t, ParametricNesoraRosenbergWaveParameter> delta;

    double phi_e = 0.0, omega_g = 0.0, alpha = 0.0, beta = 0.0, E0 = 0.0, constVal = 0.0;
    double solve_epsilon(double Te, double Ta, double Tc);
    double solve_a(double Tp, double Te, double Ta, double Ee, double Tc, double epsilon);
    double calculate_I1(double a, double Tp, double Te, double Ee, double omega_g);
    double calculate_I2(double Te, double Ta, double Tc, double Ee, double epsilon);

};

#endif // NESORA_PARAMETRIC_LF_MODEL_H
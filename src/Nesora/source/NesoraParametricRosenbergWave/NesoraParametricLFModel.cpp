// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraParametricLFModel.h"


// MARK: NesoraParametricLFModel

double NesoraParametricLFModel::Utterance(double theta) {
    double noise_input = (double)std::rand() / (double)RAND_MAX * 2.0 - 1.0;
    // 0 ~ 2pi の範囲外をガード（必要に応じて）
    if (theta >= 2.0 * M_PI) theta = fmod(theta, 2.0 * M_PI);
    
    if (theta <= phi_e) {
        // 前半フェーズ
        double output = E0 * std::exp(alpha * theta) * std::sin(omega_g * theta);
        return output + noise_input * noise;
    } else {
        // 後半フェーズ（指数帰還）
        return -constVal * (std::exp(-beta * (theta - phi_e)) - std::exp(-beta * (2.0 * M_PI - phi_e))) + noise_input * noise;
    }
}

void NesoraParametricLFModel::SetParamater(double Tp, double Te, double Ta, double Ee, double noise_level) {
    normal_tp = Tp;
    normal_te = Te;
    normal_ta = Ta;
    normal_ee = Ee;
    normal_noise = noise_level;
    noise = noise_level;

    phi_e = 2.0 * M_PI * Te;
    double phi_p = 2.0 * M_PI * Tp;
    omega_g = M_PI / phi_p;

    beta = solve_epsilon(phi_e, 2.0 * M_PI * Ta, 2.0 * M_PI);

    alpha = solve_a(phi_p, phi_e, 2.0 * M_PI * Ta, Ee, 2.0 * M_PI, beta);

    E0 = -Ee / (std::exp(alpha * phi_e) * std::sin(omega_g * phi_e));

    double exp_term_end = std::exp(-beta * (2.0 * M_PI - phi_e));
    constVal = Ee / (1.0 - exp_term_end);
}

double NesoraParametricLFModel::solve_epsilon(double Te, double Ta, double Tc) {
    double dt = Tc - Te; // 帰還フェーズの長さ
    
    // 初期値の推定（Taが小さいほどepsilonは大きい）
    // Taが十分小さい場合、epsilon ≒ 1/Ta と近似できる
    double eps = 1.0 / Ta; 
    
    // ニュートン法
    for (int i = 0; i < 20; ++i) {
        double exp_term = std::exp(-eps * dt);
        
        // f(eps) = 1 - exp(-eps * dt) - eps * Ta
        double f = 1.0 - exp_term - eps * Ta;
        
        // f'(eps) = dt * exp(-eps * dt) - Ta
        double df = dt * exp_term - Ta;
        
        double next_eps = eps - f / df;
        
        // 収束判定
        if (std::abs(next_eps - eps) < 1e-10) {
            return next_eps;
        }
        eps = next_eps;
    }
    return eps;
}

double NesoraParametricLFModel::solve_a(double Tp, double Te, double Ta, double Ee, double Tc, double epsilon) {
    double omega_g = M_PI / Tp;
    // 後半の積分 I2 を計算 (aに依存しない定数)
    double I2 = calculate_I2(Te, Ta, Tc, Ee, epsilon);

    // f(a) = I1(a) + I2 = 0 となる a を二分法で探す
    double lower_a = -50.0; // 適切な範囲
    double upper_a = 50.0;
    double mid_a;
    double f_lower = calculate_I1(lower_a, Tp, Te, Ee, omega_g) + I2;
    double f_upper = calculate_I1(upper_a, Tp, Te, Ee, omega_g) + I2;

    // 答えが範囲内にない場合のガード
    if (f_lower * f_upper > 0) {
        // 異常値だが、とりあえずどちらかに寄せる
        return (std::abs(f_lower) < std::abs(f_upper)) ? lower_a : upper_a;
    }

    for (int i = 0; i < 100; ++i) { // 最大100回ループ
        mid_a = (lower_a + upper_a) / 2.0;
        double f_val = calculate_I1(mid_a, Tp, Te, Ee, omega_g) + I2;

        if (std::abs(f_val) < 1e-10) break; // 収束したら終了

        if (f_val > 0) lower_a = mid_a;
        else upper_a = mid_a;
    }
    return mid_a;
}

double NesoraParametricLFModel::calculate_I1(double a, double Tp, double Te, double Ee, double omega_g) {
    // I1 = (-Ee / (den * sin_we)) * (a * sin_we - omega_g * cos_we + omega_g * exp(-a * Te))
    double sin_we = std::sin(omega_g * Te);
    double cos_we = std::cos(omega_g * Te);
    double den = a * a + omega_g * omega_g;

    double term = a * sin_we - omega_g * cos_we + omega_g * std::exp(-a * Te);
    return (-Ee / (den * sin_we)) * term;
}

double NesoraParametricLFModel::calculate_I2(double Te, double Ta, double Tc, double Ee, double epsilon) {
    // $$I_2 = \frac{-E_e}{\epsilon^2 T_a} \left[ 1 - e^{-\epsilon(T_c-T_e)} - \epsilon(T_c-T_e) e^{-\epsilon(T_c-T_e)} \right]$$
    double dt = Tc - Te;
    double exp_term = std::exp(-epsilon * dt);
    return (-Ee / (epsilon * epsilon * Ta)) * (1.0 - exp_term - epsilon * dt * exp_term);
}

double NesoraParametricLFModel::GetTp() const {
    return tp;
}

double NesoraParametricLFModel::GetTe() const {
    return te;
}

double NesoraParametricLFModel::GetTa() const {
    return ta;
}

double NesoraParametricLFModel::GetEe() const {
    return ee;
}

double NesoraParametricLFModel::GetNoise() const {
    return noise;
}

std::vector<unsigned char> NesoraParametricLFModel::SaveData() {

}

void NesoraParametricLFModel::LoadData(const std::vector<unsigned char>& data) {

}









// MARK: NesoraParametricLFModelRdParameter

double NesoraParametricLFModelRdParameter::Utterance(double theta) {
    double noise_input = (double)std::rand() / (double)RAND_MAX * 2.0 - 1.0;
    // 0 ~ 2pi の範囲外をガード（必要に応じて）
    if (theta >= 2.0 * M_PI) theta = fmod(theta, 2.0 * M_PI);
    
    if (theta <= phi_e) {
        // 前半フェーズ
        double output = E0 * std::exp(alpha * theta) * std::sin(omega_g * theta);
        return output + noise_input * noise;
    } else {
        // 後半フェーズ（指数帰還）
        return -constVal * (std::exp(-beta * (theta - phi_e)) - std::exp(-beta * (2.0 * M_PI - phi_e))) + noise_input * noise;
    }
}

void NesoraParametricLFModelRdParameter::SetParamater(double Rd, double Ee, double noise_level) {
    normal_rd = Rd;
    normal_ee = Ee;
    normal_noise = noise_level;
    noise = noise_level;

    // Rd の範囲制限 (0.3 ~ 2.7 が一般的)
    if (Rd < 0.3) Rd = 0.3;
    if (Rd > 2.7) Rd = 2.7;

    rd = Rd;

    // 各種比率の計算 (Fantの近似式)
    double Ra = (-1.0 + 4.8 * Rd) / 100.0;
    double Rk = (22.4 + 11.8 * Rd) / 100.0;
    double Rg = 1.0 / (2.0 * Rk * ((5.0 + 1.2 * Rd) / 100.0));

    // Tp, Te, Ta への変換 (Tc = 1.0 とした相対値)
    double Tp = 2.0 * M_PI / (2.0 * Rg);
    double Te = Tp * (1.0 + Rk);
    double Ta = Ra;

    phi_e = 2.0 * M_PI * Te;
    double phi_p = 2.0 * M_PI * Tp;
    omega_g = M_PI / phi_p;

    beta = solve_epsilon(phi_e, 2.0 * M_PI * Ta, 2.0 * M_PI);

    alpha = solve_a(phi_p, phi_e, 2.0 * M_PI * Ta, Ee, 2.0 * M_PI, beta);

    E0 = -Ee / (std::exp(alpha * phi_e) * std::sin(omega_g * phi_e));

    double exp_term_end = std::exp(-beta * (2.0 * M_PI - phi_e));
    constVal = Ee / (1.0 - exp_term_end);
}

double NesoraParametricLFModelRdParameter::solve_epsilon(double Te, double Ta, double Tc) {
    double dt = Tc - Te; // 帰還フェーズの長さ
    
    // 初期値の推定（Taが小さいほどepsilonは大きい）
    // Taが十分小さい場合、epsilon ≒ 1/Ta と近似できる
    double eps = 1.0 / Ta; 
    
    // ニュートン法
    for (int i = 0; i < 20; ++i) {
        double exp_term = std::exp(-eps * dt);
        
        // f(eps) = 1 - exp(-eps * dt) - eps * Ta
        double f = 1.0 - exp_term - eps * Ta;
        
        // f'(eps) = dt * exp(-eps * dt) - Ta
        double df = dt * exp_term - Ta;
        
        double next_eps = eps - f / df;
        
        // 収束判定
        if (std::abs(next_eps - eps) < 1e-10) {
            return next_eps;
        }
        eps = next_eps;
    }
    return eps;
}

double NesoraParametricLFModelRdParameter::solve_a(double Tp, double Te, double Ta, double Ee, double Tc, double epsilon) {
    double omega_g = M_PI / Tp;
    // 後半の積分 I2 を計算 (aに依存しない定数)
    double I2 = calculate_I2(Te, Ta, Tc, Ee, epsilon);

    // f(a) = I1(a) + I2 = 0 となる a を二分法で探す
    double lower_a = -50.0; // 適切な範囲
    double upper_a = 50.0;
    double mid_a;
    double f_lower = calculate_I1(lower_a, Tp, Te, Ee, omega_g) + I2;
    double f_upper = calculate_I1(upper_a, Tp, Te, Ee, omega_g) + I2;

    // 答えが範囲内にない場合のガード
    if (f_lower * f_upper > 0) {
        // 異常値だが、とりあえずどちらかに寄せる
        return (std::abs(f_lower) < std::abs(f_upper)) ? lower_a : upper_a;
    }

    for (int i = 0; i < 100; ++i) { // 最大100回ループ
        mid_a = (lower_a + upper_a) / 2.0;
        double f_val = calculate_I1(mid_a, Tp, Te, Ee, omega_g) + I2;

        if (std::abs(f_val) < 1e-10) break; // 収束したら終了

        if (f_val > 0) lower_a = mid_a;
        else upper_a = mid_a;
    }
    return mid_a;
}

double NesoraParametricLFModelRdParameter::calculate_I1(double a, double Tp, double Te, double Ee, double omega_g) {
    // I1 = (-Ee / (den * sin_we)) * (a * sin_we - omega_g * cos_we + omega_g * exp(-a * Te))
    double sin_we = std::sin(omega_g * Te);
    double cos_we = std::cos(omega_g * Te);
    double den = a * a + omega_g * omega_g;

    double term = a * sin_we - omega_g * cos_we + omega_g * std::exp(-a * Te);
    return (-Ee / (den * sin_we)) * term;
}

double NesoraParametricLFModelRdParameter::calculate_I2(double Te, double Ta, double Tc, double Ee, double epsilon) {
    // $$I_2 = \frac{-E_e}{\epsilon^2 T_a} \left[ 1 - e^{-\epsilon(T_c-T_e)} - \epsilon(T_c-T_e) e^{-\epsilon(T_c-T_e)} \right]$$
    double dt = Tc - Te;
    double exp_term = std::exp(-epsilon * dt);
    return (-Ee / (epsilon * epsilon * Ta)) * (1.0 - exp_term - epsilon * dt * exp_term);
}

double NesoraParametricLFModelRdParameter::GetRd() const {
    return rd;
}

double NesoraParametricLFModelRdParameter::GetEe() const {
    return ee;
}

double NesoraParametricLFModelRdParameter::GetNoise() const {
    return noise;
}

std::vector<unsigned char> NesoraParametricLFModelRdParameter::SaveData() {

}

void NesoraParametricLFModelRdParameter::LoadData(const std::vector<unsigned char>& data) {

}





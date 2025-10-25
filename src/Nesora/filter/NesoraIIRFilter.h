# pragma once

#include <vector>
#include <cmath>
#include <complex>

#include "NesoraFilter.h"

#ifndef NESORA_IIRFILTER_H
#define NESORA_IIRFILTER_H

class NesoraIIRFilter : public NesoraFilterBase {
public:
    NesoraIIRFilter(){}
    NesoraIIRFilter(double feedback_coefficient) {
        // 単純な1次IIRフィルタの例: y[n] = x[n] + feedback_coefficient * y[n-1]
        a_coefficients = {1.0, -feedback_coefficient}; // 分母係数
        b_coefficients = {1.0}; // 分子係数
        history.resize(a_coefficients.size() - 1, 0.0);
        output_history.resize(a_coefficients.size() - 1, 0.0);
        input_history.resize(b_coefficients.size() - 1, 0.0);
    }
    NesoraIIRFilter(const std::vector<std::complex<double>>& a_coeffs, const std::vector<std::complex<double>>& b_coeffs) {
        SetCoefficients(a_coeffs, b_coeffs);
        history.resize(a_coefficients.size() - 1, 0.0);
        output_history.resize(a_coefficients.size() - 1, 0.0);
        input_history.resize(b_coefficients.size() - 1, 0.0);
    }

    void SetCoefficients(const std::vector<std::complex<double>>& a_coeffs, const std::vector<std::complex<double>>& b_coeffs);
    std::vector<std::complex<double>> GetACoefficients() const;
    std::vector<std::complex<double>>& GetACoefficients();
    std::vector<std::complex<double>> GetBCoefficients() const;
    std::vector<std::complex<double>>& GetBCoefficients();
    void Reset();

    std::vector<double> CalculateFrequencyResponse(int num_samples);

    std::vector<double> GetResponse() const;

    double Filter(double x) override;
private:

    std::vector<double> history;
    std::vector<double> output_history;
    std::vector<double> input_history;

    std::vector<std::complex<double>> a_coefficients;
    std::vector<std::complex<double>> b_coefficients;

    std::vector<double> response;
};

#endif // NESORA_IIRFILTER_H

# pragma once

#include <vector>
#include <cmath>
#include <complex>

#include "NesoraFilter.h"

#ifndef NESORA_IIRFILTER_H
#define NESORA_IIRFILTER_H


struct NesoraIIRFilterPD {
    double r;
    double theta;
};


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
    NesoraIIRFilter(const std::vector<double>& a_coeffs, const std::vector<double>& b_coeffs) {
        SetCoefficients(a_coeffs, b_coeffs);
        history.resize(a_coefficients.size() - 1, 0.0);
        output_history.resize(a_coefficients.size() - 1, 0.0);
        input_history.resize(b_coefficients.size() - 1, 0.0);
    }

    void SetCoefficients(const std::vector<double>& a_coeffs, const std::vector<double>& b_coeffs);

    std::vector<NesoraIIRFilterPD> GetPeaks() const;
    std::vector<NesoraIIRFilterPD>& GetPeaks();
    std::vector<NesoraIIRFilterPD> GetDips() const;
    std::vector<NesoraIIRFilterPD>& GetDips();

    void Reset();

    void CalculateCoefficientsFromPDs();
    std::vector<double> CalculateFrequencyResponse(int num_samples);

    std::vector<double> GetResponse() const;

    double Filter(double x) override {return 0.0; /* 未実装 */ }
private:

    std::vector<double> history;
    std::vector<double> output_history;
    std::vector<double> input_history;

    std::vector<double> a_coefficients;
    std::vector<double> b_coefficients;

    std::vector<double> response;

    std::vector<NesoraIIRFilterPD> peaks;
    std::vector<NesoraIIRFilterPD> dips;
};

#endif // NESORA_IIRFILTER_H

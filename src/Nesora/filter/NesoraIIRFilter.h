# pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <iostream>
#include <thread>
#include <numeric>

// #include "../../../lib/pocketfft/pocketfft_hdronly.h"

#include "NesoraFilter.h"

#ifndef NESORA_IIRFILTER_H
#define NESORA_IIRFILTER_H

class NesoraIIRFilter : public NesoraFilterBase {
public:
    NesoraIIRFilter(){}
    NesoraIIRFilter(double feedback_coefficient, double smpl) {
        a_coefficients = {1.0, -feedback_coefficient}; // 分母係数
        b_coefficients = {1.0}; // 分子係数
        output_history.resize(a_coefficients.size() - 1, 0.0);
        input_history.resize(b_coefficients.size() - 1, 0.0);
        samplingFrequency = smpl;
    }
    NesoraIIRFilter(const std::vector<double>& a_coeffs, const std::vector<double>& b_coeffs, double smpl) {
        SetCoefficients(a_coeffs, b_coeffs);
        history.resize(a_coefficients.size() - 1, 0.0);
        output_history.resize(a_coefficients.size() - 1, 0.0);
        input_history.resize(b_coefficients.size() - 1, 0.0);
        samplingFrequency = smpl;
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

    double Filter(double x) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double samplingFrequency = NesoraDefaultSamplingFrequency;

    std::vector<double> history;
    std::vector<double> output_history;
    std::vector<double> input_history;

    std::vector<double> a_coefficients;
    std::vector<double> b_coefficients;

    std::vector<double> response;

    std::vector<NesoraIIRFilterPD> peaks;
    std::vector<NesoraIIRFilterPD> dips;

    double Gain = 1;
    size_t input_index = 0;
    size_t output_index = 0;
};

#endif // NESORA_IIRFILTER_H

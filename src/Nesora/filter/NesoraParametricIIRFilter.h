# pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <iostream>
#include <thread>
#include <numeric>

#include "NesoraFilter.h"
#include "NesoraIIRFilter.h"

#include "../ParametricNesroaDefines.h"

#ifndef NESORA_PARAMETRIC_IIRFILTER_H
#define NESORA_PARAMETRIC_IIRFILTER_H



class NesoraParametricIIRFilter : public NesoraFilterBase {
public:
    NesoraParametricIIRFilter(){}
    NesoraParametricIIRFilter(double smpl) {
        a_coefficients = {1.0}; // 分母係数
        b_coefficients = {1.0}; // 分子係数
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

    void CalculateCoefficientsFromPDs(const std::map<uint32_t, double>& parameters);
    std::vector<double> CalculateFrequencyResponse(int num_samples);
    std::vector<double> GetResponse() const;

    double Filter(double x) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double samplingFrequency = NesoraDefaultSamplingFrequency;

    std::vector<double> output_history;
    std::vector<double> input_history;

    std::vector<double> a_coefficients;
    std::vector<double> b_coefficients;

    std::vector<double> response;

    std::vector<NesoraIIRFilterPD> peaks;
    std::vector<NesoraIIRFilterPD> dips;

    std::map<uint32_t, std::vector<ParametricNesoraIIRFilterParameter>> peaks_parameter_status;
    std::map<uint32_t, std::vector<ParametricNesoraIIRFilterParameter>> dips_parameter_status;

    double Gain = 1;
    bool sorted = false;
    size_t input_index = 0;
    size_t output_index = 0;
};


#endif // NESORA_PARAMETRIC_IIRFILTER_H

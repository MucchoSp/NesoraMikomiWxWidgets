# pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <iostream>

#include "NesoraFilter.h"
#include "NesoraIIRFilter.h"

#ifndef NESORA_CUBE_IIRFILTER_H
#define NESORA_CUBE_IIRFILTER_H



class NesoraCubeIIRFilter : public NesoraFilterBase {
public:
    NesoraCubeIIRFilter(){}
    NesoraCubeIIRFilter(std::vector<NesoraIIRFilter> filters, double smpl) {
        IIRFilters = filters;
        output_history.resize(a_coeff_size - 1, 0.0);
        input_history.resize(b_coeff_size - 1, 0.0);
        samplingFrequency = smpl;
    }

    void SetCoefficients(const std::vector<double>& a_coeffs, const std::vector<double>& b_coeffs);

    void Reset();

    void CalculateCoefficientsFromPDs();
    std::vector<double> CalculateFrequencyResponse(int num_samples);

    double Filter(double x) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double samplingFrequency = NesoraDefaultSamplingFrequency;

    std::vector<double> output_history;
    std::vector<double> input_history;

    std::vector<double> response;

    std::vector<NesoraIIRFilter> IIRFilters;

    double Gain = 1;
    bool sorted = false;
    size_t a_coeff_size = 1, b_coeff_size = 1;
};


#endif // NESORA_CUBE_IIRFILTER_H

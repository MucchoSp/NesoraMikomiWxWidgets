# pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <iostream>
#include <thread>
#include <numeric>

#include "../NesoraFilter.h"

#include "../../ParametricNesroaDefines.h"

#ifndef NESORA_PARAMETRIC_SOSIIRFILTER_H
#define NESORA_PARAMETRIC_SOSIIRFILTER_H


class NesoraParametricSOFilter {
public:
    NesoraParametricSOFilter(){}
    NesoraParametricSOFilter(NesoraIIRFilterPD filter) : point(filter) {}

    void Reset();
    
    void SetPoint(NesoraIIRFilterPD in_point);
    void SetDelta(const std::map<int, ParametricNesoraIIRFilterParameter>& in_delta);
    void CalculateCoefficients(const std::map<int, double>& parameters);
    double CalculateFrequencyResponse(double omega) const;
    
    double Filter(double x);
    
    NesoraIIRFilterPD GetPoint();
    const NesoraIIRFilterPD& GetPoint() const;
    std::map<int, ParametricNesoraIIRFilterParameter> GetDelta();

private:
    NesoraIIRFilterPD point = {0};

    double s1 = 0, s2 = 0;
    double b0 = 1, b1 = 0, b2 = 0;
    double a1 = 0, a2 = 0;

    std::map<int, ParametricNesoraIIRFilterParameter> delta;
};


class NesoraParametricSOSIIRFilter : public NesoraFilterBase {
public:
    NesoraParametricSOSIIRFilter(){}
    NesoraParametricSOSIIRFilter(int in_samplingFrequency) : samplingFrequency(in_samplingFrequency) {}

    void Reset();

    void CalculateCoefficients(const std::map<int, double>& parameters);
    const std::vector<double>& CalculateFrequencyResponse(int num_samples);
    const std::vector<double>& GetResponse() const;

    double Filter(double x) override;

    std::vector<unsigned char> SaveData() override { return {}; };
    void LoadData(const std::vector<unsigned char>& data) override {};

    const std::vector<NesoraParametricSOFilter>& GetSOFilter() const;
    std::vector<NesoraParametricSOFilter>& GetSOFilter();

private:

    double samplingFrequency = NesoraDefaultSamplingFrequency;

    std::vector<double> response;

    std::vector<NesoraParametricSOFilter> SOFilters;
};


#endif // NESORA_PARAMETRIC_IIRFILTER_H

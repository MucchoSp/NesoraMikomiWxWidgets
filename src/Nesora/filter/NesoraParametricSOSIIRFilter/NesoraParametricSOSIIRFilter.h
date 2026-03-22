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
    void SetDelta(const std::map<uint32_t, ParametricNesoraIIRFilterParameter>& in_delta);
    void AddDelta(uint32_t in_delta_ID, ParametricNesoraIIRFilterParameter in_delta_value);
    void SetDestinationPoint(uint32_t parameterID, NesoraIIRFilterPD in_point);
    void CalculateCoefficients(const std::map<uint32_t, double>& parameters);
    double CalculateFrequencyResponse(double omega) const;
    
    double Filter(double x);
    
    NesoraIIRFilterPD GetPoint();
    const NesoraIIRFilterPD& GetPoint() const;
    const NesoraIIRFilterPD GetParametricPoint(const std::map<uint32_t, double>& parameters) const;
    const NesoraIIRFilterPD GetParametricPoint(const uint32_t parameterID, const double delta) const;
    std::map<uint32_t, ParametricNesoraIIRFilterParameter> GetDelta();
    const ParametricNesoraIIRFilterParameter GetDelta(const uint32_t parameterID) const;

private:
    NesoraIIRFilterPD point = {0};

    double s1 = 0, s2 = 0;
    double b0 = 1, b1 = 0, b2 = 0;
    double a1 = 0, a2 = 0;

    std::map<uint32_t, ParametricNesoraIIRFilterParameter> delta;
};


class NesoraParametricSOSIIRFilter : public NesoraFilterBase {
public:
    NesoraParametricSOSIIRFilter(){}
    NesoraParametricSOSIIRFilter(int in_samplingFrequency) : samplingFrequency(in_samplingFrequency) {}

    void Reset();

    void CalculateCoefficients(const std::map<uint32_t, double>& parameters);
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

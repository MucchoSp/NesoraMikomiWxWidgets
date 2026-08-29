// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
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
    void SetDelta(const std::vector<ParametricNesoraIIRFilterParameter>& in_delta);
    void AddDelta(size_t in_delta_Index, ParametricNesoraIIRFilterParameter in_delta_value);
    void SetDestinationPoint(size_t parameterIndex, NesoraIIRFilterPD in_point);
    void CalculateCoefficients(const ParametricNesoraDelta& parameters);
    double CalculateFrequencyResponse(double omega) const;
    
    double Filter(double x);
    double Filter(const ParametricNesoraDelta& parameters, double x);
    
    NesoraIIRFilterPD GetPoint();
    const NesoraIIRFilterPD& GetPoint() const;
    const NesoraIIRFilterPD GetParametricPoint(const ParametricNesoraDelta& parameters) const;
    const NesoraIIRFilterPD GetParametricPoint(const size_t parameterIndex, const double delta) const;
    std::vector<ParametricNesoraIIRFilterParameter> GetDelta() const;
    const ParametricNesoraIIRFilterParameter GetDelta(const size_t parameterIndex) const;

private:
    NesoraIIRFilterPD point = {0};

    double s1 = 0, s2 = 0;
    double b0 = 1, b1 = 0, b2 = 0;
    double a1 = 0, a2 = 0;

    // std::map<uint32_t, ParametricNesoraIIRFilterParameter> delta;
    std::vector<ParametricNesoraIIRFilterParameter> delta;
};


class NesoraParametricSOSIIRFilter : public NesoraParametricFilterBase {
public:
    NesoraParametricSOSIIRFilter(){}
    NesoraParametricSOSIIRFilter(int in_samplingFrequency) : samplingFrequency(in_samplingFrequency) {}

    void Reset();

    void CalculateCoefficients(const ParametricNesoraDelta& parameters);
    const std::vector<double>& CalculateFrequencyResponse(int num_samples);
    const std::vector<double>& GetResponse() const;

    double Filter(const ParametricNesoraDelta& parameters, double x) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

    const std::vector<NesoraParametricSOFilter>& GetSOFilter() const;
    std::vector<NesoraParametricSOFilter>& GetSOFilter();

private:

    double samplingFrequency = NesoraDefaultSamplingFrequency;

    std::vector<double> response;

    std::vector<NesoraParametricSOFilter> SOFilters;
};


#endif // NESORA_PARAMETRIC_IIRFILTER_H

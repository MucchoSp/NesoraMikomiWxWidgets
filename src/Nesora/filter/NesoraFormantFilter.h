#pragma once
#include <vector>
#include <cmath>

#include "NesoraFilter.h"

#ifndef NESORA_FORMANT_FILTER_H
#define NESORA_FORMANT_FILTER_H

struct NesoraFormantParam {
    double f1_frequency;
    double f1_bandwidth;
    double f1_amplitude;
};

class NesoraFormantFilter : public NesoraFilterBase {
public:

    NesoraFormantFilter(){}
    NesoraFormantFilter(double f1_frequency, double f1_bandwidth, double f1_amplitude) {
        AddParamater(f1_frequency, f1_bandwidth, f1_amplitude);
    }

    void AddParamater(double frequency, double bandwidth, double amplitude);
    std::vector<NesoraFormantParam>& GetParamater();
    const std::vector<NesoraFormantParam>& GetParamater() const;
    void SetParamater(const std::vector<NesoraFormantParam>&);

    std::vector<double> GetFilter(double dx, size_t N) const;

    void GenerateKernel();
    double Filter(double x) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    std::vector<NesoraFormantParam> paramaters;
    std::vector<double> kernel;
    std::vector<double> inputwave_buffer;

};

#endif // NESORA_FORMANT_FILTER_H
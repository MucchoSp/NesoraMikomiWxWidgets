#pragma once
#include <vector>
#include <cmath>


#ifndef NESORA_FILTER_H
#define NESORA_FILTER_H

class NesoraFilterBase {
public:
    NesoraFilterBase(){}

    virtual double Filter(double x) = 0;
private:


};


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

    double Filter(double x);

private:

    std::vector<NesoraFormantParam> paramaters;

};

#endif // NESORA_FILTER_H

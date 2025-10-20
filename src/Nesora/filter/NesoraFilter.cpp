#include "NesoraFilter.h"
#include <iostream>


void NesoraFormantFilter::AddParamater(double frequency, double bandwidth, double amplitude) {
    paramaters.push_back({frequency,bandwidth,amplitude});
}

const std::vector<NesoraFormantParam>& NesoraFormantFilter::GetParamater() const{
    return paramaters;
}

std::vector<NesoraFormantParam>& NesoraFormantFilter::GetParamater() {
    return paramaters;
}

void NesoraFormantFilter::SetParamater(const std::vector<NesoraFormantParam>& param) {
    paramaters = param;
}

std::vector<double> NesoraFormantFilter::GetFilter(double dx, size_t N) const {
    std::vector<double> out(N);
    for(size_t i = 0;i < N;i++) {
        out[i] = 0;
        for(size_t j = 0;j < paramaters.size();j++) {
            out[i] += paramaters[j].f1_amplitude * exp(-(0.5 * ((double)i * dx - paramaters[j].f1_frequency) * ((double)i * dx - paramaters[j].f1_frequency)) / (paramaters[j].f1_bandwidth * paramaters[j].f1_bandwidth));
        }
    }
    return out;
}

double NesoraFormantFilter::Filter(double x) {
    
    return 0.0;
}

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraParametricSOSIIRFilter.h"



void NesoraParametricSOFilter::Reset() {
    s1 = s2 = 0;
    b0 = b1 = b2 = 0;
    a1 = a2 = 0;
}

void NesoraParametricSOFilter::SetPoint(NesoraIIRFilterPD in_point) {
    point.r = in_point.r;
    point.theta = in_point.theta;
}

void NesoraParametricSOFilter::SetDelta(const std::vector<ParametricNesoraIIRFilterParameter>& in_delta) {
    delta = in_delta;
}

void NesoraParametricSOFilter::AddDelta(size_t in_delta_Index, ParametricNesoraIIRFilterParameter in_delta_value) {
    if (in_delta_Index < delta.size()) {
        delta[in_delta_Index] = in_delta_value;
    } else {
        delta.push_back(in_delta_value);
    }
}

void NesoraParametricSOFilter::SetDestinationPoint(size_t parameterIndex, NesoraIIRFilterPD in_point) {
    delta[parameterIndex].delta_r = in_point.r - point.r;
    delta[parameterIndex].delta_theta = in_point.theta - point.theta;
}

void NesoraParametricSOFilter::CalculateCoefficients(const ParametricNesoraDelta& parameters) {
    double r = point.r, theta = point.theta;

    for (const auto& param : parameters) {
        if (param.ID < delta.size()) {
            r += delta[param.ID].delta_r * param.delta;
            theta += delta[param.ID].delta_theta * param.delta;
        }
    }

    if(point.r < 0) {
        b0 = 1;
        b1 = 2 * r * cos(theta);
        b2 = r * r;
        a1 = 0;
        a2 = 0;
    } else {
        b0 = 1;
        b1 = 0;
        b2 = 0;
        a1 = -2 * r * cos(theta);
        a2 = r * r;
    }
}

double NesoraParametricSOFilter::CalculateFrequencyResponse(double omega) const {
    // z^{-1} = e^{-j omega}
    std::complex<double> z_inv = std::exp(std::complex<double>(0.0, -omega));

    std::complex<double> numerator = (b2 * z_inv + b1) * z_inv + b0;
    std::complex<double> denominator = (a2 * z_inv + a1) * z_inv + 1.0;

    double mag;
    if(std::abs(denominator) < 1e-300)
        return std::numeric_limits<double>::infinity(); // とてもおおきなあたい
    else
        return std::abs(numerator / denominator);
}

double NesoraParametricSOFilter::Filter(double x) {
    double y = b0 * x + s1;

    s1 = b1 * x - a1 * y + s2;
    s2 = b2 * x - a2 * y;

    return y;
}

double NesoraParametricSOFilter::Filter(const ParametricNesoraDelta& parameters, double x) {
    CalculateCoefficients(parameters);
    double y = b0 * x + s1;

    s1 = b1 * x - a1 * y + s2;
    s2 = b2 * x - a2 * y;

    return y;
}

NesoraIIRFilterPD NesoraParametricSOFilter::GetPoint() {
    return point;
}

const NesoraIIRFilterPD& NesoraParametricSOFilter::GetPoint() const {
    return point;
}

const NesoraIIRFilterPD NesoraParametricSOFilter::GetParametricPoint(const ParametricNesoraDelta& parameters) const {
    NesoraIIRFilterPD out = point;

    // for(const auto& param : parameters) {
    //     const auto& paramdelta = delta.find(param.ID);
    //     if (paramdelta != delta.end()) {
    //         out.r += paramdelta->second.delta_r * param.delta;
    //         out.theta += paramdelta->second.delta_theta * param.delta;
    //     }
    // }

    for(size_t i = 0; i < parameters.size(); ++i) {
        if (i <  delta.size()) {
            ParametricNesoraIIRFilterParameter paramdelta = delta[i];
            out.r += paramdelta.delta_r * parameters[i].delta;
            out.theta += paramdelta.delta_theta * parameters[i].delta;
        }
    }

    if (out.r < -1.0)
        out.r = -1.0;
    else if (out.r > 1.0)
        out.r = 1.0;

    if (out.theta < 0)
        out.theta = 0;
    else if (out.theta > nsPI)
        out.theta = nsPI;

    return out;
}

const NesoraIIRFilterPD NesoraParametricSOFilter::GetParametricPoint(const size_t parameterIndex, const double deltaValue) const {
    NesoraIIRFilterPD out = point;
    
    if (parameterIndex < delta.size()) {
        ParametricNesoraIIRFilterParameter paramdelta = delta[parameterIndex];
        out.r += paramdelta.delta_r * deltaValue;
        if (out.r < -1.0)
            out.r = -1.0;
        else if (out.r > 1.0)
            out.r = 1.0;
        
        out.theta += paramdelta.delta_theta * deltaValue;
        if (out.theta < 0)
            out.theta = 0;
        else if (out.theta > nsPI)
            out.theta = nsPI;
    }
    
    return out;
}

std::vector<ParametricNesoraIIRFilterParameter> NesoraParametricSOFilter::GetDelta() const {
    return delta;
}

const ParametricNesoraIIRFilterParameter NesoraParametricSOFilter::GetDelta(const size_t parameterIndex) const {
    if (parameterIndex < delta.size()) {
        return delta[parameterIndex];
    }
    return {0};
}





void NesoraParametricSOSIIRFilter::Reset() {
    response.clear();

    for(auto& filter : SOFilters)
        filter.Reset();
}

void NesoraParametricSOSIIRFilter::CalculateCoefficients(const ParametricNesoraDelta& parameters) {
    for(auto& filter : SOFilters) {
        filter.CalculateCoefficients(parameters);
    }
}

const std::vector<double>& NesoraParametricSOSIIRFilter::CalculateFrequencyResponse(int num_samples) {
    if (num_samples <= 0) {
        response.clear();
        return response;
    }
    response.assign(num_samples, 0.0);

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // デフォルト値

    auto worker = [&](int start, int end) {
        for (int n = start; n < end; n++) {
            double omega;
            if (num_samples == 1)
                omega = 0.0;
            else
                omega = nsPI * static_cast<double>(n) / static_cast<double>(num_samples - 1);

            double mag = 1.0;
            for(const auto& filter : SOFilters)
                mag *= filter.CalculateFrequencyResponse(omega);
            
            response[n] = std::log10(mag + 1e-10) * 10.0; // dBへの変換
        }
    };

    // スレッドを起動
    std::vector<std::thread> threads;
    int chunk_size = num_samples / num_threads;
    
    for (unsigned int t = 0; t < num_threads; t++) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? num_samples : (t + 1) * chunk_size;
        threads.emplace_back(worker, start, end);
    }

    // 全スレッドの完了を待つ
    for (auto& thread : threads) {
        thread.join();
    }

    return response;
}

const std::vector<double>& NesoraParametricSOSIIRFilter::GetResponse() const {
    return response;
}

double NesoraParametricSOSIIRFilter::Filter(const ParametricNesoraDelta& parameters, double x) {
    double y = x;

    for(auto& filter : SOFilters)
        y = filter.Filter(parameters, y);
    
    return y;
}

const std::vector<NesoraParametricSOFilter>& NesoraParametricSOSIIRFilter::GetSOFilter() const {
    return SOFilters;
}

std::vector<NesoraParametricSOFilter>& NesoraParametricSOSIIRFilter::GetSOFilter() {
    return SOFilters;
}



std::vector<unsigned char> NesoraParametricSOSIIRFilter::SaveData() {
    std::vector<unsigned char> data;

    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&samplingFrequency), reinterpret_cast<const unsigned char*>(&samplingFrequency) + sizeof(samplingFrequency));
    size_t soFilterSize = SOFilters.size();
    data.insert(data.end(), reinterpret_cast<const unsigned char*>(&soFilterSize), reinterpret_cast<const unsigned char*>(&soFilterSize) + sizeof(size_t));
    for(const auto& filter : SOFilters) {
        std::vector<unsigned char> filterData = filter.GetPoint().SaveData();
        size_t filterDataSize = filterData.size();
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&filterDataSize), reinterpret_cast<const unsigned char*>(&filterDataSize) + sizeof(size_t));
        data.insert(data.end(), filterData.begin(), filterData.end());
        std::vector<ParametricNesoraIIRFilterParameter> delta = filter.GetDelta();
        size_t deltaSize = delta.size();
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&deltaSize), reinterpret_cast<const unsigned char*>(&deltaSize) + sizeof(size_t));
        for(const auto& paramdelta : delta) {
            std::vector<unsigned char> deltaData = paramdelta.SaveData();
            size_t deltaDataSize = deltaData.size();
            data.insert(data.end(), reinterpret_cast<const unsigned char*>(&deltaDataSize), reinterpret_cast<const unsigned char*>(&deltaDataSize) + sizeof(size_t));
            data.insert(data.end(), deltaData.begin(), deltaData.end());
        }
    }

    return data;
};

void NesoraParametricSOSIIRFilter::LoadData(const std::vector<unsigned char>& data) {
    size_t offset = 0;

    if (data.size() < sizeof(samplingFrequency) + sizeof(size_t)) {
        // データが不十分
        return;
    }

    samplingFrequency = *reinterpret_cast<const double*>(data.data() + offset);
    offset += sizeof(samplingFrequency);

    size_t soFilterSize = *reinterpret_cast<const size_t*>(data.data() + offset);
    offset += sizeof(size_t);

    SOFilters.clear();
    for (size_t i = 0; i < soFilterSize; i++) {
        if (offset + sizeof(size_t) > data.size()) {
            // データが不十分
            return;
        }
        size_t filterDataSize = *reinterpret_cast<const size_t*>(data.data() + offset);
        offset += sizeof(size_t);

        if (offset + filterDataSize > data.size()) {
            // データが不十分
            return;
        }
        NesoraIIRFilterPD point;
        point.LoadData(std::vector<unsigned char>(data.begin() + offset, data.begin() + offset + filterDataSize));
        offset += filterDataSize;

        if (offset + sizeof(size_t) > data.size()) {
            // データが不十分
            return;
        }
        size_t deltaSize = *reinterpret_cast<const size_t*>(data.data() + offset);
        offset += sizeof(size_t);

        std::vector<ParametricNesoraIIRFilterParameter> delta;
        for (size_t j = 0; j < deltaSize; j++) {
            if (offset + sizeof(uint32_t) > data.size()) {
                // データが不十分
                return;
            }
            uint32_t paramid = *reinterpret_cast<const uint32_t*>(data.data() + offset);
            offset += sizeof(uint32_t);

            if (offset + sizeof(size_t) > data.size()) {
                // データが不十分
                return;
            }
            size_t deltaDataSize = *reinterpret_cast<const size_t*>(data.data() + offset);
            offset += sizeof(size_t);

            if (offset + deltaDataSize > data.size()) {
                // データが不十分
                return;
            }
            ParametricNesoraIIRFilterParameter paramdelta;
            paramdelta.LoadData(std::vector<unsigned char>(data.begin() + offset, data.begin() + offset + deltaDataSize));
            offset += deltaDataSize;
            delta.push_back(paramdelta);
        }
        SOFilters.emplace_back(NesoraParametricSOFilter(point));
        SOFilters.back().SetDelta(delta);
    }

};



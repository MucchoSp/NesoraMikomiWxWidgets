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

void NesoraParametricSOFilter::SetDelta(const std::map<uint32_t, ParametricNesoraIIRFilterParameter>& in_delta) {
    delta = in_delta;
}

void NesoraParametricSOFilter::AddDelta(uint32_t in_delta_ID, ParametricNesoraIIRFilterParameter in_delta_value) {
    delta[in_delta_ID] = in_delta_value;
}

void NesoraParametricSOFilter::SetDestinationPoint(uint32_t parameterID, NesoraIIRFilterPD in_point) {
    delta[parameterID].delta_r = in_point.r - point.r;
    delta[parameterID].delta_theta = in_point.theta - point.theta;
}

void NesoraParametricSOFilter::CalculateCoefficients(const std::map<uint32_t, double>& parameters) {
    double r = point.r, theta = point.theta;

    for(const auto& [paramid, param] : parameters) {
        if(delta.count(paramid)) {
            r += delta.at(paramid).delta_r * param;
            theta += delta.at(paramid).delta_theta * param;
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

NesoraIIRFilterPD NesoraParametricSOFilter::GetPoint() {
    return point;
}

const NesoraIIRFilterPD& NesoraParametricSOFilter::GetPoint() const {
    return point;
}

const NesoraIIRFilterPD NesoraParametricSOFilter::GetParametricPoint(const std::map<uint32_t, double>& parameters) const {
    NesoraIIRFilterPD out = point;

    for(const auto& [paramid, param] : parameters) {
        if (delta.count(paramid)) {
            out.r += delta.at(paramid).delta_r * param;
            out.theta += delta.at(paramid).delta_theta * param;
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

const NesoraIIRFilterPD NesoraParametricSOFilter::GetParametricPoint(const uint32_t paramid, const double param) const {
    NesoraIIRFilterPD out = point;
    
    try {
        out.r += delta.at(paramid).delta_r * param;
        if (out.r < -1.0)
            out.r = -1.0;
        else if (out.r > 1.0)
            out.r = 1.0;
        
        out.theta += delta.at(paramid).delta_theta * param;
        if (out.theta < 0)
            out.theta = 0;
        else if (out.theta > nsPI)
            out.theta = nsPI;
    }
    catch(std::out_of_range&) {
        out = point;
    }
    
    return out;
}

std::map<uint32_t, ParametricNesoraIIRFilterParameter> NesoraParametricSOFilter::GetDelta() {
    return delta;
}

const ParametricNesoraIIRFilterParameter NesoraParametricSOFilter::GetDelta(const uint32_t parameterID) const {
    try {
        return delta.at(parameterID);
    }
    catch(std::out_of_range&) {
        return {0};
    }
}





void NesoraParametricSOSIIRFilter::Reset() {
    response.clear();

    for(auto& filter : SOFilters)
        filter.Reset();
}

void NesoraParametricSOSIIRFilter::CalculateCoefficients(const std::map<uint32_t, double>& parameters) {
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

double NesoraParametricSOSIIRFilter::Filter(double x) {
    double y = x;

    for(auto& filter : SOFilters)
        y = filter.Filter(y);
    
    return y;
}

const std::vector<NesoraParametricSOFilter>& NesoraParametricSOSIIRFilter::GetSOFilter() const {
    return SOFilters;
}

std::vector<NesoraParametricSOFilter>& NesoraParametricSOSIIRFilter::GetSOFilter() {
    return SOFilters;
}



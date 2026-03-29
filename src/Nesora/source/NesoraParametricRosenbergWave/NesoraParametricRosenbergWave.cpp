#include "NesoraParametricRosenbergWave.h"

double NesoraParametricRosenbergWave::Utterance(double radian) {
    double noise_input = (double)std::rand() / (double)RAND_MAX;
    if (radian < t1)
        return ((3.0 * std::pow(radian / t1, 2.0) - 2.0 * std::pow(radian / t1, 3.0)) + a0) * (1.0 - noise) + noise_input * noise;
    else if (radian < t2)
        return ((1.0 - std::pow((radian - t1) / (t2 - t1), 2.0)) + a0) * (1.0 - noise) + noise_input * noise;
    else
        return a0 * (1.0 - noise) + noise_input * noise;
}

void NesoraParametricRosenbergWave::SetParamater(double tau1, double tau2, double a0, double noise_level) {
    normal_t1 = tau1;
    normal_t2 = tau2;
    normal_a0 = a0;
    normal_noise = noise_level;
}

void NesoraParametricRosenbergWave::SetParamater(const std::map<uint32_t, double>& parameters) {
    t1 = normal_t1;
    t2 = normal_t2;
    a0 = normal_a0;
    noise = normal_noise;
    
    for(const auto& [paramid, param] : parameters) {
        const auto& paramdelta = delta.find(paramid);
        if (paramdelta != delta.end()) {
            t1 += paramdelta->second.delta_tau1 * param;
            t2 += paramdelta->second.delta_tau2 * param;
            a0 += paramdelta->second.delta_a0 * param;
            noise += paramdelta->second.delta_noise * param;
        }
    }

    if (t1 < 0.0) t1 = 0.0;
    if (t1 > 1.0) t1 = 1.0;
    if (t2 < 0.0) t2 = 0.0;
    if (t2 > 1.0) t2 = 1.0;
    if (noise < 0.0) noise = 0.0;
    if (noise > 1.0) noise = 1.0;
}

void NesoraParametricRosenbergWave::SetDelta(const std::map<uint32_t, ParametricNesoraRosenbergWaveParameter>& in_delta) {
    delta = in_delta;
}

void NesoraParametricRosenbergWave::AddDelta(uint32_t in_delta_ID, ParametricNesoraRosenbergWaveParameter in_delta_value) {
    delta[in_delta_ID] = in_delta_value;
}

void NesoraParametricRosenbergWave::SetA0(double a0) {
    normal_a0 = a0;
}

double NesoraParametricRosenbergWave::GetT1() const {
    return normal_t1;
}

double NesoraParametricRosenbergWave::GetT2() const {
    return normal_t2;
}

double NesoraParametricRosenbergWave::GetA0() const {
    return normal_a0;
}

double NesoraParametricRosenbergWave::GetNoise() const {
    return normal_noise;
}

const NesoraRosenbergParameter NesoraParametricRosenbergWave::GetParametricSource(const std::map<uint32_t, double>& parameters) const {
    NesoraRosenbergParameter out;
    out.tau1 = normal_t1;
    out.tau2 = normal_t2;
    out.a0 = normal_a0;
    out.noise = normal_noise;

    for(const auto& [paramid, param] : parameters) {
        const auto& paramdelta = delta.find(paramid);
        if (paramdelta != delta.end()) {
            out.tau1 += paramdelta->second.delta_tau1 * param;
            out.tau2 += paramdelta->second.delta_tau2 * param;
            out.a0 += paramdelta->second.delta_a0 * param;
            out.noise += paramdelta->second.delta_noise * param;
        }
    }

    if (out.tau1 < 0.0) out.tau1 = 0.0;
    if (out.tau1 > 1.0) out.tau1 = 1.0;
    if (out.tau2 < 0.0) out.tau2 = 0.0;
    if (out.tau2 > 1.0) out.tau2 = 1.0;
    if (out.noise < 0.0) out.noise = 0.0;
    if (out.noise > 1.0) out.noise = 1.0;

    return out;
}

const NesoraRosenbergParameter NesoraParametricRosenbergWave::GetParametricSource(const uint32_t parameterID, const double in_delta) const {
    NesoraRosenbergParameter out;
    out.tau1 = normal_t1;
    out.tau2 = normal_t2;
    out.a0 = normal_a0;
    out.noise = normal_noise;

    const auto& paramdelta = delta.find(parameterID);
    if (paramdelta != delta.end()) {
        out.tau1 += paramdelta->second.delta_tau1 * in_delta;
        out.tau2 += paramdelta->second.delta_tau2 * in_delta;
        out.a0 += paramdelta->second.delta_a0 * in_delta;
        out.noise += paramdelta->second.delta_noise * in_delta;
    }

    if (out.tau1 < 0.0) out.tau1 = 0.0;
    if (out.tau1 > 1.0) out.tau1 = 1.0;
    if (out.tau2 < 0.0) out.tau2 = 0.0;
    if (out.tau2 > 1.0) out.tau2 = 1.0;
    if (out.noise < 0.0) out.noise = 0.0;
    if (out.noise > 1.0) out.noise = 1.0;

    return out;
}


std::vector<unsigned char> NesoraParametricRosenbergWave::SaveData() {
    std::vector<unsigned char> data;

    data.insert(data.end(), reinterpret_cast<unsigned char*>(&t1), reinterpret_cast<unsigned char*>(&t1) + sizeof(double));
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&t2), reinterpret_cast<unsigned char*>(&t2) + sizeof(double));
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&a0), reinterpret_cast<unsigned char*>(&a0) + sizeof(double));
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&noise), reinterpret_cast<unsigned char*>(&noise) + sizeof(double));

    return data;
}

void NesoraParametricRosenbergWave::LoadData(const std::vector<unsigned char>& data) {
    const size_t needed = sizeof(double) * 4;
    if (data.size() < needed) {
        std::cout << "RosenbergWave::LoadData: insufficient data (" << data.size() << " bytes)" << std::endl;
        return;
    }

    size_t offset = 0;
    std::memcpy(&t1, data.data() + offset, sizeof(double));
    offset += sizeof(double);
    std::memcpy(&t2, data.data() + offset, sizeof(double));
    offset += sizeof(double);
    std::memcpy(&a0, data.data() + offset, sizeof(double));
    offset += sizeof(double);
    std::memcpy(&noise, data.data() + offset, sizeof(double));
}


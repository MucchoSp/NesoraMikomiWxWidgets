#include "NesoraSource.h"

double NesoraRosenbergWave::Utterance(double radian) {
    double noise_input = (double)std::rand() / (double)RAND_MAX;
    if (radian < t1)
        return ((3.0 * std::pow(radian / t1, 2.0) - 2.0 * std::pow(radian / t1, 3.0)) + a0) * (1.0 - noise) + noise_input * noise;
    else if (radian < t2)
        return ((1.0 - std::pow((radian - t1) / (t2 - t1), 2.0)) + a0) * (1.0 - noise) + noise_input * noise;
    else
        return a0 * (1.0 - noise) + noise_input * noise;
}

void NesoraRosenbergWave::SetParamater(double tau1, double tau2, double a0, double noise_level) {
    t1 = tau1;
    t2 = tau2;
    this->a0 = a0;
    this->noise = noise_level;
}

double NesoraRosenbergWave::GetT1() const {
    return t1;
}

double NesoraRosenbergWave::GetT2() const {
    return t2;
}

double NesoraRosenbergWave::GetA0() const {
    return a0;
}

double NesoraRosenbergWave::GetNoise() const {
    return noise;
}

std::vector<unsigned char> NesoraRosenbergWave::SaveData() {
    std::vector<unsigned char> data;

    data.insert(data.end(), reinterpret_cast<unsigned char*>(&t1), reinterpret_cast<unsigned char*>(&t1) + sizeof(double));
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&t2), reinterpret_cast<unsigned char*>(&t2) + sizeof(double));
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&a0), reinterpret_cast<unsigned char*>(&a0) + sizeof(double));
    data.insert(data.end(), reinterpret_cast<unsigned char*>(&noise), reinterpret_cast<unsigned char*>(&noise) + sizeof(double));

    return data;
}

void NesoraRosenbergWave::LoadData(const std::vector<unsigned char>& data) {
    const size_t needed = sizeof(double) * 3;
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


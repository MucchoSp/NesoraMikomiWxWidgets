#include "NesoraVoice.h"

void NesoraMikomiVoice::SetSource(NesoraSourceBase* src) {
    source = src;
}

void NesoraMikomiVoice::SetFilter(NesoraFilterBase* flt) {
    filter = flt;
}

double NesoraMikomiVoice::Synthesize(double radian) {
    if (source == nullptr)
        return 0.0;
    
    double sample = source->Utterance(radian);
    
    if (filter != nullptr) {
        sample = filter->Filter(sample);
    }

    return sample;
}

double NesoraMikomiVoice::Synthesize(double frequency, double samplingFrequency)  {
    radian = std::fmod(radian + 2.0 * nsPI * frequency / samplingFrequency, 2.0 * nsPI);
    return Synthesize(radian);
}

void NesoraMikomiVoice::SaveVoiceData(const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) {
        return;
    }
    // Save source data
    std::vector<unsigned char> sourceData = source->SaveData();
    size_t sourceSize = sourceData.size();
    ofs.write(reinterpret_cast<const char*>(&sourceSize), sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(sourceData.data()), sourceSize);

    // Save filter data
    std::vector<unsigned char> filterData = filter->SaveData();
    size_t filterSize = filterData.size();
    ofs.write(reinterpret_cast<const char*>(&filterSize), sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(filterData.data()), filterSize);

    ofs.close();
}

void NesoraMikomiVoice::LoadVoiceData(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {
        return;
    }
    // Load source data
    size_t sourceSize;
    ifs.read(reinterpret_cast<char*>(&sourceSize), sizeof(size_t));
    std::vector<unsigned char> sourceData(sourceSize);
    ifs.read(reinterpret_cast<char*>(sourceData.data()), sourceSize);
    source->LoadData(sourceData);

    // Load filter data
    size_t filterSize;
    ifs.read(reinterpret_cast<char*>(&filterSize), sizeof(size_t));
    std::vector<unsigned char> filterData(filterSize);
    ifs.read(reinterpret_cast<char*>(filterData.data()), filterSize);
    filter->LoadData(filterData);

    ifs.close();
}


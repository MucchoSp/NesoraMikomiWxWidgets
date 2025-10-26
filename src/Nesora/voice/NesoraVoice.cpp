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

std::vector<unsigned char> NesoraMikomiVoice::GetVoiceData() {
    std::vector<unsigned char> fileData;

    // Save source data
    std::vector<unsigned char> sourceData = source->SaveData();
    size_t sourceSize = sourceData.size();
    fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(&sourceSize), reinterpret_cast<const unsigned char*>(&sourceSize) + sizeof(size_t));
    fileData.insert(fileData.end(), sourceData.begin(), sourceData.end());

    // Save filter data
    std::vector<unsigned char> filterData = filter->SaveData();
    size_t filterSize = filterData.size();
    fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(&filterSize), reinterpret_cast<const unsigned char*>(&filterSize) + sizeof(size_t));
    fileData.insert(fileData.end(), filterData.begin(), filterData.end());

    return fileData;
}

void NesoraMikomiVoice::LoadVoiceData(const std::vector<unsigned char>& fileData) {
    size_t sourceSize = *reinterpret_cast<const size_t*>(fileData.data());
    std::cout << "Source Size: " << sourceSize << std::endl;
    std::vector<unsigned char> sourceData(fileData.begin() + sizeof(size_t), fileData.begin() + sizeof(size_t) + sourceSize);
    source->LoadData(sourceData);

    size_t filterSize = *reinterpret_cast<const size_t*>(fileData.data() + sizeof(size_t) + sourceSize);
    std::cout << "Filter Size: " << filterSize << std::endl;
    std::vector<unsigned char> filterData(fileData.begin() + sizeof(size_t) + sourceSize + sizeof(size_t), fileData.end());
    filter->LoadData(filterData);
}


#include "NesoraVoice.h"
#include <cstdint>
#include <cstring>

// File format header
static const char NESORA_MAGIC[8] = {'N','S','V','O','D','A','T','A'}; // 8-byte magic
static const uint32_t NESORA_VERSION = 1;

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
    // Write magic + version
    ofs.write(NESORA_MAGIC, sizeof(NESORA_MAGIC));
    uint32_t ver = NESORA_VERSION;
    ofs.write(reinterpret_cast<const char*>(&ver), sizeof(ver));

    // Save source data
    std::vector<unsigned char> sourceData;
    if (source) sourceData = source->SaveData();
    size_t sourceSize = sourceData.size();
    ofs.write(reinterpret_cast<const char*>(&sourceSize), sizeof(size_t));
    if (sourceSize > 0) ofs.write(reinterpret_cast<const char*>(sourceData.data()), sourceSize);

    // Save filter data
    std::vector<unsigned char> filterData;
    if (filter) filterData = filter->SaveData();
    size_t filterSize = filterData.size();
    ofs.write(reinterpret_cast<const char*>(&filterSize), sizeof(size_t));
    if (filterSize > 0) ofs.write(reinterpret_cast<const char*>(filterData.data()), filterSize);

    ofs.close();
}

void NesoraMikomiVoice::LoadVoiceData(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {
        return;
    }
    // Read and validate magic + version
    char magic[sizeof(NESORA_MAGIC)];
    ifs.read(magic, sizeof(magic));
    if (ifs.gcount() != sizeof(magic) || std::memcmp(magic, NESORA_MAGIC, sizeof(magic)) != 0) {
        std::cerr << "LoadVoiceData(filename): invalid file magic" << std::endl;
        return;
    }
    uint32_t ver = 0;
    ifs.read(reinterpret_cast<char*>(&ver), sizeof(ver));
    if (ifs.gcount() != sizeof(ver) || ver != NESORA_VERSION) {
        std::cerr << "LoadVoiceData(filename): unsupported version " << ver << std::endl;
        return;
    }

    // Load source data
    size_t sourceSize = 0;
    ifs.read(reinterpret_cast<char*>(&sourceSize), sizeof(size_t));
    if (!ifs) { std::cerr << "LoadVoiceData(filename): failed to read sourceSize" << std::endl; return; }
    std::vector<unsigned char> sourceData(sourceSize);
    if (sourceSize > 0) {
        ifs.read(reinterpret_cast<char*>(sourceData.data()), sourceSize);
        if (!ifs) { std::cerr << "LoadVoiceData(filename): failed to read sourceData" << std::endl; return; }
    }
    if (source) source->LoadData(sourceData);

    // Load filter data
    size_t filterSize = 0;
    ifs.read(reinterpret_cast<char*>(&filterSize), sizeof(size_t));
    if (!ifs) { std::cerr << "LoadVoiceData(filename): failed to read filterSize" << std::endl; return; }
    std::vector<unsigned char> filterData(filterSize);
    if (filterSize > 0) {
        ifs.read(reinterpret_cast<char*>(filterData.data()), filterSize);
        if (!ifs) { std::cerr << "LoadVoiceData(filename): failed to read filterData" << std::endl; return; }
    }
    if (filter) filter->LoadData(filterData);

    ifs.close();
}

std::vector<unsigned char> NesoraMikomiVoice::GetVoiceData() {
    std::vector<unsigned char> fileData;
    // Prepend magic + version
    fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(NESORA_MAGIC), reinterpret_cast<const unsigned char*>(NESORA_MAGIC) + sizeof(NESORA_MAGIC));
    uint32_t ver = NESORA_VERSION;
    fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(&ver), reinterpret_cast<const unsigned char*>(&ver) + sizeof(ver));
    // Save source data (guard nullptr)
    if (source) {
        std::vector<unsigned char> sourceData = source->SaveData();
        size_t sourceSize = sourceData.size();
        fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(&sourceSize), reinterpret_cast<const unsigned char*>(&sourceSize) + sizeof(size_t));
        fileData.insert(fileData.end(), sourceData.begin(), sourceData.end());
    } else {
        size_t zero = 0;
        fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(&zero), reinterpret_cast<const unsigned char*>(&zero) + sizeof(size_t));
    }

    // Save filter data (guard nullptr)
    if (filter) {
        std::vector<unsigned char> filterData = filter->SaveData();
        size_t filterSize = filterData.size();
        fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(&filterSize), reinterpret_cast<const unsigned char*>(&filterSize) + sizeof(size_t));
        fileData.insert(fileData.end(), filterData.begin(), filterData.end());
    } else {
        size_t zero = 0;
        fileData.insert(fileData.end(), reinterpret_cast<const unsigned char*>(&zero), reinterpret_cast<const unsigned char*>(&zero) + sizeof(size_t));
    }

    return fileData;
}

void NesoraMikomiVoice::LoadVoiceData(const std::vector<unsigned char>& fileData) {
    size_t offset = 0;
    const size_t total = fileData.size();

    // Validate header
    if (total < sizeof(NESORA_MAGIC) + sizeof(uint32_t)) {
        std::cerr << "LoadVoiceData: fileData too small for header" << std::endl;
        return;
    }
    if (std::memcmp(fileData.data(), NESORA_MAGIC, sizeof(NESORA_MAGIC)) != 0) {
        std::cerr << "LoadVoiceData: invalid magic" << std::endl;
        return;
    }
    offset += sizeof(NESORA_MAGIC);
    uint32_t ver = 0;
    std::memcpy(&ver, fileData.data() + offset, sizeof(ver));
    offset += sizeof(ver);
    if (ver != NESORA_VERSION) {
        std::cerr << "LoadVoiceData: unsupported version " << ver << std::endl;
        return;
    }

    auto read_size = [&](size_t &out) -> bool {
        if (offset + sizeof(size_t) > total) return false;
        std::memcpy(&out, fileData.data() + offset, sizeof(size_t));
        offset += sizeof(size_t);
        return true;
    };

    size_t sourceSize = 0;
    if (!read_size(sourceSize)) { std::cerr << "LoadVoiceData: insufficient data for sourceSize" << std::endl; return; }
    if (offset + sourceSize > total) { std::cerr << "LoadVoiceData: sourceSize exceeds fileData size" << std::endl; return; }
    std::vector<unsigned char> sourceData;
    if (sourceSize > 0) sourceData.assign(fileData.begin() + offset, fileData.begin() + offset + sourceSize);
    offset += sourceSize;
    if (source) source->LoadData(sourceData); else std::cerr << "LoadVoiceData: source is null, skipping source load" << std::endl;

    size_t filterSize = 0;
    if (!read_size(filterSize)) { std::cerr << "LoadVoiceData: insufficient data for filterSize" << std::endl; return; }
    if (offset + filterSize > total) { std::cerr << "LoadVoiceData: filterSize exceeds fileData size" << std::endl; return; }
    std::vector<unsigned char> filterData;
    if (filterSize > 0) filterData.assign(fileData.begin() + offset, fileData.begin() + offset + filterSize);
    offset += filterSize;
    if (filter) filter->LoadData(filterData); else std::cerr << "LoadVoiceData: filter is null, skipping filter load" << std::endl;
}


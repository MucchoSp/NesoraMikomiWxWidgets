#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <iostream>

#include "../Nesora.h"

#ifndef NESORA_VOICE_H
#define NESORA_VOICE_H


class NesoraVoice {
public:
    NesoraVoice() {}
    ~NesoraVoice() {}
private:
};

class NesoraMikomiVoice : public NesoraVoice {
public:
    NesoraMikomiVoice() {}
    NesoraMikomiVoice(NesoraSourceBase* src, NesoraFilterBase* flt) : source(src), filter(flt) {}
    ~NesoraMikomiVoice() {}

    void SetSource(NesoraSourceBase* src);
    void SetFilter(NesoraFilterBase* flt);

    double Synthesize(double radian);
    double Synthesize(double frequency, double samplingFrequency);
    double GetRadian() const { return radian; }
    void SetRadian(double r) { radian = r; }

    void SaveVoiceData(const std::string& filename);
    void LoadVoiceData(const std::string& filename);

    std::vector<unsigned char> GetVoiceData();
    void LoadVoiceData(const std::vector<unsigned char>& fileData);

    NesoraSourceBase* GetSource() const { return source; }
    NesoraFilterBase* GetFilter() const { return filter; }

private:

    NesoraSourceBase* source;
    NesoraFilterBase* filter;

    double radian = 0.0;

};

#endif // NESORA_VOICE_H
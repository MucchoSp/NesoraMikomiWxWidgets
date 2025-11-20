#include "NesoraScriptPlay.h"

// MARK:NesoraScriptPlay

double NesoraScriptPlay::DeltaRadian(double t) {
    return ns2PI / pitch / samplingFrequency;
}

double NesoraScriptPlay::Volume(double t) {
    return volume;
}

nesoraVowelVector NesoraScriptPlay::Vowel(double t) {
    return vowel;
}

void NesoraScriptPlay::SetPitch(double p) {
    pitch = p;
}

void NesoraScriptPlay::SetVolume(double v) {
    volume = v;
}

void NesoraScriptPlay::SetVowel(nesoraVowelVector v) {
    vowel = v;
}


std::vector<unsigned char> NesoraScriptPlay::SaveData() {
    std::vector<unsigned char> out;
    return out;
}

void NesoraScriptPlay::LoadData(const std::vector<unsigned char>& data) {

}


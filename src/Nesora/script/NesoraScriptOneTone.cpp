#include "NesoraScriptOneTone.h"

// MARK:NesoraScriptOneTone

double NesoraScriptOneTone::DeltaRadian(double t) {
    return 0;
}

double NesoraScriptOneTone::Volume(double t) {
    return 0;
}

nesoraVowelVector NesoraScriptOneTone::Vowel(double t) {
    return {};
}


std::vector<unsigned char> NesoraScriptOneTone::SaveData() {
    std::vector<unsigned char> out;
    return out;
}

void NesoraScriptOneTone::LoadData(const std::vector<unsigned char>& data) {

}


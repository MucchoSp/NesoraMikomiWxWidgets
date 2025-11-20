#pragma once
#include <vector>


#ifndef NESORA_SOURCE_H
#define NESORA_SOURCE_H

class NesoraSourceBase {
public:
    NesoraSourceBase(){}

    virtual double Utterance(double radian) = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

private:
};

#endif // NESORA_SOURCE_H

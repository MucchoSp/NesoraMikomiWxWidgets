#pragma once
#include <vector>
#include <cmath>

#include "../NesoraDefines.h"

#ifndef NESORA_FILTER_H
#define NESORA_FILTER_H

class NesoraFilterBase {
public:
    NesoraFilterBase(){}

    virtual double Filter(double x) = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

private:
};

#endif // NESORA_FILTER_H

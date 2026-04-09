// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>
#include <map>
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

class NesoraParametricFilterBase {
public:
    NesoraParametricFilterBase(){}

    virtual double Filter(const std::map<uint32_t, double>& parameters, double x) = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

private:
};


struct NesoraIIRFilterPD {
    double r;
    double theta;

    std::vector<unsigned char> SaveData() const {
        std::vector<unsigned char> data;
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&r), reinterpret_cast<const unsigned char*>(&r) + sizeof(r));
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&theta), reinterpret_cast<const unsigned char*>(&theta) + sizeof(theta));
        return data;
    }
};


#endif // NESORA_FILTER_H

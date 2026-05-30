// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>
#include <map>

#include "../ParametricNesroaDefines.h"

#ifndef NESORA_DICTIONALY_H
#define NESORA_DICTIONALY_H

class NesoraDictionalyBase {
public:
    NesoraDictionalyBase(){}

    virtual std::map<uint32_t, ParametricNesoraParameter> Vowel(double t) = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

private:
};

#endif //NESORA_DICTIONALY_H

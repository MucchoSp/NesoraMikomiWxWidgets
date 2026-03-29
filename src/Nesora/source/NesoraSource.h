// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
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

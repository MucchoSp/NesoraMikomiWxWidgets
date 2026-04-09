// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>

#include "NesoraSource.h"

#ifndef NESORA_SIN_SOURCE_H
#define NESORA_SIN_SOURCE_H

class NesoraSinSource : public NesoraSourceBase {
public:
    NesoraSinSource(){}

    double Utterance(double radian) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:
};

#endif // NESORA_SIN_SOURCE_H

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
# pragma once

#include "NesoraFilter.h"

#ifndef NESORA_THROUGH_FILTER_H
#define NESORA_THROUGH_FILTER_H

class NesoraThroughFilter : public NesoraFilterBase {
public:
    NesoraThroughFilter(){}

    double Filter(double x) override {
        return x; // 入力をそのまま出力する
    }

    std::vector<unsigned char> SaveData() override {
        // データがないので、空のベクターを返す
        return std::vector<unsigned char>();
    }
    void LoadData(const std::vector<unsigned char>& data) override {
        // データがないので、特に何もしない
    }

private:
};

#endif // NESORA_THROUGH_FILTER_H

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraSinSource.h"

double NesoraSinSource::Utterance(double radian) {
    return std::sin(radian);
}

std::vector<unsigned char> NesoraSinSource::SaveData() {
    // サイン波はパラメータがないので、空のデータを返す
    return std::vector<unsigned char>();
}

void NesoraSinSource::LoadData(const std::vector<unsigned char>& data) {
    // サイン波はパラメータがないので、特に何もしない
}
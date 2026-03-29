// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_DEFINES_H
#define NESORA_DEFINES_H

#include <stdint.h>

constexpr double nsPI = 3.14159265358979323;
constexpr double ns2PI = nsPI * 2.0;
constexpr double ns1_PI = 1.0 / nsPI;
constexpr double ns1_2PI = 1.0 / ns2PI;

constexpr double nsE = 2.71828182845904523;

constexpr int NesoraDefaultSamplingFrequency = 48000;
constexpr int NesoraDefaultNyquistFrequency = NesoraDefaultSamplingFrequency / 2;

constexpr int nsPARAMETRIC_ID_MIN = 0;
constexpr int nsPARAMETRIC_ID_MAX = UINT32_MAX;

#endif //NESORA_DEFINES_H

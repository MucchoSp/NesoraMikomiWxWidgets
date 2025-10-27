#pragma once

#ifndef NESORA_DEFINES_H
#define NESORA_DEFINES_H

constexpr double nsPI = 3.14159265358979323;
constexpr double ns2PI = nsPI * 2.0;
constexpr double ns1_PI = 1.0 / nsPI;
constexpr double ns1_2PI = 1.0 / ns2PI;

constexpr int NesoraDefaultSamplingFrequency = 48000;
constexpr int NesoraDefaultNyquistFrequency = NesoraDefaultSamplingFrequency / 2;


#endif //NESORA_DEFINES_H

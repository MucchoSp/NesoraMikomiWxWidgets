#include "NesoraSource.h"

double NesoraRosenbergWave::Utterance(double radian) {
    if (radian < t1)
        return (3.0 * std::pow(radian / t1, 2.0) - 2.0 * std::pow(radian / t1, 3.0)) + a0;
    else if (radian < t1 + t2)
        return (1.0 - std::pow((radian - t1) / t2, 2.0)) + a0;
    else
        return a0;
}

void NesoraRosenbergWave::SetParamater(double tau1, double tau2, double a0) {
    t1 = tau1;
    t2 = tau2;
    this->a0 = a0;
}


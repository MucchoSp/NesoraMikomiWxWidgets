#pragma once
#include <vector>
#include <cmath>


#ifndef NESORA_SOURCE_H
#define NESORA_SOURCE_H

class NesoraSourceBase {
public:
    NesoraSourceBase(){}

    virtual double Utterance(double radian) = 0;
private:


};

class NesoraRosenbergWave : public NesoraSourceBase {
public:

    NesoraRosenbergWave(){}
    NesoraRosenbergWave(double tau1, double tau2, double a0) {
        SetParamater(tau1, tau2, a0);
    }

    void SetParamater(double tau1, double tau2, double a0);

    double Utterance(double radian);

private:

    double t1 = 0.0, t2 = 0.0, a0 = 0.0;

};

#endif // NESORA_SOURCE_H

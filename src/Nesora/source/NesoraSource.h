#pragma once
#include <vector>
#include <cmath>
#include <iostream>


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

class NesoraRosenbergWave : public NesoraSourceBase {
public:

    NesoraRosenbergWave(){}
    NesoraRosenbergWave(double tau1, double tau2, double a0) {
        SetParamater(tau1, tau2, a0);
    }

    void SetParamater(double tau1, double tau2, double a0);
    double GetT1() const;
    double GetT2() const;
    double GetA0() const;

    double Utterance(double radian) override;

    std::vector<unsigned char> SaveData() override;
    void LoadData(const std::vector<unsigned char>& data) override;

private:

    double t1 = 0.0, t2 = 0.0, a0 = -0.5;

};

#endif // NESORA_SOURCE_H

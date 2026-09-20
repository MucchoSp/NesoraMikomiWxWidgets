// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>
#include <map>

#include "../ParametricNesroaDefines.h"
#include "../script/NesoraScript.h"

#ifndef NESORA_DICTIONALY_H
#define NESORA_DICTIONALY_H

class NesoraDictionalyBase {
public:
    NesoraDictionalyBase(){}
    
    virtual int GetWord(int idx, ParametricNesoraParameterValue* outputParameters, double* outputPitch, double* outputEnvelope) = 0;

    virtual int SetScript(NesoraScriptBase* script) = 0;
    virtual NesoraScriptBase* GetScript() = 0;
    virtual int RefreshCache() = 0;

    virtual std::vector<unsigned char> SaveData() = 0;
    virtual void LoadData(const std::vector<unsigned char>& data) = 0;

protected:
    
    NesoraScriptBase* script = nullptr;

private:
};


#endif //NESORA_DICTIONALY_H

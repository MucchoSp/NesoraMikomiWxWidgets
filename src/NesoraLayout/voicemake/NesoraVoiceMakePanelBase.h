// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_VOICE_MAKE_PANEL_BASE_H
#define NESORA_VOICE_MAKE_PANEL_BASE_H

#include <wx/wx.h>

#include "../../Nesora/Nesora.h"


class nsSourcePanelBase : public wxPanel {
public:
    nsSourcePanelBase() {
    }
    nsSourcePanelBase(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
    }

    ~nsSourcePanelBase() {
    }

    virtual void Init() = 0;
    virtual void Update() = 0;
    
    virtual NesoraSourceBase* GetSource() = 0;

    virtual double GetPitch() const = 0;
};


class nsFilterPanelBase : public wxPanel {
public:
    nsFilterPanelBase() {
    }
    nsFilterPanelBase(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
    }
    ~nsFilterPanelBase() {
        
    }
    virtual void Init() = 0;
    virtual void Update() = 0;

    virtual NesoraFilterBase* GetFilter() = 0;
};

#endif // NESORA_VOICE_MAKE_PANEL_BASE_H
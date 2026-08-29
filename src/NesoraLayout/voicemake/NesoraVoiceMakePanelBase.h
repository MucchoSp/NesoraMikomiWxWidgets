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


class nsParametricFilterPanelBase : public wxPanel {
public:
    nsParametricFilterPanelBase() {
    }
    nsParametricFilterPanelBase(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
    }
    ~nsParametricFilterPanelBase() {
    }

    virtual void Init() = 0;
    virtual void Update() = 0;

    virtual void SetParameter(ParametricNesoraDelta* parameters) = 0;

    virtual NesoraParametricFilterBase* GetFilter() = 0;
};

class nsVoiceMakePanelBase : public wxPanel {
public:
    nsVoiceMakePanelBase() {
    }
    nsVoiceMakePanelBase(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
    }
    ~nsVoiceMakePanelBase() {
    }

    void SetVoice(NesoraMikomiVoice* voice) { this->voice = voice; }
    NesoraMikomiVoice* GetVoice() const { return voice; }
    virtual void OnSave(wxCommandEvent& event) = 0;
    virtual void OnOpen(wxCommandEvent& event) = 0;

    virtual void PanelEnable() = 0;
    virtual void PanelDisable() = 0;

protected:
    NesoraMikomiVoice* voice = nullptr;

};

class nsParametricVoiceMakePanelBase : public wxPanel {
public:
    nsParametricVoiceMakePanelBase() {
    }
    nsParametricVoiceMakePanelBase(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
    }
    ~nsParametricVoiceMakePanelBase() {
    }

    void SetVoice(NesoraMikomiParametricVoice* voice) { this->voice = voice; }
    NesoraMikomiParametricVoice* GetVoice() const { return voice; }
    virtual void OnSave(wxCommandEvent& event) = 0;
    virtual void OnOpen(wxCommandEvent& event) = 0;

    virtual void PanelEnable() = 0;
    virtual void PanelDisable() = 0;

protected:
    NesoraMikomiParametricVoice* voice = nullptr;

};


#endif // NESORA_VOICE_MAKE_PANEL_BASE_H
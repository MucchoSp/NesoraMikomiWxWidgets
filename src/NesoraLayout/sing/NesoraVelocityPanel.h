// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NEOSRA_VELOCITY_PANEL_H
#define NEOSRA_VELOCITY_PANEL_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <vector>

#include "../../NesoraStyle/NesoraStyle.h"
#include "../../Nesora/Nesora.h"

#include "../NesoraIDs.h"
#include "../NesoraEvents.h"

#include "NesoraMIDIPanel.h"

class nsVelocityControl : public wxWindow {
public:
    nsVelocityControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxWindow(parent, winid, pos, size, style, name)
    {
        SetMinSize(wxSize(-1, 128)); // 高さを固定
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
        Bind(wxEVT_PAINT, &nsVelocityControl::OnPaint, this);
    }

private:
    void OnPaint(wxPaintEvent& event);
    std::vector<double> velocities; // 0.0 - 1.0
};

class NesoraVelocityPanel : public wxPanel {
public:
    NesoraVelocityPanel() {
        Init();
    }
    NesoraVelocityPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }
    
    void Init();

private:
    nsVelocityControl* velocityControl;
};

#endif // NEOSRA_VELOCITY_PANEL_H
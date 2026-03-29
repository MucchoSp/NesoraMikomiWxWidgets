// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>

#include "../NesoraColor.h"

#ifndef NESORA_SLIDER_H
#define NESORA_SLIDER_H

class nsSlider : public wxControl {
public:
    nsSlider(wxWindow* parent,
        wxWindowID id,
        int value,
        int minValue,
        int maxValue,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize) : wxControl(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetOwnForegroundColour(background_color);

        min_value = minValue;
        max_value = maxValue;
        lower_limit = minValue;
        upper_limit = maxValue;
        current_value = value;

        Connect(wxEVT_PAINT, wxPaintEventHandler(nsSlider::onPaint));
        Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(nsSlider::onLeftDown));
        Connect(wxEVT_LEFT_UP, wxMouseEventHandler(nsSlider::onLeftUp));
        Connect(wxEVT_MOTION, wxMouseEventHandler(nsSlider::onMouseMove));
        Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(nsSlider::onMouseWheel));
    }

    int GetValue() const {
        return current_value;
    }
    int GetMin() const {
        return min_value;
    }
    int GetMax() const {
        return max_value;
    }
    int GetRange() const {
        return max_value - min_value;
    }
    int GetLowerLimit() const {
        return lower_limit;
    }
    int GetUpperLimit() const {
        return upper_limit;
    }
    int GetLimitRange() const {
        return upper_limit - lower_limit;
    }
    void SetValue(int value);
    void SetRange(int minValue, int maxValue);
    void SetLimit(int minValue, int maxValue);
    void UseWheel(bool useWheel);

private:

    int min_value;
    int max_value;
    int current_value;
    int tab_width = 10;
    int lower_limit;
    int upper_limit;
    bool use_wheel = true;

    wxColour background_color = nsGetColor(nsColorType::BACKGROUND);
    wxColour bar_background_color = nsGetColor(nsColorType::PRIMARY);
    wxColour bar_background_disabled_color = nsGetColor(nsColorType::PRIMARY_VARIANT);
    wxColour tab_color = nsGetColor(nsColorType::SECONDARY);

    void onPaint(wxPaintEvent& event);
    void onLeftDown(wxMouseEvent& event);
    void onLeftUp(wxMouseEvent& event);
    void onMouseMove(wxMouseEvent& event);
    void onMouseWheel(wxMouseEvent& event);

};

#endif // NESORA_SLIDER_H

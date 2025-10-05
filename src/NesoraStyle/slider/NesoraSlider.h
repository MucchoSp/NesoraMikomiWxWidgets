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
        SetOwnForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));

        this->minValue = minValue;
        this->maxValue = maxValue;
        this->currentValue = value;

        Connect(wxEVT_PAINT, wxPaintEventHandler(nsSlider::onPaint));
        Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(nsSlider::onLeftDown));
        Connect(wxEVT_LEFT_UP, wxMouseEventHandler(nsSlider::onLeftUp));
        Connect(wxEVT_MOTION, wxMouseEventHandler(nsSlider::onMouseMove));
    }

    int GetValue() const {
        return currentValue;
    }
    int GetMin() const {
        return minValue;
    }
    int GetMax() const {
        return maxValue;
    }
    void SetValue(int value);
    void SetRange(int minValue, int maxValue);

private:

    int minValue;
    int maxValue;
    int currentValue;
    int tabWidth = 10;

    void onPaint(wxPaintEvent& event);
    void onLeftDown(wxMouseEvent& event);
    void onLeftUp(wxMouseEvent& event);
    void onMouseMove(wxMouseEvent& event);

};

#endif // NESORA_SLIDER_H

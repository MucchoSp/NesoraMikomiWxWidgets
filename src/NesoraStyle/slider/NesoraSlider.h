#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>

#include "../NesoraColor.h"

#ifndef NESORA_SLIDER_H
#define NESORA_SLIDER_H

class nsSlider : public wxWindow {
public:
    nsSlider(wxWindow* parent,
        wxWindowID id,
        int value,
        int minValue,
        int maxValue,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize) : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetOwnForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
        Connect(wxEVT_PAINT, wxPaintEventHandler(nsSlider::onPaint));
        Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(nsSlider::onSlide));
    }

private:

    void onPaint(wxPaintEvent& event);
    void onSlide(wxScrollEvent& event);
};

#endif // NESORA_SLIDER_H

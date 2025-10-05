#include "NesoraSlider.h"

void nsSlider::onPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    int w, h;
    GetSize(&w, &h);
    
    dc.SetBrush(wxBrush(bar_background_color));
    dc.SetPen(wxPen(bar_background_color));
    wxRect rect(0, 0, w, h);
    dc.DrawRectangle(rect);

    dc.SetBrush(wxBrush(bar_background_disabled_color));
    dc.SetPen(wxPen(bar_background_disabled_color));
    dc.DrawRectangle({ 0,0,w * GetLowerLimit() / this->GetRange(),h });
    dc.DrawRectangle({ w * GetUpperLimit() / this->GetRange(),0,w,h });

    dc.SetBrush(wxBrush(tab_color));
    dc.SetPen(wxPen(tab_color));
    dc.DrawRectangle((w - tab_width) * this->GetValue() / this->GetRange(), 0, tab_width, h);
}

void nsSlider::SetValue(int value) {
    if (value < lower_limit) value = lower_limit;
    if (value > upper_limit) value = upper_limit;
    current_value = value;
    Refresh();

    wxCommandEvent event(wxEVT_COMMAND_SLIDER_UPDATED, GetId());
    event.SetEventObject(this);
    event.SetInt(current_value);
    ProcessCommand(event);
}

void nsSlider::SetRange(int minValue, int maxValue) {
    min_value = minValue;
    max_value = maxValue;
    SetLimit(minValue, maxValue);
}

void nsSlider::SetLimit(int minValue, int maxValue) {
    lower_limit = minValue;
    upper_limit = maxValue;
    if (current_value < minValue) current_value = minValue;
    if (current_value > maxValue) current_value = maxValue;
    Refresh();
}

void nsSlider::onLeftDown(wxMouseEvent& event) {
    int w, h;
    GetSize(&w, &h);
    int x = event.GetX();
    int value = (x * (this->GetMax() - this->GetMin())) / w;
    SetValue(value);
    CaptureMouse();
}

void nsSlider::onLeftUp(wxMouseEvent& event) {
    if (HasCapture()) {
        ReleaseMouse();
    }
}

void nsSlider::onMouseMove(wxMouseEvent& event) {
    if (event.Dragging() && HasCapture()) {
        int w, h;
        GetSize(&w, &h);
        int x = event.GetX();
        int value = (x * (this->GetMax() - this->GetMin())) / w;
        SetValue(value);
    }
}


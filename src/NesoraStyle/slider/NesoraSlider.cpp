#include "NesoraSlider.h"

void nsSlider::onPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    int w, h;
    GetSize(&w, &h);
    
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    wxRect rect(0, 0, w, h);
    dc.DrawRectangle(rect);

    dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::PRIMARY)));
    dc.DrawRectangle((w - tabWidth) * this->GetValue() / (this->GetMax() - this->GetMin()), 0, tabWidth, h);
}

void nsSlider::SetValue(int value) {
    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;
    currentValue = value;
    Refresh();

    wxCommandEvent event(wxEVT_COMMAND_SLIDER_UPDATED, GetId());
    event.SetEventObject(this);
    event.SetInt(currentValue);
    ProcessCommand(event);
}

void nsSlider::SetRange(int minValue, int maxValue) {
    this->minValue = minValue;
    this->maxValue = maxValue;
    if (currentValue < minValue) currentValue = minValue;
    if (currentValue > maxValue) currentValue = maxValue;
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


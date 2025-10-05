#include "NesoraSlider.h"

void nsSlider::onPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    int w, h;
    GetClientSize(&w, &h);
    wxRect rect(0, 0, w, h);
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));

    //dc.DrawLine(0, h / 2, w, h / 2);

    //rect = DoGetBestClientSize();
    dc.DrawRectangle(rect);

    dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::PRIMARY)));

    dc.DrawRectangle(w * this->GetValue() / (this->GetMax() - this->GetMin()) - 5, 0, 10, h);
    dc.DrawLabel(GetLabel(), rect, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL);
}

void nsSlider::onSlide(wxScrollEvent& event) {
    wxWindow::Refresh();
    event.Skip();
}

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraButton.h"

void nsButton::onPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    int w, h;
    GetSize(&w, &h);
    wxRect rect(0, 0, w, h);
    if(mouseHover) {
        if(mouseDown)
            dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
        else
            dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_VARIANT)));
    }
    else
        dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::PRIMARY_SHADOW)));
    dc.DrawRoundedRectangle(rect, 5);
    dc.DrawLabel(GetLabel(), rect, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL);
}

void nsButton::onMouseUp(wxMouseEvent& event) {
    mouseDown = false;
    wxWindow::Refresh();
    event.Skip();
}

void nsButton::onMouseDown(wxMouseEvent& event) {
    mouseDown = true;
    wxWindow::Refresh();
    event.Skip();
}

void nsButton::onMouseEnter(wxMouseEvent& event) {
    mouseHover = true;
    wxWindow::Refresh();
    event.Skip();
}

void nsButton::onMouseLeave(wxMouseEvent& event) {
    mouseHover = false;
    wxWindow::Refresh();
    event.Skip();
}

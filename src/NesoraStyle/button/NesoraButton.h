#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>

#include "../NesoraColor.h"

#ifndef NESORA_BUTTON_H
#define NESORA_BUTTON_H

class nsButton : public wxButton {
public:
    nsButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxButtonNameStr))
             : wxButton(parent, id, label, pos, size, style, validator, name) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Connect(wxEVT_PAINT, wxPaintEventHandler(nsButton::onPaint));
    }

private:
    void onPaint(wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(this);
        int w, h;
        GetSize(&w, &h);
        wxRect rect(0, 0, w, h);
        dc.SetBrush(wxBrush(nsColor().GetColor(nsColorType::PRIMARY)));
        dc.SetPen(wxPen(nsColor().GetColor(nsColorType::PRIMARY)));
        dc.DrawRectangle(rect);
        dc.SetPen(wxPen(nsColor().GetColor(nsColorType::LIGHT)));
        dc.DrawLabel(GetLabel(), rect);
    }
};

#endif // NESORA_BUTTON_H

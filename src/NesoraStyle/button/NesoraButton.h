#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>

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
        dc.SetBrush(wxBrush(*wxRED));
        dc.SetPen(wxPen(*wxRED));
        dc.DrawRectangle(rect);
        dc.SetPen(wxPen(*wxWHITE));
        dc.DrawLabel(GetLabel(), rect);
    }
};

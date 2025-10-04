#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>

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
        SetOwnForegroundColour(nsGetColor(nsColorType::ON_PRIMARY));
        Connect(wxEVT_PAINT, wxPaintEventHandler(nsButton::onPaint));
        Connect(wxEVT_LEFT_UP, wxMouseEventHandler(nsButton::onMouseUp));
        Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(nsButton::onMouseDown));
        Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(nsButton::onMouseEnter));
        Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(nsButton::onMouseLeave));
    }

private:
    bool mouseDown = false;
    bool mouseHover = false;

    void onPaint(wxPaintEvent& event);
    void onMouseUp(wxMouseEvent& event);
    void onMouseDown(wxMouseEvent& event);
    void onMouseEnter(wxMouseEvent& event);
    void onMouseLeave(wxMouseEvent& event);
};

#endif // NESORA_BUTTON_H

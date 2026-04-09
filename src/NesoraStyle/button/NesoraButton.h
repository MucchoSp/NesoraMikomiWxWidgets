// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
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
        Bind(wxEVT_PAINT, &nsButton::onPaint, this);
        Bind(wxEVT_LEFT_UP, &nsButton::onMouseUp, this);
        Bind(wxEVT_LEFT_DOWN, &nsButton::onMouseDown, this);
        Bind(wxEVT_ENTER_WINDOW, &nsButton::onMouseEnter, this);
        Bind(wxEVT_LEAVE_WINDOW, &nsButton::onMouseLeave, this);
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

class nsBitmapButton : public wxBitmapButton {
public:
    nsBitmapButton(wxWindow *parent,
             wxWindowID id,
             const wxBitmapBundle& bitmap,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxButtonNameStr))
             : wxBitmapButton(parent, id, bitmap, pos, size, style, validator, name) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetOwnForegroundColour(nsGetColor(nsColorType::ON_PRIMARY));
        Bind(wxEVT_PAINT, &nsBitmapButton::onPaint, this);
        Bind(wxEVT_LEFT_UP, &nsBitmapButton::onMouseUp, this);
        Bind(wxEVT_LEFT_DOWN, &nsBitmapButton::onMouseDown, this);
        Bind(wxEVT_ENTER_WINDOW, &nsBitmapButton::onMouseEnter, this);
        Bind(wxEVT_LEAVE_WINDOW, &nsBitmapButton::onMouseLeave, this);
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

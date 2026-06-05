// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_DICTIONALY_EDIT_PANEL_H
#define NESORA_DICTIONALY_EDIT_PANEL_H

#include <wx/wx.h>

#include "../../NesoraStyle/NesoraStyle.h"
#include "../../NesoraLayout/NesoraIDs.h"

#include "../../Nesora/Nesora.h"


// MARK: NesoraDictionalyTitlePanel

class NesoraDictionalyTitlePanel : public wxPanel {
public:
    NesoraDictionalyTitlePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

private:
    wxStaticText* titleText;
    wxTextCtrl* symbolEdit;
    wxTextCtrl* titleEdit;
};


// MARK: NesoraDictionalyControlPointParameterPanel

class NesoraDictionalyControlPointParameterPanel : public wxPanel {
public:
    NesoraDictionalyControlPointParameterPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

private:
    wxStaticText* parameterNameText;
    wxTextCtrl* parameterValueEdit;
};




// MARK: NesoraDictionalyTimelinePanel

class NesoraDictionalyTimelineControl : public wxWindow {
public:
    NesoraDictionalyTimelineControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxControlNameStr)) : wxWindow(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

private:
    void OnPaint(wxPaintEvent& event);

    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnRightUp(wxMouseEvent& event);

};



class NesoraDictionalyTimelinePanel : public wxPanel {
public:
    NesoraDictionalyTimelinePanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

private:
    wxStaticText* formantTimelineText;
    NesoraDictionalyTimelineControl* formantTimelineControl;
    wxStaticText* envelopeTimelineText;
    NesoraDictionalyTimelineControl* envelopeTimelineControl;
};

// MARK: NesoraDictionalyEditPanel

class NesoraDictionalyEditPanel : public wxPanel {
public:
    NesoraDictionalyEditPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

private:
    NesoraDictionalyTitlePanel* titlePanel;
    NesoraDictionalyTimelinePanel* timelinePanel;
    NesoraDictionalyControlPointParameterPanel* controlPointParameterPanel;
};

#endif // NESORA_DICTIONALY_EDIT_PANEL_H
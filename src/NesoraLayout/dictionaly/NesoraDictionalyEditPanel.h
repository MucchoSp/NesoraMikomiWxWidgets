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

class NesoraDictionalyIndexWord;

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
    void SetWord(NesoraDictionalyIndexWord* indexWord);

private:
    wxStaticText* titleText;
    wxTextCtrl* symbolEdit;
    wxTextCtrl* titleEdit;

    NesoraDictionalyIndexWord* indexWord;

    void OnTitleEdit(wxCommandEvent& event);
    void OnSymbolEdit(wxCommandEvent& event);
};




// MARK: NesroaDictionalyTimelineControlBase

class NesroaDictionalyTimelineControlBase : public wxWindow {
public:    
    NesroaDictionalyTimelineControlBase(wxWindow* parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxWindow(parent, winid, pos, size, style, name) {}

    void SetScrollOffset(int xOffset) { this->xOffset = xOffset; Refresh(); }
    void SetWord(ParametricNesoraDictionalyWord* word) { this->word = word; Refresh(); }
    void SetVoice(NesoraMikomiVoice* voice) { this->voice = voice; }

    double DoZoom(int deltax, int deltay, const wxPoint2DDouble center);

protected:
    NesoraMikomiVoice* voice;
    ParametricNesoraDictionalyWord* word;
    
    double xOffset = -10;
    double pixelPerSecond = 100.0;

    int screenWidth = 0;
    int screenHeight = 0;

    bool isDragging = false;
    wxPoint lastMousePosition;
    double dynamicWidth = 4.0;

    int clickMargin = 5;

    void DrawTimeBox(wxGraphicsContext* gc, wxSize& size, double dynamicWidths);

};



// MARK: NesoraDictionalyTimelineControl

class NesoraDictionalyEnvelopelineControl;
class NesoraDictionalyParameterControl;

class NesoraDictionalyTimelineControl : public NesroaDictionalyTimelineControlBase {
public:
    NesoraDictionalyTimelineControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : NesroaDictionalyTimelineControlBase(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

    void SetEnvelopeTimelineControl(NesoraDictionalyEnvelopelineControl* control) { envelopeTimelineControl = control; }
    void SetParameterControl(NesoraDictionalyParameterControl* control) { parameterControl = control; }

private:
    NesoraDictionalyEnvelopelineControl* envelopeTimelineControl;
    NesoraDictionalyParameterControl* parameterControl;

    enum class DraggingTimeType {
        None,
        overlapTime,
        fixedTime,
        blankTime,
    } draggingTime = DraggingTimeType::None;
    
    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
};




// MARK: NesoraDictionalyEnvelopelineControl

class NesoraDictionalyEnvelopelineControl : public NesroaDictionalyTimelineControlBase {
public:
    NesoraDictionalyEnvelopelineControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : NesroaDictionalyTimelineControlBase(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

    void SetTimelineControl(NesoraDictionalyTimelineControl* control) { timelineControl = control; }
    void SetParameterControl(NesoraDictionalyParameterControl* control) { parameterControl = control; }

private:
    NesoraDictionalyTimelineControl *timelineControl;
    NesoraDictionalyParameterControl* parameterControl;

    int selectedPointIndex = -1;

    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnRigthtDown(wxMouseEvent& event);
    void OnRightUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
};


// MARK: NesoraDictionalyParameterControl

class NesoraDictionalyParameterControl : public NesroaDictionalyTimelineControlBase {
public:
    NesoraDictionalyParameterControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : NesroaDictionalyTimelineControlBase(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

    void SetTimelineControl(NesoraDictionalyTimelineControl* control) { timelineControl = control; }
    void SetEnvelopeTimelineControl(NesoraDictionalyEnvelopelineControl* control) { envelopeTimelineControl = control; }

    void SetSelectedParameterID(int ID) { selectedParameterID = ID; Refresh(); }
private:
    NesoraDictionalyTimelineControl *timelineControl;
    NesoraDictionalyEnvelopelineControl* envelopeTimelineControl;

    int selectedParameterID = 0;

    void OnPaint(wxPaintEvent& event);
};



// MARK: NesoraDictionalyTimelinePanel

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

    void SetWord(ParametricNesoraDictionalyWord* word);

private:
    NesoraMikomiParametricVoice* voice;
    ParametricNesoraDictionalyWord* word;

    wxStaticText* timelineText;
    wxStaticText* envelopeTimelineText;
    wxStaticText* parameterText;
    wxChoice* parameterComboBox;

    NesoraDictionalyTimelineControl* timelineControl;
    NesoraDictionalyEnvelopelineControl* envelopeTimelineControl;
    NesoraDictionalyParameterControl* parameterControl;

    wxScrollBar* horizontalScrollBar;

    int xOffset = 0;
    int ppux = 8;
    int ppuy = 8;
    int width = 2000;

    void OnMouseWheel(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMagnify(wxMouseEvent& event);
    void OnParameterChoice(wxCommandEvent& event);
    void OnDropdown(wxCommandEvent& event);
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

    void SetVoice(NesoraMikomiParametricVoice* voice);
    void SetWord(NesoraDictionalyIndexWord* indexWord);

private:
    NesoraMikomiParametricVoice* voice;
    NesoraDictionalyIndexWord* indexWord;

    NesoraDictionalyTitlePanel* titlePanel;
    NesoraDictionalyTimelinePanel* timelinePanel;
};

#endif // NESORA_DICTIONALY_EDIT_PANEL_H

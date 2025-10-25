#pragma once

#ifndef NESORA_IIR_FILTER_PANEL_H
#define NESORA_IIR_FILTER_PANEL_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>

#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "../../NesoraStyle/NesoraStyle.h"

#include "../NesoraIDs.h"

#include "../../Nesora/Nesora.h"
#include "../../Nesora/filter/NesoraIIRFilter.h"




class nsIIRFrequencyResponseControl : public wxWindow {
public:
    nsIIRFrequencyResponseControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr));
    
    NesoraIIRFilter* filter;
private:

    std::vector<double> frequencyResponse;

    std::vector<wxRect2DDouble> peakControlPoints;
    int selectedPeakControlPointIndex;
    int prevSelectedPeakControlPointIndex;
    std::vector<wxRect2DDouble> dipControlPoints;
    int selectedDipControlPointIndex;
    int prevSelectedDipControlPointIndex;

    bool paramater_updated;
    bool shiftKeyDown;

    void OnPaint(wxPaintEvent& event);

    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnRightUp(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);

};


// MARK:nsIIRFilterPanel

class nsIIRFilterPanel : public wxPanel {
public:
    nsIIRFilterPanel() {
        Init();
    }
    nsIIRFilterPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }
    ~nsIIRFilterPanel() {
        // UninitAudioDevice();
    }
    void Init();
private:
    nsIIRFrequencyResponseControl* iirFilter;
};


#endif // NESORA_IIR_FILTER_PANEL_H
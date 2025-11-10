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




// MARK:nsIIRFrequencyResponseControl

class nsIIRFrequencyResponseControl : public wxWindow {
public:
    nsIIRFrequencyResponseControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)); 

    NesoraIIRFilter* filter;
    // UIを更新
    void SyncControlPointsFromFilter();
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

    double samplingFrequency = NesoraDefaultSamplingFrequency;
    double nyquistFrequency = NesoraDefaultNyquistFrequency;

    void OnPaint(wxPaintEvent& event);

    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnRightUp(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);

};




class nsIIRNesoraCubeControl : public wxWindow {
public:
    nsIIRNesoraCubeControl(wxWindow* parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxASCII_STR(wxPanelNameStr)); 


private:

    void OnPaint(wxPaintEvent& event);
    


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
        
    }
    void Init();
    void Update();

    NesoraIIRFilter* GetIIRFilter();

private:
    nsIIRFrequencyResponseControl* iirFilter;
    nsIIRNesoraCubeControl* nesoraCube;
};


#endif // NESORA_IIR_FILTER_PANEL_H
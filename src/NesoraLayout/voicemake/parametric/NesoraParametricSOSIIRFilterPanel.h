#pragma once

#ifndef NESORA_PARAMETRIC_SOSIIR_FILTER_PANEL_H
#define NESORA_PARAMETRIC_SOSIIR_FILTER_PANEL_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>

#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../../lib/miniaudio_split/miniaudio.h"

#include "../../../NesoraStyle/NesoraStyle.h"

#include "../../NesoraIDs.h"
#include "../../NesoraEvents.h"

#include "../../../Nesora/Nesora.h"
#include "../../../Nesora/filter/NesoraParametricSOSIIRFilter/NesoraParametricSOSIIRFilter.h"





// MARK:nsSOSIIRFrequencyResponseControl

class nsParametricSOSIIRFrequencyResponseControl : public wxWindow {
public:
    nsParametricSOSIIRFrequencyResponseControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)); 

    NesoraParametricSOSIIRFilter* filter;
    // UIを更新
    void SyncControlPointsFromFilter();
    void SetSelectedParameter(uint32_t param);
    void OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event);
private:
    
    std::vector<double> frequencyResponse;
    std::map<uint32_t, double>* parameters;

    uint32_t nowSelectedParameter = 0;

    std::vector<wxRect2DDouble> controlPoints;
    std::vector<wxRect2DDouble> destinationControlPoints; //いま選択されているパラメーターにより飛ばされる先の場所
    int selectedControlPointIndex = -1;
    int selectedDestinationControlPointIndex = -1;
    int prevSelectedControlPointIndex = -1;

    bool paramater_updated = false;
    bool shiftKeyDown = false;

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





// MARK:nsParametricSOSIIRFilterPanel

class nsParametricSOSIIRFilterPanel : public wxPanel {
public:
    nsParametricSOSIIRFilterPanel() {
        Init();
    }
    nsParametricSOSIIRFilterPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }
    ~nsParametricSOSIIRFilterPanel() {
        
    }
    void Init();
    void Update();

    NesoraParametricSOSIIRFilter* GetIIRFilter();

private:
    nsParametricSOSIIRFrequencyResponseControl* iirFilter;
};


#endif // NESORA_PARAMETRIC_IIR_FILTER_PANEL_H
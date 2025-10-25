#pragma once

#ifndef NESORA_FORMANT_FILTER_PANEL_H
#define NESORA_FORMANT_FILTER_PANEL_H

#include <wx/wx.h>

#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "../../../lib/miniaudio_split/miniaudio.h"

#include "../../NesoraStyle/NesoraStyle.h"

#include "../NesoraIDs.h"

#include "../../Nesora/Nesora.h"




struct nsFormantFilterChartControlParamaters {
    wxRect2DDouble top_hit_box;
    wxRect2DDouble band_hit_box;
    bool top_hit_box_is_hit;
    bool band_hit_box_is_hit;
    std::vector<double> gaussian_wave;
};

// MARK:nsFormantFilterChartControl

class nsFormantFilterChartControl : public wxWindow {
public:
    nsFormantFilterChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    
    void SetMarginX(double mx);
    void SetMarginY(double my);

    void SetBackgroundColor(const wxColour& pen);
    void SetLabelColor(const wxColour& pen);
    void SetGridColor(const wxColour& pen);
    void SetFrameColor(const wxColour& pen);
    void SetGraphColor(const wxColour& pen);
    void SetParamaters(NesoraFormantFilter* paramaters);

    NesoraFormantFilter* GetParamaters() const;
    
    void MakeData();

    void RefreshChart();
private:
    std::vector<double> m_data;
    std::vector<std::string> m_labels;
    std::string m_xAxisLabel;
    std::string m_yAxisLabel;

    NesoraFormantFilter* filter;
    std::vector<nsFormantFilterChartControlParamaters> paramaters;
    size_t dragging_param;

    wxAffineMatrix2D valueToChartArea;
    wxAffineMatrix2D chartAreaToValue;

    wxColour background_color = nsGetColor(nsColorType::BACKGROUND_HOVER);
    wxColour label_color = nsGetColor(nsColorType::ON_BACKGROUND);
    wxColour grid_color = nsGetColor(nsColorType::ON_BACKGROUND_THIN);
    wxColour frame_color = nsGetColor(nsColorType::ON_BACKGROUND);
    wxColour graph_color = nsGetColor(nsColorType::PRIMARY);
    wxColour second_graph_color = nsGetColor(nsColorType::SECONDARY_VARIANT);
    wxColour second_graph_color2 = nsGetColor(nsColorType::SECONDARY);

    double marginX = 8;
    double marginY = 8;

    int maxFreq = NesoraDefaultNyquistFrequency;
    double dx = 1.0;

    bool paramater_updated = false;

    void OnPaint(wxPaintEvent& event);

    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    std::tuple<int, double, double>calclateChartSegmentCountAndRange(double origLow, double origHisg);

    void calclateGaussianWave(size_t);
};



// MARK:nsFormantFilter

class nsFormantFilter : public wxPanel {
public:
    nsFormantFilter() {
        Init();
    }

    nsFormantFilter(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }


    ~nsFormantFilter() {
        // UninitAudioDevice();
    }

    void Init();
private:
    nsFormantFilterChartControl* chart;
    nsButton* addButton;

    std::vector<double> wave;
    NesoraFormantFilter paramaters;

    void OnAddButtonEvent(wxCommandEvent& event);

};

#endif // NESORA_FORMANT_FILTER_PANEL_H

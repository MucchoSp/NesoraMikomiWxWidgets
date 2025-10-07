#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include <wx/graphics.h>

#include <vector>
#include <string>

#include "../NesoraColor.h"

#ifndef NESORA_SIMPLE_CHART_H
#define NESORA_SIMPLE_CHART_H

class nsSimpleChartControl : public wxWindow {
private:
    std::vector<double> m_data;

    wxColour background_color = nsGetColor(nsColorType::BACKGROUND_HOVER);
    wxColour frame_color = nsGetColor(nsColorType::ON_BACKGROUND);
    wxColour graph_color = nsGetColor(nsColorType::PRIMARY);

    double marginX = 8;
    double marginY = 8;

    void OnPaint(wxPaintEvent& event);
    std::tuple<int, double, double>calclateChartSegmentCountAndRange(double origLow, double origHisg);
public:
    nsSimpleChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    void SetData(const std::vector<double>& data);
    void SetMarginX(double mx);
    void SetMarginY(double my);

    void SetBackgroundColor(const wxColour& pen);
    void SetFrameColor(const wxColour& pen);
    void SetGraphColor(const wxColour& pen);
};

#endif // NESORA_SIMPLE_CHART_H

#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include <wx/graphics.h>

#include <vector>
#include <string>

#include "../NesoraColor.h"

#ifndef NESORA_CHART_H
#define NESORA_CHART_H

class nsChartControl : public wxWindow {
private:
    std::vector<double> m_data;
    std::vector<std::string> m_labels;
    std::string m_title;
    std::string m_xAxisLabel;
    std::string m_yAxisLabel;

    wxColour background_color = nsGetColor(nsColorType::BACKGROUND_HOVER);
    wxColour title_color = nsGetColor(nsColorType::ON_BACKGROUND);
    wxColour label_color = nsGetColor(nsColorType::ON_BACKGROUND);
    wxColour grid_color = nsGetColor(nsColorType::ON_BACKGROUND_THIN);
    wxColour frame_color = nsGetColor(nsColorType::ON_BACKGROUND);
    wxColour graph_color = nsGetColor(nsColorType::PRIMARY);

    bool title_enabled = true;
    bool label_enabled = true;
    bool grid_enabled = true;

    void OnPaint(wxPaintEvent& event);
    std::tuple<int, double, double>calclateChartSegmentCountAndRange(double origLow, double origHisg);
public:
    nsChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    void SetData(const std::vector<double>& data);
    void SetLabels(const std::vector<std::string>& labels);
    void SetTitle(const std::string& title);
    void SetXAxisLabel(const std::string& label);
    void SetYAxisLabel(const std::string& label);

    void ShowTitle(bool enbled);
    void ShowLabel(bool enbled);
    void ShowGrid(bool enbled);

    void SetBackgroundColor(const wxColour& pen);
    void SetTitleColor(const wxColour& pen);
    void SetLabelColor(const wxColour& pen);
    void SetGridColor(const wxColour& pen);
    void SetFrameColor(const wxColour& pen);
    void SetGraphColor(const wxColour& pen);
};

#endif // NESORA_CHART_H

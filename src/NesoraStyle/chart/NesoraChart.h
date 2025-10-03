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

    void OnPaint(wxPaintEvent& event);
public:
    nsChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    void SetData(const std::vector<double>& data);
    void SetLabels(const std::vector<std::string>& labels);
    void SetTitle(const std::string& title);
    void SetXAxisLabel(const std::string& label);
    void SetYAxisLabel(const std::string& label);

};

#endif // NESORA_CHART_H

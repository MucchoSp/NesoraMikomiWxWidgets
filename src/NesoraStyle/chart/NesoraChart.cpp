#include "NesoraChart.h"

nsChartControl::nsChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &nsChartControl::OnPaint, this);
}

void nsChartControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));

        double tw, th;
        gc->GetTextExtent(m_title, &tw, &th);

        const double titleTopBottomMinimumMargin = this->FromDIP(10);

        wxRect2DDouble fullArea(0, 0, static_cast<double>(this->GetSize().GetWidth()), static_cast<double>(this->GetSize().GetHeight()));

        const double marginX = fullArea.GetSize().GetWidth() / 8.0;
        const double marginTop = std::max(fullArea.GetSize().GetHeight() / 8.0, titleTopBottomMinimumMargin * 2.0 + th);
        const double marginBottom = fullArea.GetSize().GetHeight() / 8.0;

        wxRect2DDouble chartArea = fullArea;
        chartArea.Inset(marginX, marginTop, marginX, marginBottom);

        gc->DrawText(m_title, (fullArea.GetSize().GetWidth() - tw) / 2.0, (marginTop - th) / 2.0);

        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND_HOVER)));
        gc->DrawRectangle(chartArea.GetX(), chartArea.GetY(), chartArea.GetWidth(), chartArea.GetHeight());

        delete gc;
    }
}

void nsChartControl::SetData(const std::vector<double>& data) {
    m_data = data;
    Refresh(); // Trigger a repaint
}

void nsChartControl::SetLabels(const std::vector<std::string>& labels) {
    m_labels = labels;
    Refresh(); // Trigger a repaint
}

void nsChartControl::SetTitle(const std::string& title) {
    m_title = title;
    Refresh(); // Trigger a repaint
}

void nsChartControl::SetXAxisLabel(const std::string& label) {
    m_xAxisLabel = label;
    Refresh(); // Trigger a repaint
}

void nsChartControl::SetYAxisLabel(const std::string& label) {
    m_yAxisLabel = label;
    Refresh(); // Trigger a repaint
}


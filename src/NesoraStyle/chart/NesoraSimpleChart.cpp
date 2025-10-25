#include "NesoraSimpleChart.h"

nsSimpleChartControl::nsSimpleChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &nsSimpleChartControl::OnPaint, this);
}

void nsSimpleChartControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc && m_data.size() > 0) {
        wxRect2DDouble fullArea(0, 0, static_cast<double>(this->GetSize().GetWidth()), static_cast<double>(this->GetSize().GetHeight()));

        wxRect2DDouble chartArea = fullArea;
        chartArea.Inset(marginX, marginY, marginX, marginY);

        gc->SetBrush(wxBrush(background_color));
        gc->DrawRectangle(chartArea.m_x, chartArea.m_y, chartArea.m_width, chartArea.m_height);

        wxAffineMatrix2D normalizedToChartArea{};
        normalizedToChartArea.Translate(chartArea.GetLeft(), chartArea.GetTop());
        normalizedToChartArea.Scale(chartArea.m_width, chartArea.m_height);

        double lowValue = *std::min_element(m_data.begin(), m_data.end());
        double highValue = *std::max_element(m_data.begin(), m_data.end());

        double yValueSpan = highValue - lowValue;

        wxAffineMatrix2D normalizedToValue{};
        normalizedToValue.Translate(0, highValue);
        normalizedToValue.Scale(1, -1);
        normalizedToValue.Scale(static_cast<double>(m_data.size() - 1), yValueSpan);

        gc->SetPen(wxPen(frame_color));
        gc->SetBrush(wxBrush(*wxTRANSPARENT_BRUSH));
        gc->DrawRectangle(chartArea.m_x, chartArea.m_y, chartArea.m_width, chartArea.m_height);

        const size_t pointArraySize = m_data.size();
        wxPoint2DDouble* pointArray = new wxPoint2DDouble[pointArraySize];

        wxAffineMatrix2D valueToNormalized = normalizedToValue;
        valueToNormalized.Invert();
        wxAffineMatrix2D valueToChartArea = normalizedToChartArea;
        valueToChartArea.Concat(valueToNormalized);

        for (size_t i = 0;i < pointArraySize;i++)
            pointArray[i] = valueToChartArea.TransformPoint({ static_cast<double>(i),m_data[i] });
        gc->SetPen(wxPen(graph_color));
        gc->StrokeLines(m_data.size(), pointArray);

        delete[] pointArray;
        delete gc;
    }
}

std::tuple<int, double, double> nsSimpleChartControl::calclateChartSegmentCountAndRange(double origLow, double origHigh) {
    constexpr double rangeMults[] = { 0.2,0.25,0.5,1.0,2.0,2.5,5.0 };
    constexpr int maxSegments = 6;

    double magnitude = std::floor(std::log10(origHigh - origLow));

    for (auto r : rangeMults) {
        double stepSize = r * std::pow(10.0, magnitude);
        double low = std::floor(origLow / stepSize) * stepSize;
        double high = std::ceil(origHigh / stepSize) * stepSize;

        int segments = round((high - low) / stepSize);

        if (segments <= maxSegments)
            return std::make_tuple(segments, low, high);
    }

    return std::make_tuple(10, origLow, origHigh);
}

void nsSimpleChartControl::SetData(const std::vector<double>& data) {
    m_data = data;
    Refresh(); // Trigger a repaint
}


void nsSimpleChartControl::SetMarginX(double mx) {
    marginX = mx;
}

void nsSimpleChartControl::SetMarginY(double my) {
    marginY = my;
}


void nsSimpleChartControl::SetBackgroundColor(const wxColour& pen) {
    background_color = pen;
}

void nsSimpleChartControl::SetFrameColor(const wxColour& pen) {
    frame_color = pen;
}

void nsSimpleChartControl::SetGraphColor(const wxColour& pen) {
    graph_color = pen;
}

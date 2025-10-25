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
    if (gc && m_data.size() > 0) {
        wxFont font = GetFont();
        gc->SetFont(font, title_color);

        double tw, th;
        gc->GetTextExtent(m_title, &tw, &th);

        const double titleTopBottomMinimumMargin = this->FromDIP(10) * title_enabled;

        wxRect2DDouble fullArea(0, 0, static_cast<double>(this->GetSize().GetWidth()), static_cast<double>(this->GetSize().GetHeight()));

        const double marginX = fullArea.GetSize().GetWidth() / 8.0;
        const double marginTop = std::max(fullArea.GetSize().GetHeight() / 8.0, titleTopBottomMinimumMargin * 2.0 + th);
        const double marginBottom = fullArea.GetSize().GetHeight() / 8.0;
        double labelsToChartAreaMargin = this->FromDIP(10);

        wxRect2DDouble chartArea = fullArea;
        chartArea.Inset(marginX, marginTop, marginX, marginBottom);

        if(title_enabled)
            gc->DrawText(m_title, (fullArea.GetSize().GetWidth() - tw) / 2.0, (marginTop - th) / 2.0);

        gc->SetBrush(wxBrush(background_color));
        gc->DrawRectangle(chartArea.m_x, chartArea.m_y, chartArea.m_width, chartArea.m_height);

        wxAffineMatrix2D normalizedToChartArea{};
        normalizedToChartArea.Translate(chartArea.GetLeft(), chartArea.GetTop());
        normalizedToChartArea.Scale(chartArea.m_width, chartArea.m_height);

        double lowValue = *std::min_element(m_data.begin(), m_data.end());
        double highValue = *std::max_element(m_data.begin(), m_data.end());

        int segmentCount = 0;
        double rangeLow = 0.0, rangeHigh = 0.0;
        std::tie(segmentCount, rangeLow, rangeHigh) = calclateChartSegmentCountAndRange(lowValue, highValue);

        double yValueSpan = rangeHigh - rangeLow;
        lowValue = rangeLow;
        highValue = rangeHigh;

        wxAffineMatrix2D normalizedToValue{};
        normalizedToValue.Translate(0, highValue);
        normalizedToValue.Scale(1, -1);
        normalizedToValue.Scale(static_cast<double>(m_data.size() - 1), yValueSpan);

        gc->SetPen(wxPen(grid_color));
        gc->SetFont(*wxNORMAL_FONT, label_color);

        if (grid_enabled or label_enabled) {
            double yLinesCount = segmentCount;
            for (int i = 1;i < yLinesCount;i++) {
                double normalizedLineY = static_cast<double>(i) / (yLinesCount);
                auto lineStartPoint = normalizedToChartArea.TransformPoint({ 0, normalizedLineY });
                if (grid_enabled) {

                    auto lineEndPoint = normalizedToChartArea.TransformPoint({ 1, normalizedLineY });

                    wxPoint2DDouble linePoints[] = { lineStartPoint, lineEndPoint };

                    gc->StrokeLines(2, linePoints);
                }

                if(label_enabled) {
                    double valueAtLineY = normalizedToValue.TransformPoint({ 0, normalizedLineY }).m_y;

                    auto text = wxString::Format("%.2f", valueAtLineY);
                    text = wxControl::Ellipsize(text, dc, wxELLIPSIZE_MIDDLE, chartArea.GetLeft() - labelsToChartAreaMargin);

                    double tw, th;
                    gc->GetTextExtent(text, &tw, &th);
                    gc->DrawText(text, chartArea.GetLeft() - labelsToChartAreaMargin - tw, lineStartPoint.m_y - th / 2.0);
                }
            }
        }

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

std::tuple<int, double, double> nsChartControl::calclateChartSegmentCountAndRange(double origLow, double origHigh) {
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


void nsChartControl::ShowTitle(bool enbled) {
    title_enabled = enbled;
}

void nsChartControl::ShowLabel(bool enbled) {
    label_enabled = enbled;
}

void nsChartControl::ShowGrid(bool enbled) {
    grid_enabled = enbled;
}


void nsChartControl::SetBackgroundColor(const wxColour& pen) {
    background_color = pen;
}

void nsChartControl::SetTitleColor(const wxColour& pen) {
    title_color = pen;
}

void nsChartControl::SetLabelColor(const wxColour& pen) {
    label_color = pen;
}

void nsChartControl::SetGridColor(const wxColour& pen) {
    grid_color = pen;
}

void nsChartControl::SetFrameColor(const wxColour& pen) {
    frame_color = pen;
}

void nsChartControl::SetGraphColor(const wxColour& pen) {
    graph_color = pen;
}

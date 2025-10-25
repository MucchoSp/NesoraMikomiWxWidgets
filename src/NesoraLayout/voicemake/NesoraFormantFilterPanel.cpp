#include "NesoraFormantFilterPanel.h"


// MARK:nsFormantFilter

void nsFormantFilter::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Formant filter"));
    wxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    
    //1100 1700 4000 4700
    paramaters.AddParamater(1100, 200, 0.8);
    paramaters.AddParamater(1700, 200, 0.8);
    paramaters.AddParamater(4000, 200, 0.8);
    paramaters.AddParamater(4700, 200, 0.8);

    chart = new nsFormantFilterChartControl(sourceSizer->GetStaticBox(), wxID_ANY);
    chart->SetParamaters(&paramaters);
    addButton = new nsButton(sourceSizer->GetStaticBox(), wxID_ANY, _("Add Formant"));
    addButton->Bind(wxEVT_BUTTON, &nsFormantFilter::OnAddButtonEvent, this);
    buttonsSizer->Add(addButton, 0, wxALL, 4);
    sourceSizer->Add(chart, 1, wxEXPAND | wxALL);
    sourceSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT);
    
    this->SetSizer(sourceSizer);
}

void nsFormantFilter::OnAddButtonEvent(wxCommandEvent& event) {
    paramaters.AddParamater(0, 200, 0.8);
    chart->RefreshChart();
}




// MARK:nsFormantFilterChartControl

nsFormantFilterChartControl::nsFormantFilterChartControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &nsFormantFilterChartControl::OnPaint, this);
    Bind(wxEVT_MOTION, &nsFormantFilterChartControl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &nsFormantFilterChartControl::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &nsFormantFilterChartControl::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &nsFormantFilterChartControl::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &nsFormantFilterChartControl::OnMouseWheel, this);
}

void nsFormantFilterChartControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    MakeData();
    
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc && m_data.size() > 0) {
        wxFont font = GetFont();
        gc->SetFont(font, label_color);

        wxRect2DDouble fullArea(0, 0, static_cast<double>(this->GetSize().GetWidth()), static_cast<double>(this->GetSize().GetHeight()));

        double labelsToChartAreaMargin = this->FromDIP(10);

        double tw, th;
        gc->GetTextExtent("0.5", &tw, &th);
        wxRect2DDouble chartArea = fullArea;
        chartArea.Inset(marginX + labelsToChartAreaMargin + tw, marginY, marginX, marginY);

        gc->SetBrush(wxBrush(background_color));
        gc->DrawRectangle(chartArea.m_x, chartArea.m_y, chartArea.m_width, chartArea.m_height);

        wxAffineMatrix2D normalizedToChartArea{};
        normalizedToChartArea.Translate(chartArea.GetLeft(), chartArea.GetTop());
        normalizedToChartArea.Scale(chartArea.m_width, chartArea.m_height);

        double lowValue = 0;
        double highValue = 1.2;

        wxAffineMatrix2D normalizedToValue{};
        normalizedToValue.Translate(0, highValue);
        normalizedToValue.Scale(1, -1);
        normalizedToValue.Scale(static_cast<double>(m_data.size() - 1), highValue);

        wxAffineMatrix2D valueToNormalized = normalizedToValue;
        valueToNormalized.Invert();
        valueToChartArea = normalizedToChartArea;
        valueToChartArea.Concat(valueToNormalized);

        wxAffineMatrix2D chartAreaToNormalized = normalizedToChartArea;
        chartAreaToNormalized.Invert();
        chartAreaToValue = normalizedToValue;
        chartAreaToValue.Concat(chartAreaToNormalized);

        // if(paramater_updated) {
            for(size_t i = 0;i < paramaters.size();i++) {
                wxPoint2DDouble hitbox = valueToChartArea.TransformPoint({filter->GetParamater()[i].f1_frequency, filter->GetParamater()[i].f1_amplitude});
                paramaters[i].top_hit_box = {hitbox.m_x - 5, hitbox.m_y - 5, 10, 10};
            }

            paramater_updated = false;
        // }

        gc->SetPen(wxPen(grid_color));
        gc->SetFont(*wxNORMAL_FONT, label_color);

        for (double i = 0.2;i < 1.2;i += 0.2) {
            auto lineStartPoint = normalizedToChartArea.TransformPoint({ 0, i / 1.2 });
            auto lineEndPoint = normalizedToChartArea.TransformPoint({ 1, i / 1.2 });

            wxPoint2DDouble linePoints[] = { lineStartPoint, lineEndPoint };

            gc->StrokeLines(2, linePoints);

            auto text = wxString::Format("%.1f", 1.2 - i);
            text = wxControl::Ellipsize(text, dc, wxELLIPSIZE_MIDDLE, chartArea.GetLeft() - labelsToChartAreaMargin);

            double tw, th;
            gc->GetTextExtent(text, &tw, &th);
            gc->DrawText(text, chartArea.GetLeft() - labelsToChartAreaMargin - tw, lineStartPoint.m_y - th / 2.0);
        }

        for(size_t i = 0;i < paramaters.size();i++) {
            gc->SetPen(wxPen(second_graph_color));
            if(paramaters[i].top_hit_box_is_hit)
                gc->SetBrush(wxBrush(second_graph_color2));
            else
                gc->SetBrush(wxBrush(*wxTRANSPARENT_BRUSH));
            gc->DrawEllipse(paramaters[i].top_hit_box);

            const size_t gaussianPointArraySize = paramaters[i].gaussian_wave.size();
            wxPoint2DDouble* gaussianPointArray = new wxPoint2DDouble[gaussianPointArraySize];

            for (size_t j = 0;j < gaussianPointArraySize;j++) {
                gaussianPointArray[j] = valueToChartArea.TransformPoint({ static_cast<double>(j+filter->GetParamater()[i].f1_frequency-gaussianPointArraySize / 2),paramaters[i].gaussian_wave[j] });
            }
            gc->SetPen(wxPen(second_graph_color));
            gc->StrokeLines(gaussianPointArraySize, gaussianPointArray);

            delete[] gaussianPointArray;
        }

        gc->SetPen(wxPen(frame_color));
        gc->SetBrush(wxBrush(*wxTRANSPARENT_BRUSH));
        gc->DrawRectangle(chartArea.m_x, chartArea.m_y, chartArea.m_width, chartArea.m_height);

        const size_t pointArraySize = m_data.size();
        wxPoint2DDouble* pointArray = new wxPoint2DDouble[pointArraySize];

        for (size_t i = 0;i < pointArraySize;i++)
            pointArray[i] = valueToChartArea.TransformPoint({ static_cast<double>(i),m_data[i] });
        gc->SetPen(wxPen(graph_color));
        gc->StrokeLines(m_data.size(), pointArray);

        delete[] pointArray;
        delete gc;
    }
}

std::tuple<int, double, double> nsFormantFilterChartControl::calclateChartSegmentCountAndRange(double origLow, double origHigh) {
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


void nsFormantFilterChartControl::SetMarginX(double mx) {
    marginX = mx;
}

void nsFormantFilterChartControl::SetMarginY(double my) {
    marginY = my;
}



void nsFormantFilterChartControl::SetBackgroundColor(const wxColour& pen) {
    background_color = pen;
}

void nsFormantFilterChartControl::SetLabelColor(const wxColour& pen) {
    label_color = pen;
}

void nsFormantFilterChartControl::SetGridColor(const wxColour& pen) {
    grid_color = pen;
}

void nsFormantFilterChartControl::SetFrameColor(const wxColour& pen) {
    frame_color = pen;
}

void nsFormantFilterChartControl::SetGraphColor(const wxColour& pen) {
    graph_color = pen;
}

void nsFormantFilterChartControl::SetParamaters(NesoraFormantFilter* param) {
    filter = param;
    paramaters.resize(param->GetParamater().size());
    for(size_t i = 0;i < param->GetParamater().size();i++)
        calclateGaussianWave(i);
    paramater_updated = true;
}

NesoraFormantFilter* nsFormantFilterChartControl::GetParamaters() const {
    return filter;
}

void nsFormantFilterChartControl::MakeData() {
    m_data = filter->GetFilter(dx, maxFreq);
}

void nsFormantFilterChartControl::OnMouseLeave(wxMouseEvent& event) {
    // --- IGNORE ---
}

void nsFormantFilterChartControl::OnMouseDown(wxMouseEvent& event) {
    CaptureMouse();
    wxWindow::Refresh();
    event.Skip();
}

void nsFormantFilterChartControl::OnMouseUp(wxMouseEvent& event) {
    if (HasCapture()) {
        ReleaseMouse();
    }
    wxWindow::Refresh();
    event.Skip();
}

void nsFormantFilterChartControl::OnMouseMove(wxMouseEvent& event) {
    if (event.Dragging() && HasCapture()) {
        wxPoint2DDouble param = chartAreaToValue.TransformPoint({static_cast<double>(event.GetX()), static_cast<double>(event.GetY())});

        filter->GetParamater()[dragging_param].f1_frequency = param.m_x;
        filter->GetParamater()[dragging_param].f1_amplitude = std::min(param.m_y, 1.0);

        calclateGaussianWave(dragging_param);

        paramater_updated = true;
    }
    else {
        for(size_t i = 0;i < paramaters.size();i++) {
            if(nsHitTest(paramaters[i].top_hit_box, event.GetX(), event.GetY())) {
                paramaters[i].top_hit_box_is_hit = true;
                dragging_param = i;
                break;
            }
            else {
                paramaters[i].top_hit_box_is_hit = false;
            }
            if(nsHitTest(paramaters[i].band_hit_box, event.GetX(), event.GetY())) {
                paramaters[i].band_hit_box_is_hit = true;
                break;
            }
            else {
                paramaters[i].top_hit_box_is_hit = false;
            }
        }
    }
    
    wxWindow::Refresh();
    event.Skip();
}

void nsFormantFilterChartControl::OnMouseWheel(wxMouseEvent& event) {
    for(size_t i = 0;i < paramaters.size();i++) {
        if(paramaters[i].top_hit_box_is_hit) {
            int delta = event.GetWheelRotation() / event.GetWheelDelta();
            filter->GetParamater()[i].f1_bandwidth += delta * 10;
            filter->GetParamater()[i].f1_bandwidth = std::max(10.0, filter->GetParamater()[i].f1_bandwidth);

            calclateGaussianWave(i);

            paramater_updated = true;
            wxWindow::Refresh();
            break;
        }
    }
    event.Skip();
}

void nsFormantFilterChartControl::calclateGaussianWave(size_t i) {
    int N = filter->GetParamater()[i].f1_bandwidth * 8;
    paramaters[i].gaussian_wave.resize(N);
    double b = filter->GetParamater()[i].f1_frequency - N / 2.0;
    for(int k = 0;k < N;k++) {
        paramaters[i].gaussian_wave[k] = filter->GetParamater()[i].f1_amplitude * exp(-(0.5 * (((double)k + b) * dx - filter->GetParamater()[i].f1_frequency) * ((double)((double)k + b) * dx - filter->GetParamater()[i].f1_frequency)) / (filter->GetParamater()[i].f1_bandwidth * filter->GetParamater()[i].f1_bandwidth));
    }
}

void nsFormantFilterChartControl::RefreshChart() {
    paramaters.resize(filter->GetParamater().size());
    for(size_t i = 0;i < filter->GetParamater().size();i++)
        calclateGaussianWave(i);
    paramater_updated = true;
    wxWindow::Refresh();
}

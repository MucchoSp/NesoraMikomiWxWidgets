#include "NesoraIIRFilterPanel.h"

// MARK:nsIIRFrequencyResponseControl

nsIIRFrequencyResponseControl::nsIIRFrequencyResponseControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
        SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
        filter = new NesoraIIRFilter({{1, 0}, {0.9, 0.1}, {0.9, -0.1}}, 
                                    {{1.0, 0}}); // 初期化例: フィードバック係数0.9の1次IIRフィルタ

        selectedPeakControlPointIndex = -1;
        selectedDipControlPointIndex = -1;
        paramater_updated = false;
        peakControlPoints.resize(filter->GetACoefficients().size());
        dipControlPoints.resize(filter->GetBCoefficients().size());

        Bind(wxEVT_PAINT, &nsIIRFrequencyResponseControl::OnPaint, this);
        Bind(wxEVT_MOTION, &nsIIRFrequencyResponseControl::OnMouseMove, this);
        Bind(wxEVT_LEAVE_WINDOW, &nsIIRFrequencyResponseControl::OnMouseLeave, this);
        Bind(wxEVT_LEFT_DOWN, &nsIIRFrequencyResponseControl::OnMouseDown, this);
        Bind(wxEVT_LEFT_UP, &nsIIRFrequencyResponseControl::OnMouseUp, this);
        Bind(wxEVT_MOUSEWHEEL, &nsIIRFrequencyResponseControl::OnMouseWheel, this);
}

void nsIIRFrequencyResponseControl::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    frequencyResponse = filter->CalculateFrequencyResponse(size.GetWidth());

    for(auto& val : frequencyResponse) {
        val = std::log10(val + 1e-10); // Avoid log(0)
    }

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(frequencyResponse.size(), size.GetHeight() / 2 - frequencyResponse[0] * (size.GetHeight() / 2));
        for (size_t i = 1; i < frequencyResponse.size(); ++i) {
            path.AddLineToPoint(frequencyResponse.size()-i, size.GetHeight() / 2 - frequencyResponse[i] * (size.GetHeight() / 2));
        }
        gc->StrokePath(path);

        std::vector<std::complex<double>> aCoeffs = filter->GetACoefficients();
        std::vector<std::complex<double>> bCoeffs = filter->GetBCoefficients();

        for (double i = 0;i < aCoeffs.size();i++) {
            wxPoint2DDouble lineStartPoint;
            lineStartPoint.m_x = std::arg(aCoeffs[i]) / M_PI * size.GetWidth();
            peakControlPoints[i] = wxRect2DDouble(lineStartPoint.m_x - 5, lineStartPoint.m_y - 5, 10, 10);
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
            gc->DrawEllipse(lineStartPoint.m_x - 5, lineStartPoint.m_y - 5, 10, 10);
        }

        for (double i = 0;i < bCoeffs.size();i++) {
            wxPoint2DDouble lineStartPoint;
            lineStartPoint.m_x = std::arg(bCoeffs[i]) / M_PI * size.GetWidth();
            lineStartPoint.m_y = std::abs(bCoeffs[i]) * size.GetHeight();
            dipControlPoints[i] = wxRect2DDouble(lineStartPoint.m_x - 5, lineStartPoint.m_y - 5, 10, 10);
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
            gc->DrawEllipse(lineStartPoint.m_x - 5, lineStartPoint.m_y - 5, 10, 10);
        }


        delete gc;
    }

    // Draw frequency response here using filter data
    // (Implementation of drawing the frequency response is omitted for brevity)
}

void nsIIRFrequencyResponseControl::OnMouseMove(wxMouseEvent& event) {
    if (event.Dragging() && HasCapture()) {
        if (selectedPeakControlPointIndex != -1) {
            peakControlPoints[selectedPeakControlPointIndex].m_x = event.GetX();
            peakControlPoints[selectedPeakControlPointIndex].m_y = event.GetY();
            double r = peakControlPoints[selectedPeakControlPointIndex].m_y / GetClientSize().GetHeight();
            double theta = (peakControlPoints[selectedPeakControlPointIndex].m_x) / GetClientSize().GetWidth() * M_PI;
            filter->GetACoefficients()[selectedPeakControlPointIndex] = std::polar(r, theta);

            filter->GetACoefficients()[selectedPeakControlPointIndex + 1] = 
                std::polar(peakControlPoints[selectedPeakControlPointIndex + 1].m_y / (GetClientSize().GetHeight()),
                           (peakControlPoints[selectedPeakControlPointIndex + 1].m_x) / GetClientSize().GetWidth() * M_PI);
        }
        if (selectedDipControlPointIndex != -1) {
            dipControlPoints[selectedDipControlPointIndex].m_x = event.GetX();
            dipControlPoints[selectedDipControlPointIndex].m_y = event.GetY();
            filter->GetBCoefficients()[selectedDipControlPointIndex] = 
                std::polar(dipControlPoints[selectedDipControlPointIndex].m_y / (GetClientSize().GetHeight()),
                           (dipControlPoints[selectedDipControlPointIndex].m_x) / GetClientSize().GetWidth() * M_PI);

            if(selectedDipControlPointIndex>0){
                int nowIndex = selectedDipControlPointIndex - 1;
                if(selectedDipControlPointIndex%2){
                    nowIndex = selectedDipControlPointIndex + 1;
                }
                dipControlPoints[nowIndex].m_x = -event.GetX();
                dipControlPoints[nowIndex].m_y = event.GetY();
                filter->GetBCoefficients()[nowIndex] = 
                    std::polar(dipControlPoints[nowIndex].m_y / (GetClientSize().GetHeight()),
                            (dipControlPoints[nowIndex].m_x) / GetClientSize().GetWidth() * M_PI);
            }
        }

        paramater_updated = true;
    }
    else {
        selectedPeakControlPointIndex = -1;
        selectedDipControlPointIndex = -1;
        for(int i = 0;i < peakControlPoints.size();i++) {
            if(nsHitTest(peakControlPoints[i], event.GetX(), event.GetY())) {
                selectedPeakControlPointIndex = i;
                break;
            }
        }
        for(int i = 0;i < dipControlPoints.size();i++) {
            if(nsHitTest(dipControlPoints[i], event.GetX(), event.GetY())) {
                selectedDipControlPointIndex = i;
                break;
            }
        }
    }
    
    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnMouseLeave(wxMouseEvent& event) {
    // Handle mouse leave event
}

void nsIIRFrequencyResponseControl::OnMouseDown(wxMouseEvent& event) {
    CaptureMouse();
    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnMouseUp(wxMouseEvent& event) {
    if (HasCapture()) {
        ReleaseMouse();
    }
    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnMouseWheel(wxMouseEvent& event) {
    // Handle mouse wheel event
}








// MARK:nsIIRFilterPanel
void nsIIRFilterPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("IIR Filter"));
    wxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

    iirFilter = new nsIIRFrequencyResponseControl(sourceSizer->GetStaticBox());
    addButton = new nsButton(sourceSizer->GetStaticBox(), wxID_ANY, _("Add IIR Filter"));
    buttonsSizer->Add(addButton, 0, wxALL, 4);
    sourceSizer->Add(iirFilter, 1, wxEXPAND | wxALL);
    sourceSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT);
    
    this->SetSizer(sourceSizer);
}




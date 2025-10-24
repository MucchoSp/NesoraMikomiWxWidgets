#include "NesoraIIRFilterPanel.h"

// MARK:nsIIRFrequencyResponseControl

nsIIRFrequencyResponseControl::nsIIRFrequencyResponseControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
        SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
        filter = new NesoraIIRFilter({{1, 0}}, 
                                    {{1.0, 0}, {0.9, 0.1}}); // 初期化例: フィードバック係数0.9の1次IIRフィルタ

        selectedPeakControlPointIndex = -1;
        selectedDipControlPointIndex = -1;
        paramater_updated = false;
        peakControlPoints.resize(filter->GetBCoefficients().size());
        dipControlPoints.resize(filter->GetACoefficients().size());
        for(int i = 0;i < peakControlPoints.size();i++) {
            peakControlPoints[i] = wxRect2DDouble(0, 0, 10, 10);
        }
        for(int i = 0;i < dipControlPoints.size();i++) {
            dipControlPoints[i] = wxRect2DDouble(0, 0, 10, 10);
        }

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

    frequencyResponse = filter->CalculateFrequencyResponse(size.GetWidth() * 2);

    for(auto& val : frequencyResponse) {
        val = std::log10(val + 1e-10); // Avoid log(0)
    }

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(0, size.GetHeight() / 2 - frequencyResponse[0] * (size.GetHeight() / 2));
        for (size_t i = 1; i < frequencyResponse.size() / 2; ++i) {
            path.AddLineToPoint(i, size.GetHeight() / 2 - frequencyResponse[i] * (size.GetHeight() / 2));
        }
        gc->StrokePath(path);

        std::vector<std::complex<double>> aCoeffs = filter->GetACoefficients();
        std::vector<std::complex<double>> bCoeffs = filter->GetBCoefficients();

        for (double i = 1;i < bCoeffs.size();i++) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
            gc->DrawEllipse(peakControlPoints[i].m_x, peakControlPoints[i].m_y, 10, 10);
        }

        for (double i = 1;i < aCoeffs.size();i++) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
            gc->DrawEllipse(dipControlPoints[i].m_x, dipControlPoints[i].m_y, 10, 10);
        }


        delete gc;
    }

    // Draw frequency response here using filter data
    // (Implementation of drawing the frequency response is omitted for brevity)
}

void nsIIRFrequencyResponseControl::OnMouseMove(wxMouseEvent& event) {
    if (event.Dragging() && HasCapture()) {
        if (selectedPeakControlPointIndex != -1) {
            if(event.GetX() < 0)
                peakControlPoints[selectedPeakControlPointIndex].m_x = 0;
            else if(event.GetX() > GetClientSize().GetWidth())
                peakControlPoints[selectedPeakControlPointIndex].m_x = GetClientSize().GetWidth() - 5;
            else
                peakControlPoints[selectedPeakControlPointIndex].m_x = (double)event.GetX() - 5;

            if(event.GetY() < 0)
                peakControlPoints[selectedPeakControlPointIndex].m_y = 0;
            else if(event.GetY() > GetClientSize().GetHeight())
                peakControlPoints[selectedPeakControlPointIndex].m_y = GetClientSize().GetHeight() - 5;
            else
                peakControlPoints[selectedPeakControlPointIndex].m_y = (double)event.GetY() - 5;
            
            double r = ((double)GetClientSize().GetHeight() - peakControlPoints[selectedPeakControlPointIndex].m_y) / (double)GetClientSize().GetHeight();
            double theta = peakControlPoints[selectedPeakControlPointIndex].m_x / (double)GetClientSize().GetWidth() * M_PI;
            filter->GetBCoefficients()[selectedPeakControlPointIndex] = -2.0 * std::cos(theta) * r;
            filter->GetBCoefficients()[selectedPeakControlPointIndex + 1] = r * r;
            std::cout << "Updated B Coefficients: " << filter->GetBCoefficients()[selectedPeakControlPointIndex - 1] << ", " << filter->GetBCoefficients()[selectedPeakControlPointIndex] << ", " << filter->GetBCoefficients()[selectedPeakControlPointIndex + 1] <<
                ", " << r << ", " << theta << std::endl;
        }
        if (selectedDipControlPointIndex != -1) {
            dipControlPoints[selectedDipControlPointIndex].m_x = (double)event.GetX();
            dipControlPoints[selectedDipControlPointIndex].m_y = (double)event.GetY();
            double r = ((double)GetClientSize().GetHeight() - dipControlPoints[selectedDipControlPointIndex].m_y) / (double)GetClientSize().GetHeight();
            double theta = (dipControlPoints[selectedDipControlPointIndex].m_x) / (double)GetClientSize().GetWidth() * M_PI;
            filter->GetACoefficients()[selectedDipControlPointIndex] = -2.0 * std::cos(theta) * r;
            filter->GetACoefficients()[selectedDipControlPointIndex + 1] = r * r;
        }

        paramater_updated = true;
    }
    else {
        selectedPeakControlPointIndex = -1;
        selectedDipControlPointIndex = -1;
        for(int i = 1;i < peakControlPoints.size();i++) {
            if(nsHitTest(peakControlPoints[i], event.GetX(), event.GetY())) {
                selectedPeakControlPointIndex = i;
                break;
            }
        }
        for(int i = 1;i < dipControlPoints.size();i++) {
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




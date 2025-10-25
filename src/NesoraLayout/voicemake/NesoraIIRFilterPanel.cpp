#include "NesoraIIRFilterPanel.h"

// MARK:nsIIRFrequencyResponseControl

nsIIRFrequencyResponseControl::nsIIRFrequencyResponseControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetDoubleBuffered(true);
    filter = new NesoraIIRFilter({ {1, 0}, {0.9, 0.1}, {0.9, -0.1} }, { {1.0, 0}, {0.9, 0.1}, {0.9, -0.1} });

    selectedPeakControlPointIndex = -1;
    selectedDipControlPointIndex = -1;
    paramater_updated = false;
    prevSelectedPeakControlPointIndex = -1;
    prevSelectedDipControlPointIndex = -1;
    peakControlPoints.resize(filter->GetACoefficients().size() / 2 + 1);
    dipControlPoints.resize(filter->GetBCoefficients().size() / 2 + 1);
    for(int i = 0;i < peakControlPoints.size();i++) {
        peakControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    for(int i = 0;i < dipControlPoints.size();i++) {
        dipControlPoints[i] = wxRect2DDouble(50 * i, 20, 10, 10);
    }
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());

    Bind(wxEVT_PAINT, &nsIIRFrequencyResponseControl::OnPaint, this);
    Bind(wxEVT_MOTION, &nsIIRFrequencyResponseControl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &nsIIRFrequencyResponseControl::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &nsIIRFrequencyResponseControl::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &nsIIRFrequencyResponseControl::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &nsIIRFrequencyResponseControl::OnMouseWheel, this);
}

void nsIIRFrequencyResponseControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    frequencyResponse = filter->GetResponse();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(0, size.GetHeight() / 2 - frequencyResponse[0] * (size.GetHeight() / 2));
        for (size_t i = 1; i < frequencyResponse.size(); ++i) {
            path.AddLineToPoint(i, size.GetHeight() / 2 - frequencyResponse[i] * (size.GetHeight() / 2));
        }
        gc->StrokePath(path);

        for (size_t i = 1; i < peakControlPoints.size(); ++i) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
            gc->DrawEllipse(peakControlPoints[i].m_x, peakControlPoints[i].m_y, 10, 10);
        }
        for (size_t i = 1; i < dipControlPoints.size(); ++i) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
            gc->DrawEllipse(dipControlPoints[i].m_x, dipControlPoints[i].m_y, 10, 10);
        }

        delete gc;
    }
}

void nsIIRFrequencyResponseControl::OnMouseMove(wxMouseEvent& event) {
    int oldPeak = selectedPeakControlPointIndex;
    int oldDip = selectedDipControlPointIndex;

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
            filter->GetACoefficients()[selectedPeakControlPointIndex * 2 - 1] = std::complex<double>(-2.0 * std::cos(theta) * r, 0.0);
            filter->GetACoefficients()[selectedPeakControlPointIndex * 2] = std::complex<double>(r * r, 0.0);
        }
        if (selectedDipControlPointIndex != -1) {
            if(event.GetX() < 0)
                dipControlPoints[selectedDipControlPointIndex].m_x = 0;
            else if(event.GetX() > GetClientSize().GetWidth())
                dipControlPoints[selectedDipControlPointIndex].m_x = GetClientSize().GetWidth() - 5;
            else
                dipControlPoints[selectedDipControlPointIndex].m_x = (double)event.GetX() - 5;

            if(event.GetY() < 0)
                dipControlPoints[selectedDipControlPointIndex].m_y = 0;
            else if(event.GetY() > GetClientSize().GetHeight())
                dipControlPoints[selectedDipControlPointIndex].m_y = GetClientSize().GetHeight() - 5;
            else
                dipControlPoints[selectedDipControlPointIndex].m_y = (double)event.GetY() - 5;

            double r = dipControlPoints[selectedDipControlPointIndex].m_y / (double)GetClientSize().GetHeight();
            double theta = dipControlPoints[selectedDipControlPointIndex].m_x / (double)GetClientSize().GetWidth() * M_PI;
            filter->GetBCoefficients()[selectedDipControlPointIndex * 2 - 1] = std::complex<double>(-2.0 * std::cos(theta) * r, 0.0);
            filter->GetBCoefficients()[selectedDipControlPointIndex * 2] = std::complex<double>(r * r, 0.0);
        }
            filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
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

    if ((event.Dragging() && HasCapture()) || oldPeak != selectedPeakControlPointIndex || oldDip != selectedDipControlPointIndex) {
        Refresh(false);
    }
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




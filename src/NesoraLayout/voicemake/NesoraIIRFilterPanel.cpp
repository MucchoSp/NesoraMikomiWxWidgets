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
    filter = new NesoraIIRFilter({ 1.0 }, { 1.0 });

    selectedPeakControlPointIndex = -1;
    selectedDipControlPointIndex = -1;
    paramater_updated = false;
    prevSelectedPeakControlPointIndex = -1;
    prevSelectedDipControlPointIndex = -1;
    peakControlPoints.resize(1);
    dipControlPoints.resize(1);
    for(int i = 0;i < peakControlPoints.size();i++) {
        peakControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    for(int i = 0;i < dipControlPoints.size();i++) {
        dipControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    filter->CalculateCoefficientsFromPDs();
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
    shiftKeyDown = false;

    Bind(wxEVT_PAINT, &nsIIRFrequencyResponseControl::OnPaint, this);
    Bind(wxEVT_MOTION, &nsIIRFrequencyResponseControl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &nsIIRFrequencyResponseControl::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &nsIIRFrequencyResponseControl::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &nsIIRFrequencyResponseControl::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &nsIIRFrequencyResponseControl::OnMouseWheel, this);
    Bind(wxEVT_RIGHT_UP, &nsIIRFrequencyResponseControl::OnRightUp, this);
    Bind(wxEVT_RIGHT_DOWN, &nsIIRFrequencyResponseControl::OnRightDown, this);
    Bind(wxEVT_SIZE, &nsIIRFrequencyResponseControl::OnSize, this);
    Bind(wxEVT_KEY_DOWN, &nsIIRFrequencyResponseControl::OnKeyDown, this);
    Bind(wxEVT_KEY_UP, &nsIIRFrequencyResponseControl::OnKeyUp, this);
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
            peakControlPoints[selectedPeakControlPointIndex].m_x = std::min(std::max((double)event.GetX() - 5, 0.0), (double)GetClientSize().GetWidth() - 5.0);
            peakControlPoints[selectedPeakControlPointIndex].m_y = std::min(std::max((double)event.GetY() - 5, 0.0), (double)GetClientSize().GetHeight() - 5.0);
            
            filter->GetPeaks()[selectedPeakControlPointIndex - 1].theta = peakControlPoints[selectedPeakControlPointIndex].m_x / (double)GetClientSize().GetWidth() * M_PI;
            filter->GetPeaks()[selectedPeakControlPointIndex - 1].r = ((double)GetClientSize().GetHeight() - peakControlPoints[selectedPeakControlPointIndex].m_y) / (double)GetClientSize().GetHeight();
        }
        if (selectedDipControlPointIndex != -1) {
            dipControlPoints[selectedDipControlPointIndex].m_x = std::min(std::max((double)event.GetX() - 5, 0.0), (double)GetClientSize().GetWidth() - 5.0);
            dipControlPoints[selectedDipControlPointIndex].m_y = std::min(std::max((double)event.GetY() - 5, 0.0), (double)GetClientSize().GetHeight() - 5.0);

            filter->GetDips()[selectedDipControlPointIndex - 1].theta = dipControlPoints[selectedDipControlPointIndex].m_x / (double)GetClientSize().GetWidth() * M_PI;
            filter->GetDips()[selectedDipControlPointIndex - 1].r = dipControlPoints[selectedDipControlPointIndex].m_y / (double)GetClientSize().GetHeight();
        }
        filter->CalculateCoefficientsFromPDs();
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

void nsIIRFrequencyResponseControl::OnSize(wxSizeEvent& event) {
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnRightUp(wxMouseEvent& event) {
    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnRightDown(wxMouseEvent& event) {
    if(selectedPeakControlPointIndex != -1) {
        peakControlPoints.erase(peakControlPoints.begin() + selectedPeakControlPointIndex);
        filter->GetPeaks().erase(filter->GetPeaks().begin() + selectedPeakControlPointIndex - 1);
        filter->CalculateCoefficientsFromPDs();
        filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
    } else if (selectedDipControlPointIndex != -1) {
        dipControlPoints.erase(dipControlPoints.begin() + selectedDipControlPointIndex);
        filter->GetDips().erase(filter->GetDips().begin() + selectedDipControlPointIndex - 1);
        filter->CalculateCoefficientsFromPDs();
        filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
    } else {
        if(shiftKeyDown) {
            dipControlPoints.push_back(wxRect2DDouble(event.GetX() - 5, event.GetY() - 5, 10, 10));
            filter->GetDips().push_back(NesoraIIRFilterPD{
                .theta = (event.GetX() / (double)GetClientSize().GetWidth()) * M_PI,
                .r = (double)event.GetY() / (double)GetClientSize().GetHeight()
            });
            filter->CalculateCoefficientsFromPDs();
            filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
        } else {
            peakControlPoints.push_back(wxRect2DDouble(event.GetX() - 5, event.GetY() - 5, 10, 10));
            filter->GetPeaks().push_back(NesoraIIRFilterPD{
                .theta = (event.GetX() / (double)GetClientSize().GetWidth()) * M_PI,
                .r = ((double)GetClientSize().GetHeight() - event.GetY()) / (double)GetClientSize().GetHeight()
            });
            filter->CalculateCoefficientsFromPDs();
            filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
        }
    }

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
    
    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnKeyDown(wxKeyEvent& event) {
    std::cout << "Key Down: " << event.GetKeyCode() << std::endl;
    // Handle key down event
    if (event.GetKeyCode() == WXK_SHIFT) {
        shiftKeyDown = true;
    }
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnKeyUp(wxKeyEvent& event) {
    // Handle key up event
    if (event.GetKeyCode() == WXK_SHIFT) {
        shiftKeyDown = false;
    }
    event.Skip();
}




// MARK:nsIIRFilterPanel
void nsIIRFilterPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("IIR Filter"));

    iirFilter = new nsIIRFrequencyResponseControl(sourceSizer->GetStaticBox());
    sourceSizer->Add(iirFilter, 1, wxEXPAND | wxALL);
    
    this->SetSizer(sourceSizer);
}




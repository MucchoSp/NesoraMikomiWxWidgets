#include "NesoraIIRFilterPanel.h"

// MARK:nsIIRFrequencyResponseControl

// グラフを見やすくするためにy軸を指数関数的に変化するようにするためのもの
double y_to_r(double y) {
    return 1.0 - std::pow(1.0 - y, 2.0);
}

double r_to_y(double r) {
    return -std::sqrt(-r + 1.0) + 1.0;
}


nsIIRFrequencyResponseControl::nsIIRFrequencyResponseControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetDoubleBuffered(true);
    filter = new NesoraIIRFilter({ 1.0 }, { 1.0 }, samplingFrequency);

    selectedPeakControlPointIndex = -1;
    selectedDipControlPointIndex = -1;
    paramater_updated = false;
    prevSelectedPeakControlPointIndex = -1;
    prevSelectedDipControlPointIndex = -1;
    peakControlPoints.resize(1);
    dipControlPoints.resize(1);
    for(size_t i = 0;i < peakControlPoints.size();i++) {
        peakControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    for(size_t i = 0;i < dipControlPoints.size();i++) {
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

}

void nsIIRFrequencyResponseControl::SyncControlPointsFromFilter() {
    // 制御点の数
    size_t peakCount = filter ? filter->GetPeaks().size() : 0;
    size_t dipCount = filter ? filter->GetDips().size() : 0;

    // 操作点の数を合わせる
    peakControlPoints.resize(peakCount + 1);
    dipControlPoints.resize(dipCount + 1);

    // 制御点から操作点を計算する
    if (filter) {
        for (size_t i = 1; i < peakControlPoints.size(); i++) {
            const auto& p = filter->GetPeaks()[i - 1];
            peakControlPoints[i].m_x = p.theta / nsPI * (double)GetClientSize().GetWidth();
            peakControlPoints[i].m_y = -r_to_y(p.r) * (double)GetClientSize().GetHeight() + (double)GetClientSize().GetHeight();
            peakControlPoints[i].m_width = 10;
            peakControlPoints[i].m_height = 10;
        }
        for (size_t i = 1; i < dipControlPoints.size(); i++) {
            const auto& d = filter->GetDips()[i - 1];
            dipControlPoints[i].m_x = d.theta / nsPI * (double)GetClientSize().GetWidth();
            dipControlPoints[i].m_y = r_to_y(d.r) * (double)GetClientSize().GetHeight();
            dipControlPoints[i].m_width = 10;
            dipControlPoints[i].m_height = 10;
        }
    } else {
        for (size_t i = 1; i < peakControlPoints.size(); i++) {
            peakControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
        }
        for (size_t i = 1; i < dipControlPoints.size(); i++) {
            dipControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
        }
    }
    filter->CalculateCoefficientsFromPDs();
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
}

void nsIIRFrequencyResponseControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    // 周波数応答
    frequencyResponse = filter->GetResponse();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        // 周波数のグリッド
        for (int i = 0;i < nyquistFrequency / 1000;i++) {
            wxPoint2DDouble linePoints[] = { {(wxDouble)i * 1000.0 * (wxDouble)size.GetWidth() / (wxDouble)nyquistFrequency,0.0}, {(wxDouble)i * 1000.0 * (wxDouble)size.GetWidth() / (wxDouble)nyquistFrequency,static_cast<wxDouble>(size.GetHeight())} };
            gc->StrokeLines(2, linePoints);
        }
        wxPoint2DDouble linePoints[] = { {0.0,(wxDouble)size.GetHeight() / 2.0}, {(wxDouble)size.GetWidth(),(wxDouble)size.GetHeight() / 2.0} };
        gc->StrokeLines(2, linePoints);
        
        // 周波数応答
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(0, size.GetHeight() / 2.0 - frequencyResponse[0] * (size.GetHeight() / 2.0 / 40.0));
        for (size_t i = 1; i < frequencyResponse.size(); i++) {
            path.AddLineToPoint(i, size.GetHeight() / 2.0 - frequencyResponse[i] * (size.GetHeight() / 2.0 / 40.0));
        }
        gc->StrokePath(path);

        // 操作点
        for (size_t i = 1; i < peakControlPoints.size(); i++) {
            if(i == selectedPeakControlPointIndex) 
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
            else
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
            gc->DrawEllipse(peakControlPoints[i].m_x, peakControlPoints[i].m_y, peakControlPoints[i].m_width, peakControlPoints[i].m_height);
        }
        for (size_t i = 1; i < dipControlPoints.size(); i++) {
            if(i == selectedDipControlPointIndex)
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_HOVER)));
            else
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
            gc->DrawEllipse(dipControlPoints[i].m_x, dipControlPoints[i].m_y, dipControlPoints[i].m_width, dipControlPoints[i].m_height);
        }

        // 操作点のパラメータ
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::BACKGROUND_SHADOW)));
        if(selectedPeakControlPointIndex != -1){
            wxString outputString = wxString::Format("peak(%0.2fHz, %0.4f)", filter->GetPeaks()[selectedPeakControlPointIndex - 1].theta * NesoraDefaultSamplingFrequency / ns2PI, filter->GetPeaks()[selectedPeakControlPointIndex - 1].r);
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = peakControlPoints[selectedPeakControlPointIndex].m_x + peakControlPoints[selectedPeakControlPointIndex].m_width + tw < size.GetWidth() ? peakControlPoints[selectedPeakControlPointIndex].m_x + peakControlPoints[selectedPeakControlPointIndex].m_width : peakControlPoints[selectedPeakControlPointIndex].m_x - tw;
            int y = peakControlPoints[selectedPeakControlPointIndex].m_y + peakControlPoints[selectedPeakControlPointIndex].m_height + th < size.GetHeight() ? peakControlPoints[selectedPeakControlPointIndex].m_y + peakControlPoints[selectedPeakControlPointIndex].m_height : peakControlPoints[selectedPeakControlPointIndex].m_y - th;
            gc->DrawRectangle(x,y,tw,th);
            gc->DrawText(outputString, x, y);
        }
        else if(selectedDipControlPointIndex != -1){
            wxString outputString = wxString::Format("dip(%0.2fHz, %0.4f)", filter->GetDips()[selectedDipControlPointIndex - 1].theta * NesoraDefaultSamplingFrequency / ns2PI, filter->GetDips()[selectedDipControlPointIndex - 1].r);
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = dipControlPoints[selectedDipControlPointIndex].m_x + dipControlPoints[selectedDipControlPointIndex].m_width + tw < size.GetWidth() ? dipControlPoints[selectedDipControlPointIndex].m_x + dipControlPoints[selectedDipControlPointIndex].m_width : dipControlPoints[selectedDipControlPointIndex].m_x - tw;
            int y = dipControlPoints[selectedDipControlPointIndex].m_y + dipControlPoints[selectedDipControlPointIndex].m_height + th < size.GetHeight() ? dipControlPoints[selectedDipControlPointIndex].m_y + dipControlPoints[selectedDipControlPointIndex].m_height : dipControlPoints[selectedDipControlPointIndex].m_y - th;
            gc->DrawRectangle(x,y,tw,th);
            gc->DrawText(outputString, x, y);
        }
        delete gc;
    }
}

void nsIIRFrequencyResponseControl::OnMouseMove(wxMouseEvent& event) {
    int oldPeak = selectedPeakControlPointIndex;
    int oldDip = selectedDipControlPointIndex;

    // ドラッグしている場合の処理
    if (event.Dragging() && HasCapture()) {
        if (selectedPeakControlPointIndex != -1) {
            peakControlPoints[selectedPeakControlPointIndex].m_x = std::min(std::max((double)event.GetX() - 5, -5.0), (double)GetClientSize().GetWidth() - 5.0);
            peakControlPoints[selectedPeakControlPointIndex].m_y = std::min(std::max((double)event.GetY() - 5, -5.0), (double)GetClientSize().GetHeight() - 5.0);
            
            filter->GetPeaks()[selectedPeakControlPointIndex - 1].theta = (peakControlPoints[selectedPeakControlPointIndex].m_x + 5) / (double)GetClientSize().GetWidth() * nsPI;
            filter->GetPeaks()[selectedPeakControlPointIndex - 1].r = y_to_r(((double)GetClientSize().GetHeight() - (peakControlPoints[selectedPeakControlPointIndex].m_y + 5)) / (double)GetClientSize().GetHeight());
        }
        if (selectedDipControlPointIndex != -1) {
            dipControlPoints[selectedDipControlPointIndex].m_x = std::min(std::max((double)event.GetX() - 5, -5.0), (double)GetClientSize().GetWidth() - 5.0);
            dipControlPoints[selectedDipControlPointIndex].m_y = std::min(std::max((double)event.GetY() - 5, -5.0), (double)GetClientSize().GetHeight() - 5.0);

            filter->GetDips()[selectedDipControlPointIndex - 1].theta = (dipControlPoints[selectedDipControlPointIndex].m_x + 5) / (double)GetClientSize().GetWidth() * nsPI;
            filter->GetDips()[selectedDipControlPointIndex - 1].r = y_to_r((dipControlPoints[selectedDipControlPointIndex].m_y + 5) / (double)GetClientSize().GetHeight());
        }
        filter->CalculateCoefficientsFromPDs();
        filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
        paramater_updated = true;
    }
    // ドラッグしていない場合の処理
    else {
        // ホバーの処理
        selectedPeakControlPointIndex = -1;
        selectedDipControlPointIndex = -1;
        for(size_t i = 1;i < peakControlPoints.size();i++) {
            if(nsHitTest(peakControlPoints[i], event.GetX(), event.GetY())) {
                selectedPeakControlPointIndex = i;
                break;
            }
        }
        for(size_t i = 1;i < dipControlPoints.size();i++) {
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

    for (int i = 1;i < peakControlPoints.size();i++) {
        peakControlPoints[i].m_x = filter->GetPeaks()[i - 1].theta / nsPI * (double)GetClientSize().GetWidth();
        peakControlPoints[i].m_y = -r_to_y(filter->GetPeaks()[i - 1].r) * (double)GetClientSize().GetHeight() + (double)GetClientSize().GetHeight();
    }
    for (int i = 1;i < dipControlPoints.size();i++) {
        dipControlPoints[i].m_x = filter->GetDips()[i - 1].theta / nsPI * (double)GetClientSize().GetWidth();
        dipControlPoints[i].m_y = r_to_y(filter->GetDips()[i - 1].r) * (double)GetClientSize().GetHeight();
    }

    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnRightUp(wxMouseEvent& event) {
    wxWindow::Refresh();
    event.Skip();
}

void nsIIRFrequencyResponseControl::OnRightDown(wxMouseEvent& event) {
    // ホバーしていたら操作点と制御点を削除
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
        // ホバーしていなければ操作点と制御点を追加
        if(wxGetKeyState(WXK_SHIFT)) {
            dipControlPoints.push_back(wxRect2DDouble(event.GetX() - 5, event.GetY() - 5, 10, 10));
            filter->GetDips().push_back(NesoraIIRFilterPD{
                y_to_r((double)event.GetY() / (double)GetClientSize().GetHeight()),
                (event.GetX() / (double)GetClientSize().GetWidth()) * nsPI,
            });
            filter->CalculateCoefficientsFromPDs();
            filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
        } else {
            peakControlPoints.push_back(wxRect2DDouble(event.GetX() - 5, event.GetY() - 5, 10, 10));
            filter->GetPeaks().push_back(NesoraIIRFilterPD{
                y_to_r(((double)GetClientSize().GetHeight() - event.GetY()) / (double)GetClientSize().GetHeight()),
                (event.GetX() / (double)GetClientSize().GetWidth()) * nsPI,
            });
            filter->CalculateCoefficientsFromPDs();
            filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
        }
    }

    // ホバーの処理
    selectedPeakControlPointIndex = -1;
    selectedDipControlPointIndex = -1;
    for(size_t i = 1;i < peakControlPoints.size();i++) {
        if(nsHitTest(peakControlPoints[i], event.GetX(), event.GetY())) {
            selectedPeakControlPointIndex = i;
            break;
        }
    }
    for(size_t i = 1;i < dipControlPoints.size();i++) {
        if(nsHitTest(dipControlPoints[i], event.GetX(), event.GetY())) {
            selectedDipControlPointIndex = i;
            break;
        }
    }
    
    wxWindow::Refresh();
    event.Skip();
}



// MARK:nsIIRNesoraCubeControl

nsIIRNesoraCubeControl::nsIIRNesoraCubeControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));


    Bind(wxEVT_PAINT, &nsIIRNesoraCubeControl::OnPaint, this);
}

void nsIIRNesoraCubeControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        
        

        delete gc;
    }
}








// MARK:nsIIRFilterPanel
void nsIIRFilterPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("IIR Filter"));

    nesoraCube = new nsIIRNesoraCubeControl(sourceSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(100, 100));
    iirFilter = new nsIIRFrequencyResponseControl(sourceSizer->GetStaticBox());
    sourceSizer->Add(nesoraCube, 0, wxEXPAND | wxALL);
    sourceSizer->Add(iirFilter, 1, wxEXPAND | wxALL);
    
    this->SetSizer(sourceSizer);
}

void nsIIRFilterPanel::Update() {
    // UIの更新
    if (iirFilter) {
        iirFilter->SyncControlPointsFromFilter();
        iirFilter->Refresh(false);
    }
}

NesoraIIRFilter* nsIIRFilterPanel::GetIIRFilter() {
    return iirFilter->filter;
}




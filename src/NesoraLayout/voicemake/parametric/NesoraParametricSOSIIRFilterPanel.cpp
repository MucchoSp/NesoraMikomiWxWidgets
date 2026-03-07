#include "NesoraParametricSOSIIRFilterPanel.h"

// MARK:nsParametricSOSIIRFrequencyResponseControl

// グラフを見やすくするためにy軸を指数関数的に変化するようにするためのもの
double y_to_r(double y) {
    if(y < 0)
        return -1.0 + std::pow(1.0 + y, 2.0);
    else
        return 1.0 - std::pow(1.0 - y, 2.0);
}

double r_to_y(double r) {
    if(r < 0)
        return std::sqrt(r + 1.0) - 1.0;
    else
        return -std::sqrt(-r + 1.0) + 1.0;
}


nsParametricSOSIIRFrequencyResponseControl::nsParametricSOSIIRFrequencyResponseControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetDoubleBuffered(true);
    filter = new NesoraParametricSOSIIRFilter(samplingFrequency);

    selectedControlPointIndex = -1;
    paramater_updated = false;
    prevSelectedControlPointIndex = -1;
    controlPoints.resize(1);
    for(size_t i = 0;i < controlPoints.size();i++) {
        controlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    filter->CalculateCoefficients({}); //ここでパラメーターを渡す
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
    shiftKeyDown = false;

    Bind(wxEVT_PAINT, &nsParametricSOSIIRFrequencyResponseControl::OnPaint, this);
    Bind(wxEVT_MOTION, &nsParametricSOSIIRFrequencyResponseControl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &nsParametricSOSIIRFrequencyResponseControl::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &nsParametricSOSIIRFrequencyResponseControl::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &nsParametricSOSIIRFrequencyResponseControl::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &nsParametricSOSIIRFrequencyResponseControl::OnMouseWheel, this);
    Bind(wxEVT_RIGHT_UP, &nsParametricSOSIIRFrequencyResponseControl::OnRightUp, this);
    Bind(wxEVT_RIGHT_DOWN, &nsParametricSOSIIRFrequencyResponseControl::OnRightDown, this);
    Bind(wxEVT_SIZE, &nsParametricSOSIIRFrequencyResponseControl::OnSize, this);

}

void nsParametricSOSIIRFrequencyResponseControl::SyncControlPointsFromFilter() {
    // 制御点の数
    size_t count = filter ? filter->GetSOFilter().size() : 0;

    // 操作点の数を合わせる
    controlPoints.resize(count + 1);

    // 制御点から操作点を計算する
    if (filter) {
        for (size_t i = 1; i < controlPoints.size(); i++) {
            const auto& p = filter->GetSOFilter()[i - 1];
            controlPoints[i].m_x = p.GetPoint().theta / nsPI * (double)GetClientSize().GetWidth();
            controlPoints[i].m_y = -r_to_y(p.GetPoint().r) * (double)GetClientSize().GetHeight() / 2.0 + (double)GetClientSize().GetHeight() / 2.0;
            controlPoints[i].m_width = 10;
            controlPoints[i].m_height = 10;
        }
    } else {
        for (size_t i = 1; i < controlPoints.size(); i++) {
            controlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
        }
    }
    filter->CalculateCoefficients({}); //ここでパラメーターを渡す
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
}

void nsParametricSOSIIRFrequencyResponseControl::OnPaint(wxPaintEvent& event) {
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
        for (size_t i = 1; i < controlPoints.size(); i++) {
            if(i == selectedControlPointIndex) {
                if(filter->GetSOFilter()[i - 1].GetPoint().r < 0)
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_HOVER)));
                else
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
            } else {
                if(filter->GetSOFilter()[i - 1].GetPoint().r < 0)
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
                else
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
            }
            gc->DrawEllipse(controlPoints[i].m_x, controlPoints[i].m_y, controlPoints[i].m_width, controlPoints[i].m_height);
        }

        // 操作点のパラメータ
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::BACKGROUND_SHADOW)));
        if(selectedControlPointIndex != -1){
            wxString outputString = wxString::Format("point(%0.2fHz, %0.4f)", filter->GetSOFilter()[selectedControlPointIndex - 1].GetPoint().theta * NesoraDefaultSamplingFrequency / ns2PI, std::abs(filter->GetSOFilter()[selectedControlPointIndex - 1].GetPoint().r));
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = controlPoints[selectedControlPointIndex].m_x + controlPoints[selectedControlPointIndex].m_width + tw < size.GetWidth() ? controlPoints[selectedControlPointIndex].m_x + controlPoints[selectedControlPointIndex].m_width : controlPoints[selectedControlPointIndex].m_x - tw;
            int y = controlPoints[selectedControlPointIndex].m_y + controlPoints[selectedControlPointIndex].m_height + th < size.GetHeight() ? controlPoints[selectedControlPointIndex].m_y + controlPoints[selectedControlPointIndex].m_height : controlPoints[selectedControlPointIndex].m_y - th;
            gc->DrawRectangle(x, y, tw, th);
            gc->DrawText(outputString, x, y);
        }
        delete gc;
    }
}

void nsParametricSOSIIRFrequencyResponseControl::OnMouseMove(wxMouseEvent& event) {
    int oldControlPoint = selectedControlPointIndex;

    // ドラッグしている場合の処理
    if (event.Dragging() && HasCapture()) {
        if (selectedControlPointIndex != -1) {
            controlPoints[selectedControlPointIndex].m_x = std::min(std::max((double)event.GetX() - 5.0, -5.0), (double)GetClientSize().GetWidth() - 5.0);
            controlPoints[selectedControlPointIndex].m_y = std::min(std::max((double)event.GetY() - 5.0, -5.0), (double)GetClientSize().GetHeight() - 5.0);
            
            filter->GetSOFilter()[selectedControlPointIndex - 1].SetPoint(
                    {y_to_r(((double)GetClientSize().GetHeight() / 2.0 - (controlPoints[selectedControlPointIndex].m_y + 5.0)) * 2.0 / (double)GetClientSize().GetHeight()), 
                    (controlPoints[selectedControlPointIndex].m_x + 5) / (double)GetClientSize().GetWidth() * nsPI});
        }
        filter->CalculateCoefficients({}); //ここでパラメーターを渡す
        filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
        paramater_updated = true;
    }
    // ドラッグしていない場合の処理
    else {
        // ホバーの処理
        selectedControlPointIndex = -1;
        for(size_t i = 1;i < controlPoints.size();i++) {
            if(nsHitTest(controlPoints[i], event.GetX(), event.GetY())) {
                selectedControlPointIndex = i;
                break;
            }
        }
    }

    if ((event.Dragging() && HasCapture()) || oldControlPoint != selectedControlPointIndex)
        Refresh(false);
    
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnMouseLeave(wxMouseEvent& event) {
    // Handle mouse leave event
}

void nsParametricSOSIIRFrequencyResponseControl::OnMouseDown(wxMouseEvent& event) {
    CaptureMouse();
    wxWindow::Refresh();
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnMouseUp(wxMouseEvent& event) {
    if (HasCapture()) {
        ReleaseMouse();
    }
    wxWindow::Refresh();
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnMouseWheel(wxMouseEvent& event) {
    // Handle mouse wheel event
}

void nsParametricSOSIIRFrequencyResponseControl::OnSize(wxSizeEvent& event) {
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());

    for (int i = 1;i < controlPoints.size();i++) {
        controlPoints[i].m_x = filter->GetSOFilter()[i - 1].GetPoint().theta / nsPI * (double)GetClientSize().GetWidth();
        controlPoints[i].m_y = -r_to_y(filter->GetSOFilter()[i - 1].GetPoint().r) * (double)GetClientSize().GetHeight() / 2.0 + (double)GetClientSize().GetHeight() / 2.0;
    }

    wxWindow::Refresh();
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnRightUp(wxMouseEvent& event) {
    wxWindow::Refresh();
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnRightDown(wxMouseEvent& event) {
    // ホバーしていたら操作点と制御点を削除
    if(selectedControlPointIndex != -1) {
        controlPoints.erase(controlPoints.begin() + selectedControlPointIndex);
        filter->GetSOFilter().erase(filter->GetSOFilter().begin() + selectedControlPointIndex - 1);
        filter->CalculateCoefficients({}); //ここでパラメーターを渡す
        filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
    } else {
        // ホバーしていなければ操作点と制御点を追加
        controlPoints.push_back(wxRect2DDouble(event.GetX() - 5, event.GetY() - 5, 10, 10));
        filter->GetSOFilter().push_back(NesoraIIRFilterPD{
            y_to_r(((double)GetClientSize().GetHeight() / 2.0 - event.GetY()) * 2.0 / (double)GetClientSize().GetHeight()),
            (event.GetX() / (double)GetClientSize().GetWidth()) * nsPI,
        });
        filter->CalculateCoefficients({}); //ここでパラメーターを渡す
        filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
    }

    // ホバーの処理
    selectedControlPointIndex = -1;
    for(size_t i = 1;i < controlPoints.size();i++) {
        if(nsHitTest(controlPoints[i], event.GetX(), event.GetY())) {
            selectedControlPointIndex = i;
            break;
        }
    }
    
    wxWindow::Refresh();
    event.Skip();
}










// MARK:nsParametricSOSIIRFilterPanel
void nsParametricSOSIIRFilterPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Second-Order Sections IIR Filter"));

    iirFilter = new nsParametricSOSIIRFrequencyResponseControl(sourceSizer->GetStaticBox());
    sourceSizer->Add(iirFilter, 1, wxEXPAND | wxALL);
    
    this->SetSizer(sourceSizer);
}

void nsParametricSOSIIRFilterPanel::Update() {
    // UIの更新
    if (iirFilter) {
        iirFilter->SyncControlPointsFromFilter();
        iirFilter->Refresh(false);
    }
}

NesoraParametricSOSIIRFilter* nsParametricSOSIIRFilterPanel::GetIIRFilter() {
    return iirFilter->filter;
}




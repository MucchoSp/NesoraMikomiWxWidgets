// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraParametricSOSIIRFilterPanel.h"

// MARK:nsParametricSOSIIRFrequencyResponseControl

// グラフを見やすくするためにy軸を指数関数的に変化するようにするためのもの
inline double y_to_r(double y) {
    // return y;
    if(y < 0)
        return -1.0 + std::pow(1.0 + y, 2.0);
    else
        return 1.0 - std::pow(1.0 - y, 2.0);
}

inline double r_to_y(double r) {
    // return r;
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

    controlPoints.resize(1);
    for(size_t i = 0;i < controlPoints.size();i++) {
        controlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    destinationControlPoints.resize(1);
    for(size_t i = 0;i < destinationControlPoints.size();i++) {
        destinationControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    nowControlPoints.resize(1);
    for(size_t i = 0;i < nowControlPoints.size();i++) {
        nowControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
    }
    RecalculationFrequencyResponse();

    Bind(wxEVT_PAINT, &nsParametricSOSIIRFrequencyResponseControl::OnPaint, this);
    Bind(wxEVT_MOTION, &nsParametricSOSIIRFrequencyResponseControl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &nsParametricSOSIIRFrequencyResponseControl::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &nsParametricSOSIIRFrequencyResponseControl::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &nsParametricSOSIIRFrequencyResponseControl::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &nsParametricSOSIIRFrequencyResponseControl::OnMouseWheel, this);
    Bind(wxEVT_RIGHT_UP, &nsParametricSOSIIRFrequencyResponseControl::OnRightUp, this);
    Bind(wxEVT_RIGHT_DOWN, &nsParametricSOSIIRFrequencyResponseControl::OnRightDown, this);
    Bind(wxEVT_SIZE, &nsParametricSOSIIRFrequencyResponseControl::OnSize, this);

    wxWindow* voiceMakeFrame = wxWindow::FindWindowById(nsID_VOICE_MAKE_PANEL);
    if (voiceMakeFrame) {
        voiceMakeFrame->Bind(nsEVT_SELECTED_PARAMETER_CHANGED, &nsParametricSOSIIRFrequencyResponseControl::OnChangeSelectedParameter, this);
        voiceMakeFrame->Bind(nsEVT_PARAMETER_CHANGED, &nsParametricSOSIIRFrequencyResponseControl::OnChangeParameter, this);
        voiceMakeFrame->Bind(nsEVT_ADD_PARAMETER, &nsParametricSOSIIRFrequencyResponseControl::OnAddParameter, this);
    }
}

void nsParametricSOSIIRFrequencyResponseControl::RecalculationFrequencyResponse() {
    filter->CalculateCoefficients(parameters); //ここでパラメーターを渡す
    filter->CalculateFrequencyResponse(GetClientSize().GetWidth());
}

void nsParametricSOSIIRFrequencyResponseControl::SyncControlPointsFromFilter() {
    // 制御点の数
    size_t count = filter ? filter->GetSOFilter().size() : 0;

    // 操作点の数を合わせる
    controlPoints.resize(count + 1);
    destinationControlPoints.resize(count + 1);
    nowControlPoints.resize(count + 1);

    // 制御点から操作点を計算する
    if (filter) {
        SetControlPointsFromFilter();
    } else {
        for (size_t i = 1; i < controlPoints.size(); i++) {
            controlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
            destinationControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
            nowControlPoints[i] = wxRect2DDouble(50 * i, 0, 10, 10);
        }
    }
    RecalculationFrequencyResponse();
}

void nsParametricSOSIIRFrequencyResponseControl::SetSelectedParameter(uint32_t param) {
    nowSelectedParameter = param;
    wxWindow::Refresh();
}

void nsParametricSOSIIRFrequencyResponseControl::SetControlPointsFromFilter() {
    for (size_t i = 1; i < controlPoints.size(); i++) {
        const auto& p = filter->GetSOFilter()[i - 1].GetPoint();
        controlPoints[i].m_width = 10.0;
        controlPoints[i].m_height = 10.0;
        controlPoints[i].m_x = p.theta / nsPI * (double)GetClientSize().GetWidth() - controlPoints[i].m_width / 2.0;
        controlPoints[i].m_y = -r_to_y(p.r) * (double)GetClientSize().GetHeight() / 2.0 + (double)GetClientSize().GetHeight() / 2.0 - controlPoints[i].m_height / 2.0;

        const auto& pdp = filter->GetSOFilter()[i - 1].GetParametricPoint(nowSelectedParameter, 1.0);
        destinationControlPoints[i].m_width = 10.0;
        destinationControlPoints[i].m_height = 10.0;
        destinationControlPoints[i].m_x = pdp.theta / nsPI * (double)GetClientSize().GetWidth() - destinationControlPoints[i].m_width / 2.0;
        destinationControlPoints[i].m_y = -r_to_y(pdp.r) * (double)GetClientSize().GetHeight() / 2.0 + (double)GetClientSize().GetHeight() / 2.0 - destinationControlPoints[i].m_height / 2.0;

        if(nowSelectedParameter) {
            const auto& paramValue = parameters.find(nowSelectedParameter);
            if (paramValue != parameters.end()) {
                const auto& pnp = filter->GetSOFilter()[i - 1].GetParametricPoint(nowSelectedParameter, paramValue->second);
                nowControlPoints[i].m_width = 10.0;
                nowControlPoints[i].m_height = 10.0;
                nowControlPoints[i].m_x = pnp.theta / nsPI * (double)GetClientSize().GetWidth() - nowControlPoints[i].m_width / 2.0;
                nowControlPoints[i].m_y = -r_to_y(pnp.r) * (double)GetClientSize().GetHeight() / 2.0 + (double)GetClientSize().GetHeight() / 2.0 - nowControlPoints[i].m_width / 2.0;
            } else {
                nowControlPoints[i] = controlPoints[i];
            }
        } else {
            nowControlPoints[i] = controlPoints[i];
        }
    }
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
            if(destinationControlPoints[i] != controlPoints[i] and nowSelectedParameter) {
                gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
                gc->StrokeLine(controlPoints[i].m_x + 5.0, controlPoints[i].m_y + 5.0, destinationControlPoints[i].m_x + 5.0, destinationControlPoints[i].m_y + 5.0);
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
                gc->DrawEllipse(destinationControlPoints[i].m_x, destinationControlPoints[i].m_y, destinationControlPoints[i].m_width, destinationControlPoints[i].m_height);
            }

            gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2));
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
            gc->DrawEllipse(controlPoints[i].m_x, controlPoints[i].m_y, controlPoints[i].m_width, controlPoints[i].m_height);

            gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2, wxPENSTYLE_TRANSPARENT));
            if(filter->GetSOFilter()[i - 1].GetPoint().r < 0.0)
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
            else
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
            gc->DrawEllipse(nowControlPoints[i].m_x, nowControlPoints[i].m_y, nowControlPoints[i].m_width, nowControlPoints[i].m_height);
        }

        // 操作点のパラメータ
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::BACKGROUND_SHADOW)));
        if(selectedControlPointIndex != -1){
            wxString outputString = wxString::Format("point(%0.2fHz, %0.4f)",
                filter->GetSOFilter()[selectedControlPointIndex - 1].GetPoint().theta * NesoraDefaultSamplingFrequency / ns2PI,
                std::abs(filter->GetSOFilter()[selectedControlPointIndex - 1].GetPoint().r));
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = controlPoints[selectedControlPointIndex].m_x + controlPoints[selectedControlPointIndex].m_width + tw < size.GetWidth() ? controlPoints[selectedControlPointIndex].m_x + controlPoints[selectedControlPointIndex].m_width : controlPoints[selectedControlPointIndex].m_x - tw;
            int y = controlPoints[selectedControlPointIndex].m_y + controlPoints[selectedControlPointIndex].m_height + th < size.GetHeight() ? controlPoints[selectedControlPointIndex].m_y + controlPoints[selectedControlPointIndex].m_height : controlPoints[selectedControlPointIndex].m_y - th;
            gc->DrawRectangle(x, y, tw, th);
            gc->DrawText(outputString, x, y);
        }
        if(selectedDestinationControlPointIndex != -1){
            wxString outputString = wxString::Format("d_point(%0.2fHz, %0.4f)",
                filter->GetSOFilter()[selectedDestinationControlPointIndex - 1].GetParametricPoint(nowSelectedParameter, 1.0).theta * NesoraDefaultSamplingFrequency / ns2PI,
                std::abs(filter->GetSOFilter()[selectedDestinationControlPointIndex - 1].GetParametricPoint(nowSelectedParameter, 1.0).r));
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = destinationControlPoints[selectedDestinationControlPointIndex].m_x + destinationControlPoints[selectedDestinationControlPointIndex].m_width + tw < size.GetWidth() ? destinationControlPoints[selectedDestinationControlPointIndex].m_x + destinationControlPoints[selectedDestinationControlPointIndex].m_width : destinationControlPoints[selectedDestinationControlPointIndex].m_x - tw;
            int y = destinationControlPoints[selectedDestinationControlPointIndex].m_y + destinationControlPoints[selectedDestinationControlPointIndex].m_height + th < size.GetHeight() ? destinationControlPoints[selectedDestinationControlPointIndex].m_y + destinationControlPoints[selectedDestinationControlPointIndex].m_height : destinationControlPoints[selectedDestinationControlPointIndex].m_y - th;
            gc->DrawRectangle(x, y, tw, th);
            gc->DrawText(outputString, x, y);
        }
        delete gc;
    }
}

void nsParametricSOSIIRFrequencyResponseControl::OnMouseMove(wxMouseEvent& event) {
    int oldControlPoint = selectedControlPointIndex;
    int oldDestinationControlPoint = selectedDestinationControlPointIndex;

    // ドラッグしている場合の処理
    if (event.Dragging() && HasCapture()) {
        if (selectedControlPointIndex != -1) {
            filter->GetSOFilter()[selectedControlPointIndex - 1].SetPoint(
                    {y_to_r(((double)GetClientSize().GetHeight() / 2.0 - std::min(std::max((double)event.GetY(), 0.0), (double)GetClientSize().GetHeight())) * 2.0 / (double)GetClientSize().GetHeight()), 
                    std::min(std::max((double)event.GetX(), 0.0), (double)GetClientSize().GetWidth()) / (double)GetClientSize().GetWidth() * nsPI});

        }
        if (selectedDestinationControlPointIndex != -1) {
            filter->GetSOFilter()[selectedDestinationControlPointIndex - 1].SetDestinationPoint(nowSelectedParameter,
                    {y_to_r(((double)GetClientSize().GetHeight() / 2.0 - std::min(std::max((double)event.GetY(), 0.0), (double)GetClientSize().GetHeight())) * 2.0 / (double)GetClientSize().GetHeight()), 
                    std::min(std::max((double)event.GetX(), 0.0), (double)GetClientSize().GetWidth()) / (double)GetClientSize().GetWidth() * nsPI});
        }
        SetControlPointsFromFilter();
        RecalculationFrequencyResponse();
        paramater_updated = true;
    }
    // ドラッグしていない場合の処理
    else {
        // ホバーの処理
        selectedControlPointIndex = -1;
        selectedDestinationControlPointIndex = -1;
        for(size_t i = 1;i < controlPoints.size();i++) {
            if(nsHitTest(controlPoints[i], event.GetX(), event.GetY())) {
                if(nsHitTest(destinationControlPoints[i], event.GetX(), event.GetY())) {
                    if(nowSelectedParameter)
                        selectedDestinationControlPointIndex = i;
                    else
                        selectedControlPointIndex = i;
                } else {
                    selectedControlPointIndex = i;
                }
                break;
            } else if(nsHitTest(destinationControlPoints[i], event.GetX(), event.GetY())) {
                selectedDestinationControlPointIndex = i;
                break;
            }
        }
    }

    if ((event.Dragging() && HasCapture()) || oldControlPoint != selectedControlPointIndex || oldDestinationControlPoint != selectedDestinationControlPointIndex)
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

    SetControlPointsFromFilter();

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
        destinationControlPoints.erase(destinationControlPoints.begin() + selectedControlPointIndex);
        nowControlPoints.erase(nowControlPoints.begin() + selectedControlPointIndex);
        filter->GetSOFilter().erase(filter->GetSOFilter().begin() + selectedControlPointIndex - 1);
    } else if(selectedDestinationControlPointIndex != -1) {
        destinationControlPoints[selectedDestinationControlPointIndex] = controlPoints[selectedDestinationControlPointIndex];
    } else {
        // ホバーしていなければ操作点と制御点を追加
        controlPoints.push_back(wxRect2DDouble(event.GetX() - 5.0, event.GetY() - 5.0, 10.0, 10.0));
        destinationControlPoints.push_back(wxRect2DDouble(event.GetX() - 5.0, event.GetY() - 5.0, 10.0, 10.0));
        nowControlPoints.push_back(wxRect2DDouble(event.GetX() - 5.0, event.GetY() - 5.0, 10.0, 10.0));
        filter->GetSOFilter().push_back(NesoraIIRFilterPD{
                y_to_r(((double)GetClientSize().GetHeight() / 2.0 - (controlPoints.back().m_y + 5.0)) * 2.0 / (double)GetClientSize().GetHeight()), 
                (controlPoints.back().m_x + 5.0) / (double)GetClientSize().GetWidth() * nsPI});
    }

    RecalculationFrequencyResponse();

    // ホバーの処理
    selectedControlPointIndex = -1;
    selectedDestinationControlPointIndex = -1;
    for(size_t i = 1;i < controlPoints.size();i++) {
        if(nsHitTest(controlPoints[i], event.GetX(), event.GetY())) {
            if(nsHitTest(destinationControlPoints[i], event.GetX(), event.GetY())) {
                if(nowSelectedParameter)
                    selectedDestinationControlPointIndex = i;
                else
                    selectedControlPointIndex = i;
            } else {
                selectedControlPointIndex = i;
            }
            break;
        } else if(nsHitTest(destinationControlPoints[i], event.GetX(), event.GetY())) {
            selectedDestinationControlPointIndex = i;
            break;
        }
    }
    
    wxWindow::Refresh();
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event) {
    nowSelectedParameter = event.GetID();

    SetControlPointsFromFilter();
    RecalculationFrequencyResponse();

    wxWindow::Refresh();
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnChangeParameter(nsParameterChangeEvent& event) {
    parameters[event.GetID()] = event.GetParam();

    SetControlPointsFromFilter();
    RecalculationFrequencyResponse();

    wxWindow::Refresh();
    event.Skip();
}

void nsParametricSOSIIRFrequencyResponseControl::OnAddParameter(nsAddParameterEvent& event) {
    parameters[event.GetData()] = 0.0;
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

NesoraFilterBase* nsParametricSOSIIRFilterPanel::GetFilter() {
    return iirFilter->filter;
}



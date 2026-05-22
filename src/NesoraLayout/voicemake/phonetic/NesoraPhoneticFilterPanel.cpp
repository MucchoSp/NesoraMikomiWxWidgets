// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraPhoneticFilterPanel.h"


// MARK:nsPhoneticVowelControl

nsPhoneticVowelControl::nsPhoneticVowelControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetDoubleBuffered(true);

    Bind(wxEVT_PAINT, &nsPhoneticVowelControl::OnPaint, this);
    Bind(wxEVT_MOTION, &nsPhoneticVowelControl::OnMouseMove, this);
    // Bind(wxEVT_LEAVE_WINDOW, &nsPhoneticVowelControl::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &nsPhoneticVowelControl::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &nsPhoneticVowelControl::OnMouseUp, this);
    Bind(wxEVT_MOUSEWHEEL, &nsPhoneticVowelControl::OnMouseWheel, this);
    // Bind(wxEVT_RIGHT_UP, &nsPhoneticVowelControl::OnRightUp, this);
    // Bind(wxEVT_RIGHT_DOWN, &nsPhoneticVowelControl::OnRightDown, this);
    Bind(wxEVT_SIZE, &nsPhoneticVowelControl::OnSize, this);
}

void nsPhoneticVowelControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());


    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));

        // 周波数のグリッド
        for (int i = 0;i < (wxDouble)size.GetWidth() / nowGraphScaleX / 100;i++) {
            wxPoint2DDouble linePoints[] = { {(wxDouble)size.GetWidth() - (wxDouble)(i + 1) * 100.0 * nowGraphScaleX + nowGraphX % (int)(nowGraphScaleX * 100), 0.0},
                                             {(wxDouble)size.GetWidth() - (wxDouble)(i + 1) * 100.0 * nowGraphScaleX + nowGraphX % (int)(nowGraphScaleX * 100), (wxDouble)size.GetHeight()} };
            gc->StrokeLines(2, linePoints);
            gc->DrawText(wxString::Format("%d", (i + nowGraphX / (int)(nowGraphScaleX * 100) + 1) * 100), (wxDouble)size.GetWidth() - (wxDouble)(i + 1) * 100.0 * nowGraphScaleX + nowGraphX % (int)(nowGraphScaleX * 100), 0.0);
        }
        for (int i = 0;i < (wxDouble)size.GetHeight() / nowGraphScaleY / 100;i++) {
            wxPoint2DDouble linePoints[] = { {0.0, (wxDouble)(i + 1) * 100.0 * nowGraphScaleY - nowGraphY % (int)(nowGraphScaleY * 100)},
                                             {(wxDouble)size.GetWidth(), (wxDouble)(i + 1) * 100.0 * nowGraphScaleY - nowGraphY % (int)(nowGraphScaleY * 100)} };
            gc->StrokeLines(2, linePoints);
            wxString label = wxString::Format("%d", (i + nowGraphY / (int)(nowGraphScaleY * 100) + 1) * 100);
            gc->DrawText(label, (wxDouble)size.GetWidth() - GetTextExtent(label).GetWidth(), (wxDouble)(i + 1) * 100.0 * nowGraphScaleY - nowGraphY % (int)(nowGraphScaleY * 100));
        }

        for (const auto& point : vowelPoints) {
            if (vowelPointEnabled[point.first]) {
                wxPoint2DDouble graphPoint = { (wxDouble)size.GetWidth() - point.second.m_x * nowGraphScaleX + nowGraphX, point.second.m_y * nowGraphScaleY - nowGraphY };
                if (point.first == hoveredVowelType) {
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
                } else {
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
                }
                gc->DrawEllipse(graphPoint.m_x - 5, graphPoint.m_y - 5, 10, 10);
                wxString label = wxString::Format("/%s/", nsPhoneticVowelNames[static_cast<size_t>(point.first)]);
                gc->DrawText(label, graphPoint.m_x + 10, graphPoint.m_y);
            }
        }

        if (draggingVowelType != NesoraPhoneticVowelType::None or hoveredVowelType != NesoraPhoneticVowelType::None) {
            NesoraPhoneticVowelType type = (draggingVowelType != NesoraPhoneticVowelType::None) ? draggingVowelType : hoveredVowelType;
            wxString outputString = wxString::Format("/%s/(%0.2fHz, %0.4f)", nsPhoneticVowelNames[static_cast<size_t>(type)], vowelPoints[type].m_x, vowelPoints[type].m_y);
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            wxPoint2DDouble graphPoint = { (wxDouble)size.GetWidth() - vowelPoints[type].m_x * nowGraphScaleX + nowGraphX, vowelPoints[type].m_y * nowGraphScaleY - nowGraphY };
            int x = graphPoint.m_x + 5.0 + tw < size.GetWidth() ? graphPoint.m_x + 5.0 : graphPoint.m_x - tw;
            int y = graphPoint.m_y + th < size.GetHeight() ? graphPoint.m_y : graphPoint.m_y - th;
            gc->DrawRectangle(x, y, tw, th);
            gc->DrawText(outputString, x, y);
        }

        delete gc;
    }

}

void nsPhoneticVowelControl::OnMouseMove(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    wxSize size = GetClientSize();
    if (event.MiddleIsDown()) {
        wxPoint delta = pos - lastMousePos;
        nowGraphX += delta.x;
        if (nowGraphX < 0) nowGraphX = 0;
        nowGraphY -= delta.y;
        if (nowGraphY < 0) nowGraphY = 0;
        Refresh(); // Redraw the control
    }
    if (draggingVowelType != NesoraPhoneticVowelType::None and event.LeftIsDown()) {
        wxPoint2DDouble& vowelPoint = vowelPoints[draggingVowelType];
        vowelPoint.m_x = -(pos.x - nowGraphX - (wxDouble)size.GetWidth()) / nowGraphScaleX;
        vowelPoint.m_y = (pos.y + nowGraphY) / nowGraphScaleY;
        Refresh(); // Redraw the control
        lastMousePos = pos;
        event.Skip();
        return;
    }
    NesoraPhoneticVowelType oldSelectedVowelType = hoveredVowelType;
    hoveredVowelType = NesoraPhoneticVowelType::None;
    for (const auto& point : vowelPoints) {
        wxPoint2DDouble graphPoint = { (wxDouble)size.GetWidth() - point.second.m_x * nowGraphScaleX + nowGraphX, point.second.m_y * nowGraphScaleY - nowGraphY };
        if (std::abs(pos.x - graphPoint.m_x) < 10 && std::abs(pos.y - graphPoint.m_y) < 10) {
            hoveredVowelType = point.first;
            Refresh();
            break;
        }
    }
    if (oldSelectedVowelType != hoveredVowelType) {
        Refresh();
    }
    lastMousePos = pos;
    event.Skip();
}

void nsPhoneticVowelControl::OnMouseDown(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    wxSize size = GetClientSize();
    for (const auto& point : vowelPoints) {
        wxPoint2DDouble graphPoint = { (wxDouble)size.GetWidth() - point.second.m_x * nowGraphScaleX + nowGraphX, point.second.m_y * nowGraphScaleY - nowGraphY };
        if (std::abs(pos.x - graphPoint.m_x) < 10 && std::abs(pos.y - graphPoint.m_y) < 10) {
            draggingVowelType = point.first;
            Refresh(); // Redraw the control
            break;
        }
    }
    event.Skip();
}

void nsPhoneticVowelControl::OnMouseUp(wxMouseEvent& event) {
    draggingVowelType = NesoraPhoneticVowelType::None;
    event.Skip();
}

void nsPhoneticVowelControl::OnMouseWheel(wxMouseEvent& event) {
    int rotation = event.GetWheelRotation();
    if (rotation > 0) {
        nowGraphScaleX *= 1.1;
        nowGraphScaleY *= 1.1;
    } else if (rotation < 0) {
        nowGraphScaleX /= 1.1;
        nowGraphScaleY /= 1.1;
    }
    Refresh(); // Redraw the control
    event.Skip();
}

void nsPhoneticVowelControl::OnSize(wxSizeEvent& event) {
    static int oldSize = 0;
    if (event.GetSize().y != oldSize) {
        SetMinSize(wxSize(event.GetSize().y, event.GetSize().y));
        Refresh();
        event.Skip();

        if (this->GetParent()) {
            this->GetParent()->Layout();
        }
        oldSize = event.GetSize().y;
    }
}

// MARK:nsPhoneticPersonalityControl

nsPhoneticPersonalityControl::nsPhoneticPersonalityControl(wxWindow* parent,
    wxWindowID winid,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name) : wxWindow(parent, winid, pos, size, style, name) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetDoubleBuffered(true);

    genderSlider = new nsSlider(this, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxSize(-1, 15));
    ageSlider = new nsSlider(this, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxSize(-1, 15));
    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Gender")), 0, wxLEFT | wxRIGHT | wxTOP, 10);
    mainSizer->Add(genderSlider, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Age")), 0, wxLEFT | wxRIGHT | wxTOP, 10);
    mainSizer->Add(ageSlider, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    mainSizer->AddStretchSpacer();
    SetSizer(mainSizer);
}


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

    Bind(wxEVT_PAINT, &nsIIRFrequencyResponseControl::OnPaint, this);
}   

void nsIIRFrequencyResponseControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));


        delete gc;
    }

}



// MARK:nsPhoneticFilterPanel

void nsPhoneticFilterPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* filterSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* vowelFormantSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Vowel formant filter"));
    wxStaticBoxSizer* personalityFormantSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Personality formant filter"));
    wxStaticBoxSizer* filterResponseSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Filter frequency response"));

    vowelFilter = new nsPhoneticVowelControl(vowelFormantSizer->GetStaticBox());
    vowelFormantSizer->Add(vowelFilter, 1, wxEXPAND | wxALL);
    personalityFilter = new nsPhoneticPersonalityControl(personalityFormantSizer->GetStaticBox());
    personalityFormantSizer->Add(personalityFilter, 1, wxEXPAND | wxALL);
    filterSizer->Add(vowelFormantSizer, 0, wxEXPAND | wxALL);
    filterSizer->Add(personalityFormantSizer, 1, wxEXPAND | wxALL);

    frequencyResponse = new nsIIRFrequencyResponseControl(filterResponseSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(-1, 64));
    filterResponseSizer->Add(frequencyResponse, 0, wxEXPAND | wxALL);

    mainSizer->Add(filterSizer, 1, wxEXPAND | wxALL);
    mainSizer->Add(filterResponseSizer, 0, wxEXPAND | wxALL);

    this->SetSizer(mainSizer);
}

void nsPhoneticFilterPanel::Update() {
}

NesoraFilterBase* nsPhoneticFilterPanel::GetFilter() {
    return filter;
}
// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraVelocityPanel.h"

void nsVelocityControl::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    if (gc) {
        wxSize size = GetClientSize();
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->SetFont(GetFont(), nsGetColor(nsColorType::ON_BACKGROUND));

        if (velocities.empty()) {
            // デフォルトは全てベロシティ1.0
            velocities.resize(128, 0.5);
        }
        for (size_t i = 0; i < velocities.size(); i++) {
            double x = i * NESORA_MIDI_PANEL_QUANTIME_MIN_WIDTH;
            double y = size.GetHeight() * (1.0 - velocities[i]);
            double w = NESORA_MIDI_PANEL_QUANTIME_MIN_WIDTH;
            double h = size.GetHeight() * velocities[i];

            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
            gc->DrawRectangle(x, y, w, h);
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
            gc->DrawRectangle(x, y, w, h);
        }
    }

    delete gc;
}



void NesoraVelocityPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI Editor"));

    // ピアノロール（中央）を先に作成して、リンクさせる
    velocityControl = new nsVelocityControl(mainSizer->GetStaticBox());

    mainSizer->Add(velocityControl, 1, wxEXPAND);
    
    this->SetSizer(mainSizer);
}

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraDictionalyEditPanel.h"
#include "NesoraDictionalyIndexPanel.h"

#include <cmath>

namespace {

double GetNiceTickStep(double minStep) {
    if (minStep <= 0.0) {
        return 1.0;
    }

    double exponent = std::pow(10.0, std::floor(std::log10(minStep)));
    double fraction = minStep / exponent;

    double niceFraction = 1.0;
    if (fraction <= 1.0) {
        niceFraction = 1.0;
    } else if (fraction <= 2.0) {
        niceFraction = 2.0;
    } else if (fraction <= 5.0) {
        niceFraction = 5.0;
    } else {
        niceFraction = 10.0;
    }

    return niceFraction * exponent;
}

int GetTickDecimalPlaces(double tickStep) {
    if (tickStep >= 1.0) {
        return 0;
    }

    return std::max(0, static_cast<int>(std::ceil(-std::log10(tickStep))));
}

}




// MARK: NesoraDictionalyTitlePanel

void NesoraDictionalyTitlePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Title"));

    titleEdit = new wxTextCtrl(mainSizer->GetStaticBox(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    titleEdit->SetHint(_("Word"));
    mainSizer->Add(titleEdit, 1, wxEXPAND | wxALL);
    mainSizer->AddSpacer(10);
    symbolEdit = new wxTextCtrl(mainSizer->GetStaticBox(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    symbolEdit->SetHint(_("Symbol"));
    mainSizer->Add(symbolEdit, 1, wxEXPAND | wxALL);

    titleEdit->Bind(wxEVT_TEXT, &NesoraDictionalyTitlePanel::OnTitleEdit, this);
    symbolEdit->Bind(wxEVT_TEXT, &NesoraDictionalyTitlePanel::OnSymbolEdit, this);

    SetSizer(mainSizer);
}

void NesoraDictionalyTitlePanel::SetWord(NesoraDictionalyIndexWord* indexWord) {
    this->indexWord = indexWord;
    if (indexWord) {
        titleEdit->SetValue(indexWord->word.word);
        symbolEdit->SetValue(indexWord->word.symbol);
    } else {
        titleEdit->Clear();
        symbolEdit->Clear();
    }
}

void NesoraDictionalyTitlePanel::OnTitleEdit(wxCommandEvent& event) {
    if (indexWord) {
        indexWord->word.word = titleEdit->GetValue().ToStdString();
        indexWord->ChangeWord();
    }
}

void NesoraDictionalyTitlePanel::OnSymbolEdit(wxCommandEvent& event) {
    if (indexWord) {
        indexWord->word.symbol = symbolEdit->GetValue().ToStdString();
        indexWord->ChangeWord();
    }
}





// MARK: TimelineControlBase

void NesroaDictionalyTimelineControlBase::DrawTimeBox(wxGraphicsContext* gc, wxSize& size, double dynamicWidth) {
     if (word){
        // 単語の重なり時間を示す線を描画
        double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
        double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;
        double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;

        gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 4));
        gc->DrawRectangle(0, 0, overlapX, size.y);

        gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_TRANSPARENT)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY_TRANSPARENT), 4));
        gc->DrawRectangle(fixedX, 0.0, dynamicWidth * pixelPerSecond, size.y);

        gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 4));
        gc->DrawRectangle(blankX, 0.0, size.x - blankX, size.y);
    }

    gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
    wxPoint2DDouble points[] = { {(double)-xOffset, 0.0}, {(double)-xOffset, (double)size.y} };
    gc->StrokeLines(2, points);
}

double NesroaDictionalyTimelineControlBase::DoZoom(int deltax, int deltay, const wxPoint2DDouble center) {
    double zoomFactor = 1.0 + deltay * 0.025; // ズームの倍率を計算
    pixelPerSecond *= zoomFactor;
    pixelPerSecond = std::max(1.0, pixelPerSecond);

    // ズームの中心を基準にスクロール位置を調整
    xOffset = -(center.m_x - (center.m_x + xOffset) * zoomFactor);

    Refresh();

    return xOffset;
}



// MARK: TimelineControl


void NesoraDictionalyTimelineControl::Init() {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, 50));
    
    Bind(wxEVT_PAINT, &NesoraDictionalyTimelineControl::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraDictionalyTimelineControl::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &NesoraDictionalyTimelineControl::OnLeftUp, this);
    Bind(wxEVT_MOTION, &NesoraDictionalyTimelineControl::OnMouseMove, this);
}

void NesoraDictionalyTimelineControl::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBackground(nsGetColor(nsColorType::BACKGROUND));
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 4));
        gc->DrawRectangle(0, 0, size.x, size.y);
        DrawTimeBox(gc, size, dynamicWidth);

        // 目盛りの描画
        const double minPixelSpacing = 50.0;
        const double rawTickStep = minPixelSpacing / pixelPerSecond;
        const double tickStep = GetNiceTickStep(rawTickStep);
        const int decimalPlaces = GetTickDecimalPlaces(tickStep);

        const double visibleTimeStart = xOffset / pixelPerSecond;
        const double visibleTimeEnd = (xOffset + size.x) / pixelPerSecond;

        const long long startTickIndex = static_cast<long long>(std::floor(visibleTimeStart / tickStep));
        const long long endTickIndex = static_cast<long long>(std::ceil(visibleTimeEnd / tickStep));

        gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 1));
        for (long long tickIndex = startTickIndex; tickIndex <= endTickIndex; ++tickIndex) {
            double time = tickIndex * tickStep;
            if (std::abs(time) < tickStep * 1e-8) {
                time = 0.0;
            }

            double screenX = time * pixelPerSecond - xOffset;
            gc->StrokeLine(screenX, size.y - 8, screenX, size.y);

            wxString format = wxString::Format("%%.%df", decimalPlaces);
            wxString timeStr = wxString::Format(format, time);
            gc->DrawText(timeStr, screenX + 2, size.y - 34);
        }

        delete gc;
    }
}

void NesoraDictionalyTimelineControl::OnLeftDown(wxMouseEvent& event) {
    isDragging = true;
    lastMousePosition = event.GetPosition();
    draggingTime = DraggingTimeType::None;
    if (word) {
        double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
        double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;
        double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;

        if (std::abs(event.GetPosition().x - overlapX) <= clickMargin) {
            draggingTime = DraggingTimeType::overlapTime;
        } else if (event.GetPosition().x >= fixedX && event.GetPosition().x < fixedX + dynamicWidth * pixelPerSecond) {
            draggingTime = DraggingTimeType::fixedTime;
            SetCursor(wxCursor(wxCURSOR_CLOSED_HAND));
        } else if (std::abs(event.GetPosition().x - blankX) <= clickMargin) {
            draggingTime = DraggingTimeType::blankTime;
        }
    }
}

void NesoraDictionalyTimelineControl::OnLeftUp(wxMouseEvent& event) {
    isDragging = false;
    if (draggingTime == DraggingTimeType::fixedTime) {
        SetCursor(wxCursor(wxCURSOR_OPEN_HAND));
    }
}

void NesoraDictionalyTimelineControl::OnMouseMove(wxMouseEvent& event) {
    if (isDragging) {
        wxPoint currentPosition = event.GetPosition();
        int deltaX = currentPosition.x - lastMousePosition.x;
        
        if (draggingTime == DraggingTimeType::overlapTime) {
            word->overlapTime -= deltaX / pixelPerSecond;
        } else if (draggingTime == DraggingTimeType::fixedTime) {
            word->fixedTime += deltaX / pixelPerSecond;
        } else if (draggingTime == DraggingTimeType::blankTime) {
            word->blankTime += deltaX / pixelPerSecond;
        }

        lastMousePosition = currentPosition;
        NesoraDictionalyTimelinePanel* timelinePanel = (NesoraDictionalyTimelinePanel*)GetParent()->GetParent();
        timelinePanel->SetWord(word); // 変更を反映
    } else {
        // マウスポインタの更新
        if (word) {
            double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
            double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;
            double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;

            if (std::abs(event.GetPosition().x - overlapX) <= clickMargin) {
                SetCursor(wxCursor(wxCURSOR_SIZEWE));
            } else if (event.GetPosition().x >= fixedX && event.GetPosition().x < fixedX + dynamicWidth * pixelPerSecond) {
                SetCursor(wxCursor(wxCURSOR_OPEN_HAND));
            } else if (std::abs(event.GetPosition().x - blankX) <= clickMargin) {
                SetCursor(wxCursor(wxCURSOR_SIZEWE));
            } else {
                SetCursor(wxCursor(wxCURSOR_ARROW));
            }
        }
    }
}






// MARK: EnvelopelineControl

void NesoraDictionalyEnvelopelineControl::Init() {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, 100));

    Bind(wxEVT_PAINT, &NesoraDictionalyEnvelopelineControl::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraDictionalyEnvelopelineControl::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &NesoraDictionalyEnvelopelineControl::OnLeftUp, this);
    Bind(wxEVT_RIGHT_DOWN, &NesoraDictionalyEnvelopelineControl::OnRigthtDown, this);
    Bind(wxEVT_RIGHT_UP, &NesoraDictionalyEnvelopelineControl::OnRightUp, this);
    Bind(wxEVT_MOTION, &NesoraDictionalyEnvelopelineControl::OnMouseMove, this);
}

void NesoraDictionalyEnvelopelineControl::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    wxSize size = GetSize();
    dc.SetBackground(nsGetColor(nsColorType::BACKGROUND));
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 4));
        gc->DrawRectangle(0, 0, size.x, size.y);
        DrawTimeBox(gc, size, dynamicWidth);
        if (word) {
            double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
            double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;

            std::vector<wxPoint2DDouble> points;
            points.push_back({(double)overlapX, (double)size.y});

            for (size_t i = 0; i < word->envelope.size(); i++) {
                double timeX = word->envelope[i].time * pixelPerSecond + overlapX;
                double valueY = size.y - (word->envelope[i].value * size.y);
                points.push_back({timeX, valueY});
            }
            points.push_back({(double)fixedX, 0.0});
            gc->StrokeLines(points.size(), points.data());
            for (size_t i = 0; i < word->envelope.size(); i++) {
                const auto& point = points[i + 1];
                gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2));
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
                gc->DrawEllipse(point.m_x - clickMargin, point.m_y - clickMargin, clickMargin * 2, clickMargin * 2);
            }
            if (selectedPointIndex >= 0 && selectedPointIndex < static_cast<int>(word->envelope.size())) {
                const auto& selectedPoint = points[selectedPointIndex + 1];
                gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
                gc->DrawEllipse(selectedPoint.m_x - clickMargin, selectedPoint.m_y - clickMargin, clickMargin * 2, clickMargin * 2);
            }
        }

        delete gc;
    }
}

void NesoraDictionalyEnvelopelineControl::OnLeftDown(wxMouseEvent& event) {
    isDragging = true;
    lastMousePosition = event.GetPosition();
}

void NesoraDictionalyEnvelopelineControl::OnLeftUp(wxMouseEvent& event) {
    isDragging = false;
    if (selectedPointIndex >= 0 && word && selectedPointIndex < static_cast<int>(word->envelope.size())) {
        std::sort(word->envelope.begin(), word->envelope.end(), [](const auto& a, const auto& b) {
            return a.time < b.time;
        });
        Refresh();
    }
    selectedPointIndex = -1;
}

void NesoraDictionalyEnvelopelineControl::OnRigthtDown(wxMouseEvent& event) {
    lastMousePosition = event.GetPosition();
    if (word) {
        word->envelope.push_back({(lastMousePosition.x + xOffset) / pixelPerSecond + word->overlapTime, 1.0 - ((double)lastMousePosition.y / (double)GetSize().y)});
        std::sort(word->envelope.begin(), word->envelope.end(), [](const auto& a, const auto& b) {
            return a.time < b.time;
        });
        Refresh();
    }
}

void NesoraDictionalyEnvelopelineControl::OnRightUp(wxMouseEvent& event) {
}

void NesoraDictionalyEnvelopelineControl::OnMouseMove(wxMouseEvent& event) {
    static int oldSelectedPointIndex = -1;
    if (isDragging) {
        wxPoint currentPosition = event.GetPosition();
        int deltaX = currentPosition.x - lastMousePosition.x;
        int deltaY = currentPosition.y - lastMousePosition.y;

        if (selectedPointIndex >= 0 && selectedPointIndex < static_cast<int>(word->envelope.size())) {
            word->envelope[selectedPointIndex].time += deltaX / pixelPerSecond;
            word->envelope[selectedPointIndex].value -= deltaY / (double)GetSize().y;
            if (word->envelope[selectedPointIndex].value < 0.0) {
                word->envelope[selectedPointIndex].value = 0.0;
            } else if (word->envelope[selectedPointIndex].value > 1.0) {
                word->envelope[selectedPointIndex].value = 1.0;
            }
        }

        lastMousePosition = currentPosition;
        Refresh();
    } else {
        // マウスポインタの更新
        if (word) {
            SetCursor(wxCursor(wxCURSOR_ARROW));
            selectedPointIndex = -1;
            for (int i = 0; i < word->envelope.size(); i++) {
                double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
                double timeX = word->envelope[i].time * pixelPerSecond + overlapX;
                double valueY = GetSize().y - (word->envelope[i].value * GetSize().y);
                if (std::abs(event.GetPosition().x - timeX) <= clickMargin && std::abs(event.GetPosition().y - valueY) <= clickMargin) {
                    SetCursor(wxCursor(wxCURSOR_HAND));
                    selectedPointIndex = i;
                    break;
                }
            }
        }
    }

    if ((event.Dragging() && HasCapture()) || selectedPointIndex != oldSelectedPointIndex) {
        Refresh();
        oldSelectedPointIndex = selectedPointIndex;
    }

    event.Skip();
}





// MARK: ParameterControl

void NesoraDictionalyParameterControl::Init() {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, 100));

    Bind(wxEVT_PAINT, &NesoraDictionalyParameterControl::OnPaint, this);
}

void NesoraDictionalyParameterControl::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBackground(nsGetColor(nsColorType::BACKGROUND));
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 4));
        gc->DrawRectangle(0, 0, size.x, size.y);
        gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
        wxPoint2DDouble points[] = { {(double)-xOffset, 0.0}, {(double)-xOffset, (double)size.y} };
        DrawTimeBox(gc, size, dynamicWidth);
        if (word) {
        }

        delete gc;
    }
}




// MARK: TimelinePanel

void NesoraDictionalyTimelinePanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Timeline"));
    timelineText = new wxStaticText(mainSizer->GetStaticBox(), wxID_ANY, _("Timeline"));
    mainSizer->Add(timelineText, 0, wxALIGN_LEFT | wxALL);
    timelineControl = new NesoraDictionalyTimelineControl(mainSizer->GetStaticBox(), wxID_ANY);
    mainSizer->Add(timelineControl, 0, wxEXPAND | wxALL);

    envelopeTimelineText = new wxStaticText(mainSizer->GetStaticBox(), wxID_ANY, _("Envelope"));
    mainSizer->Add(envelopeTimelineText, 0, wxALIGN_LEFT | wxALL);
    envelopeTimelineControl = new NesoraDictionalyEnvelopelineControl(mainSizer->GetStaticBox(), wxID_ANY);
    mainSizer->Add(envelopeTimelineControl, 1, wxEXPAND | wxALL);

    wxBoxSizer* parameterSizer = new wxBoxSizer(wxHORIZONTAL);
    parameterText = new wxStaticText(mainSizer->GetStaticBox(), wxID_ANY, _("Parameter"));
    parameterSizer->Add(parameterText, 0, wxEXPAND | wxALL);
    parameterComboBox = new wxChoice(mainSizer->GetStaticBox(), wxID_ANY);
    parameterSizer->Add(parameterComboBox, 0, wxEXPAND | wxALL);
    mainSizer->Add(parameterSizer, 0, wxEXPAND | wxALL);
    parameterControl = new NesoraDictionalyParameterControl(mainSizer->GetStaticBox(), wxID_ANY);
    mainSizer->Add(parameterControl, 1, wxEXPAND | wxALL);

    horizontalScrollBar = new wxScrollBar(mainSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    horizontalScrollBar->SetScrollbar(xOffset, GetSize().GetWidth(), width, GetSize().GetWidth());
    mainSizer->Add(horizontalScrollBar, 0, wxEXPAND | wxALL);

    envelopeTimelineControl->SetTimelineControl(timelineControl);
    envelopeTimelineControl->SetParameterControl(parameterControl);
    parameterControl->SetTimelineControl(timelineControl);
    parameterControl->SetEnvelopeTimelineControl(envelopeTimelineControl);
    timelineControl->SetEnvelopeTimelineControl(envelopeTimelineControl);
    timelineControl->SetParameterControl(parameterControl);

    envelopeTimelineControl->Bind(wxEVT_MOUSEWHEEL, &NesoraDictionalyTimelinePanel::OnMouseWheel, this);
    envelopeTimelineControl->Bind(wxEVT_MAGNIFY, &NesoraDictionalyTimelinePanel::OnMagnify, this);
    parameterControl->Bind(wxEVT_MOUSEWHEEL, &NesoraDictionalyTimelinePanel::OnMouseWheel, this);
    parameterControl->Bind(wxEVT_MAGNIFY, &NesoraDictionalyTimelinePanel::OnMagnify, this);
    timelineControl->Bind(wxEVT_MOUSEWHEEL, &NesoraDictionalyTimelinePanel::OnMouseWheel, this);
    timelineControl->Bind(wxEVT_MAGNIFY, &NesoraDictionalyTimelinePanel::OnMagnify, this);
    parameterComboBox->Bind(wxEVT_CHOICE, &NesoraDictionalyTimelinePanel::OnParameterChoice, this);
    parameterComboBox->Bind(wxEVT_COMBOBOX_DROPDOWN, &NesoraDictionalyTimelinePanel::OnDropdown, this);

    SetSizer(mainSizer);

    Bind(wxEVT_SIZE, &NesoraDictionalyTimelinePanel::OnSize, this);
}

void NesoraDictionalyTimelinePanel::OnMouseWheel(wxMouseEvent& event) {
    int rotation = event.GetWheelRotation();
    int linesPerAction = event.GetLinesPerAction();
    int scrollAmount = (rotation / event.GetWheelDelta()) * linesPerAction * ppux;
    xOffset += scrollAmount;
    
    if (timelineControl) {
        timelineControl->SetScrollOffset(xOffset);
    }
    if (envelopeTimelineControl) {
        envelopeTimelineControl->SetScrollOffset(xOffset);
    }
    if (parameterControl) {
        parameterControl->SetScrollOffset(xOffset);
    }
    horizontalScrollBar->SetThumbPosition(xOffset);

    event.Skip();
}

void NesoraDictionalyTimelinePanel::OnSize(wxSizeEvent& event) {
    horizontalScrollBar->SetScrollbar(xOffset, GetSize().GetWidth(), width, GetSize().GetWidth());
    event.Skip();
}

void NesoraDictionalyTimelinePanel::OnMagnify(wxMouseEvent& event) {
    if (timelineControl) {
        wxPoint2DDouble center(event.GetX(), event.GetY());
        xOffset = timelineControl->DoZoom(0, event.GetMagnification() * 100.0, center);
    }
    if (envelopeTimelineControl) {
        wxPoint2DDouble center(event.GetX(), event.GetY());
        xOffset = envelopeTimelineControl->DoZoom(0, event.GetMagnification() * 100.0, center);
    }
    if (parameterControl) {
        wxPoint2DDouble center(event.GetX(), event.GetY());
        xOffset = parameterControl->DoZoom(0, event.GetMagnification() * 100.0, center);
    }
    horizontalScrollBar->SetThumbPosition(xOffset);
    event.Skip();
}

void NesoraDictionalyTimelinePanel::SetWord(ParametricNesoraDictionalyWord* word) {
    this->word = word;
    if (timelineControl) {
        timelineControl->SetWord(word);
    }
    if (envelopeTimelineControl) {
        envelopeTimelineControl->SetWord(word);
    }
    if (parameterControl) {
        parameterControl->SetWord(word);
    }
}

void NesoraDictionalyTimelinePanel::OnParameterChoice(wxCommandEvent& event) {
    int selection = parameterComboBox->GetSelection();
    if (parameterControl) {
        parameterControl->SetSelectedParameterID(selection);
    }
}

void NesoraDictionalyTimelinePanel::OnDropdown(wxCommandEvent& event) {
    // Handle dropdown event if needed
    
}








// MARK: NesoraDictionalyEditPanel

void NesoraDictionalyEditPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    titlePanel = new NesoraDictionalyTitlePanel(this, wxID_ANY);
    timelinePanel = new NesoraDictionalyTimelinePanel(this, wxID_ANY);
    mainSizer->Add(titlePanel, 0, wxEXPAND | wxALL);
    mainSizer->Add(timelinePanel, 1, wxEXPAND | wxALL);

    SetSizer(mainSizer);
}

void NesoraDictionalyEditPanel::SetVoice(NesoraMikomiParametricVoice* voice) {
    this->voice = voice;
}

void NesoraDictionalyEditPanel::SetWord(NesoraDictionalyIndexWord* indexWord) {
    this->indexWord = indexWord;
    if (titlePanel) {
        titlePanel->SetWord(indexWord);
    }
    if (timelinePanel) {
        timelinePanel->SetWord(&(indexWord->word));
    }
}

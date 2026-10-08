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

double ParameterValueToScreenY(double value, double height) {
    return height * (1.0 - value);
}

double ScreenYToParameterValue(double screenY, double height) {
    if (height <= 0.0) {
        return 0.0;
    }
    return 1.0 - screenY / height;
}

double ClampParameterValue(double value) {
    return std::clamp(value, 0.0, 1.0);
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
            double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;
            
            std::vector<wxPoint2DDouble> points;
            std::vector<wxPoint2DDouble> fixedPoints;
            std::vector<wxPoint2DDouble> blankPoints;

            points.push_back({(double)overlapX, (double)size.y});
            fixedPoints.push_back({(double)overlapX, (double)size.y});
            for (size_t i = 0; i < word->fixedEnvelope.size(); i++) {
                double timeX = word->fixedEnvelope[i].time * pixelPerSecond + overlapX;
                double valueY = size.y - (word->fixedEnvelope[i].value * size.y);
                points.push_back({timeX, valueY});
                fixedPoints.push_back({timeX, valueY});
            }
            for (size_t i = 0; i < word->blankEnvelope.size(); i++) {
                double timeX = word->blankEnvelope[i].time * pixelPerSecond + fixedX + dynamicWidth * pixelPerSecond;
                double valueY = size.y - (word->blankEnvelope[i].value * size.y);
                points.push_back({timeX, valueY});
                blankPoints.push_back({timeX, valueY});
            }
            points.push_back({(double)blankX,(double)size.y});
            blankPoints.push_back({(double)blankX,(double)size.y});
            gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
            gc->StrokeLines(points.size(), points.data());

            for (size_t i = 0; i < word->fixedEnvelope.size(); i++) {
                const auto& point = fixedPoints[i + 1];
                if (selectedFixedPointIndex == static_cast<int>(i)) {
                    gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
                } else {
                    gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2));
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
                }
                gc->DrawEllipse(point.m_x - clickMargin, point.m_y - clickMargin, clickMargin * 2, clickMargin * 2);
            }

            for (size_t i = 0; i < word->blankEnvelope.size(); i++) {
                const auto& point = blankPoints[i];
                if (selectedBlankPointIndex == static_cast<int>(i)) {
                    gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
                } else {
                    gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2));
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
                }
                gc->DrawEllipse(point.m_x - clickMargin, point.m_y - clickMargin, clickMargin * 2, clickMargin * 2);
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
    if (selectedFixedPointIndex >= 0 && word && selectedFixedPointIndex < static_cast<int>(word->fixedEnvelope.size())) {
        if (word->fixedEnvelope[selectedFixedPointIndex].time < 0.0) {
            word->fixedEnvelope[selectedFixedPointIndex].time = 0.0;
        } else if (word->fixedEnvelope[selectedFixedPointIndex].time > word->fixedTime) {
            word->fixedEnvelope[selectedFixedPointIndex].time = word->fixedTime;
        }
        std::sort(word->fixedEnvelope.begin(), word->fixedEnvelope.end(), [](const auto& a, const auto& b) {
            return a.time < b.time;
        });
        Refresh();
    }
    if (selectedBlankPointIndex >= 0 && word && selectedBlankPointIndex < static_cast<int>(word->blankEnvelope.size())) {
        if (word->blankEnvelope[selectedBlankPointIndex].time < 0.0) {
            word->blankEnvelope[selectedBlankPointIndex].time = 0.0;
        } else if (word->blankEnvelope[selectedBlankPointIndex].time > word->blankTime) {
            word->blankEnvelope[selectedBlankPointIndex].time = word->blankTime;
        }
        std::sort(word->blankEnvelope.begin(), word->blankEnvelope.end(), [](const auto& a, const auto& b) {
            return a.time < b.time;
        });
        Refresh();
    }
}

void NesoraDictionalyEnvelopelineControl::OnRigthtDown(wxMouseEvent& event) {
    lastMousePosition = event.GetPosition();
    if (word) {

        double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
        double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;
        double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;

        if (selectedFixedPointIndex >= 0 && selectedFixedPointIndex < static_cast<int>(word->fixedEnvelope.size())) {
            word->fixedEnvelope.erase(word->fixedEnvelope.begin() + selectedFixedPointIndex);
            selectedFixedPointIndex = -1;
        } else if (selectedBlankPointIndex >= 0 && selectedBlankPointIndex < static_cast<int>(word->blankEnvelope.size())) {
            word->blankEnvelope.erase(word->blankEnvelope.begin() + selectedBlankPointIndex);
            selectedBlankPointIndex = -1;
        } else {
            if (lastMousePosition.x >= overlapX && lastMousePosition.x < fixedX) {
                word->fixedEnvelope.push_back({(lastMousePosition.x + xOffset) / pixelPerSecond + word->overlapTime, 1.0 - ((double)lastMousePosition.y / (double)GetSize().y)});
                std::sort(word->fixedEnvelope.begin(), word->fixedEnvelope.end(), [](const auto& a, const auto& b) {
                    return a.time < b.time;
                });
            } else if (lastMousePosition.x >= fixedX + dynamicWidth * pixelPerSecond && lastMousePosition.x < blankX) {
                word->blankEnvelope.push_back({(lastMousePosition.x + xOffset) / pixelPerSecond + word->overlapTime - dynamicWidth - word->fixedTime, 1.0 - ((double)lastMousePosition.y / (double)GetSize().y)});
                std::sort(word->blankEnvelope.begin(), word->blankEnvelope.end(), [](const auto& a, const auto& b) {
                    return a.time < b.time;
                });
            }
        }
        Refresh();
    }
}

void NesoraDictionalyEnvelopelineControl::OnRightUp(wxMouseEvent& event) {
}

void NesoraDictionalyEnvelopelineControl::OnMouseMove(wxMouseEvent& event) {
    static int oldSelectedFixedPointIndex = -1, oldSelectedFBlankPointIndex = -1;
    if (isDragging) {
        wxPoint currentPosition = event.GetPosition();
        int deltaX = currentPosition.x - lastMousePosition.x;
        int deltaY = currentPosition.y - lastMousePosition.y;


        double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
        double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;
        double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;

        if (selectedFixedPointIndex >= 0 && selectedFixedPointIndex < static_cast<int>(word->fixedEnvelope.size())) {
            word->fixedEnvelope[selectedFixedPointIndex].time = (currentPosition.x + xOffset) / pixelPerSecond + word->overlapTime;
            word->fixedEnvelope[selectedFixedPointIndex].value = 1.0 - ((double)currentPosition.y / (double)GetSize().y);
            if (word->fixedEnvelope[selectedFixedPointIndex].value < 0.0) {
                word->fixedEnvelope[selectedFixedPointIndex].value = 0.0;
            } else if (word->fixedEnvelope[selectedFixedPointIndex].value > 1.0) {
                word->fixedEnvelope[selectedFixedPointIndex].value = 1.0;
            }
            if (word->fixedEnvelope[selectedFixedPointIndex].time < 0.0) {
                word->fixedEnvelope[selectedFixedPointIndex].time = 0.0;
            } else if (word->fixedEnvelope[selectedFixedPointIndex].time > word->fixedTime) {
                word->fixedEnvelope[selectedFixedPointIndex].time = word->fixedTime;
            }
        } else if (selectedBlankPointIndex >= 0 && selectedBlankPointIndex < static_cast<int>(word->blankEnvelope.size())) {
            word->blankEnvelope[selectedBlankPointIndex].time = (currentPosition.x + xOffset) / pixelPerSecond - dynamicWidth - (word->fixedTime - word->overlapTime);
            word->blankEnvelope[selectedBlankPointIndex].value = 1.0 - ((double)currentPosition.y / (double)GetSize().y);
            if (word->blankEnvelope[selectedBlankPointIndex].value < 0.0) {
                word->blankEnvelope[selectedBlankPointIndex].value = 0.0;
            } else if (word->blankEnvelope[selectedBlankPointIndex].value > 1.0) {
                word->blankEnvelope[selectedBlankPointIndex].value = 1.0;
            }
            if (word->blankEnvelope[selectedBlankPointIndex].time < 0.0) {
                word->blankEnvelope[selectedBlankPointIndex].time = 0.0;
            } else if (word->blankEnvelope[selectedBlankPointIndex].time > word->blankTime) {
                word->blankEnvelope[selectedBlankPointIndex].time = word->blankTime;
            }
        }

        lastMousePosition = currentPosition;
        Refresh();
    } else {
        // マウスポインタの更新
        if (word) {
            SetCursor(wxCursor(wxCURSOR_ARROW));

            selectedFixedPointIndex = -1;
            for (int i = 0; i < word->fixedEnvelope.size(); i++) {
                double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
                double timeX = word->fixedEnvelope[i].time * pixelPerSecond + overlapX;
                double valueY = GetSize().y - (word->fixedEnvelope[i].value * GetSize().y);
                if (std::abs(event.GetPosition().x - timeX) <= clickMargin && std::abs(event.GetPosition().y - valueY) <= clickMargin) {
                    SetCursor(wxCursor(wxCURSOR_HAND));
                    selectedFixedPointIndex = i;
                    break;
                }
            }
            selectedBlankPointIndex = -1;
            for (int i = 0; i < word->blankEnvelope.size(); i++) {
                double timeX = (word->blankEnvelope[i].time + (word->fixedTime - word->overlapTime) + dynamicWidth) * pixelPerSecond - xOffset;
                double valueY = GetSize().y - (word->blankEnvelope[i].value * GetSize().y);
                if (std::abs(event.GetPosition().x - timeX) <= clickMargin && std::abs(event.GetPosition().y - valueY) <= clickMargin) {
                    SetCursor(wxCursor(wxCURSOR_HAND));
                    selectedBlankPointIndex = i;
                    break;
                }
            }
        }
    }

    if ((event.Dragging() && HasCapture()) || selectedFixedPointIndex != oldSelectedFixedPointIndex) {
        Refresh();
        oldSelectedFixedPointIndex = selectedFixedPointIndex;
    }
    if ((event.Dragging() && HasCapture()) || selectedBlankPointIndex != oldSelectedFBlankPointIndex) {
        Refresh();
        oldSelectedFBlankPointIndex = selectedBlankPointIndex;
    }
    event.Skip();
}





// MARK: ParameterControl

void NesoraDictionalyParameterControl::Init() {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, 100));

    Bind(wxEVT_PAINT, &NesoraDictionalyParameterControl::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraDictionalyParameterControl::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &NesoraDictionalyParameterControl::OnLeftUp, this);
    Bind(wxEVT_RIGHT_DOWN, &NesoraDictionalyParameterControl::OnRightDown, this);
    Bind(wxEVT_RIGHT_UP, &NesoraDictionalyParameterControl::OnRightUp, this);
    Bind(wxEVT_MOTION, &NesoraDictionalyParameterControl::OnMouseMove, this);
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
        if (word && !selectedParameterID.empty()) {
            const auto fixedParameter = word->fixedParameterValues.find(selectedParameterID);
            const auto blankParameter = word->blankParameterValues.find(selectedParameterID);
            const auto* fixedValues = fixedParameter == word->fixedParameterValues.end() ? nullptr : &fixedParameter->second;
            const auto* blankValues = blankParameter == word->blankParameterValues.end() ? nullptr : &blankParameter->second;
            double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
            double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;
            double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;
            std::vector<wxPoint2DDouble> points;
            std::vector<wxPoint2DDouble> fixedPoints;
            std::vector<wxPoint2DDouble> blankPoints;
            const double zeroY = ParameterValueToScreenY(0.0, size.y);

            points.push_back({overlapX, zeroY});
            fixedPoints.push_back({overlapX, zeroY});
            if (fixedValues) {
                for (const auto& value : *fixedValues) {
                    double timeX = value.time * pixelPerSecond + overlapX;
                    double valueY = ParameterValueToScreenY(value.value, size.y);
                    points.push_back({timeX, valueY});
                    fixedPoints.push_back({timeX, valueY});
                }
            }
            if (blankValues) {
                for (const auto& value : *blankValues) {
                    double timeX = (value.time + word->fixedTime - word->overlapTime + dynamicWidth) * pixelPerSecond - xOffset;
                    double valueY = ParameterValueToScreenY(value.value, size.y);
                    points.push_back({timeX, valueY});
                    blankPoints.push_back({timeX, valueY});
                }
            }
            points.push_back({blankX, zeroY});
            gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
            gc->StrokeLines(points.size(), points.data());
            gc->StrokeLine(0.0, zeroY, size.x, zeroY);

            if (fixedValues) {
                for (size_t i = 0; i < fixedValues->size(); ++i) {
                    const auto& point = fixedPoints[i + 1];
                    bool selected = selectedFixedPointIndex == static_cast<int>(i);
                    gc->SetPen(wxPen(nsGetColor(selected ? nsColorType::SECONDARY : nsColorType::PRIMARY), 2));
                    gc->SetBrush(wxBrush(nsGetColor(selected ? nsColorType::SECONDARY : nsColorType::BACKGROUND)));
                    gc->DrawEllipse(point.m_x - clickMargin, point.m_y - clickMargin, clickMargin * 2, clickMargin * 2);
                }
            }
            if (blankValues) {
                for (size_t i = 0; i < blankValues->size(); ++i) {
                    const auto& point = blankPoints[i];
                    bool selected = selectedBlankPointIndex == static_cast<int>(i);
                    gc->SetPen(wxPen(nsGetColor(selected ? nsColorType::SECONDARY : nsColorType::PRIMARY), 2));
                    gc->SetBrush(wxBrush(nsGetColor(selected ? nsColorType::SECONDARY : nsColorType::BACKGROUND)));
                    gc->DrawEllipse(point.m_x - clickMargin, point.m_y - clickMargin, clickMargin * 2, clickMargin * 2);
                }
            }
        }

        delete gc;
    }
}

void NesoraDictionalyParameterControl::OnLeftDown(wxMouseEvent& event) {
    isDragging = true;
    lastMousePosition = event.GetPosition();
    CaptureMouse();
}

void NesoraDictionalyParameterControl::OnLeftUp(wxMouseEvent& event) {
    isDragging = false;
    if (HasCapture()) {
        ReleaseMouse();
    }
    if (!word || selectedParameterID.empty()) {
        return;
    }

    auto fixedParameter = word->fixedParameterValues.find(selectedParameterID);
    if (fixedParameter != word->fixedParameterValues.end() && selectedFixedPointIndex >= 0 && selectedFixedPointIndex < static_cast<int>(fixedParameter->second.size())) {
        std::sort(fixedParameter->second.begin(), fixedParameter->second.end(), [](const auto& left, const auto& right) { return left.time < right.time; });
    }
    auto blankParameter = word->blankParameterValues.find(selectedParameterID);
    if (blankParameter != word->blankParameterValues.end() && selectedBlankPointIndex >= 0 && selectedBlankPointIndex < static_cast<int>(blankParameter->second.size())) {
        std::sort(blankParameter->second.begin(), blankParameter->second.end(), [](const auto& left, const auto& right) { return left.time < right.time; });
    }
    Refresh();
}

void NesoraDictionalyParameterControl::OnRightDown(wxMouseEvent& event) {
    if (!word || selectedParameterID.empty()) {
        return;
    }

    auto fixedParameter = word->fixedParameterValues.find(selectedParameterID);
    auto blankParameter = word->blankParameterValues.find(selectedParameterID);
    if (selectedFixedPointIndex >= 0 && fixedParameter != word->fixedParameterValues.end() && selectedFixedPointIndex < static_cast<int>(fixedParameter->second.size())) {
        fixedParameter->second.erase(fixedParameter->second.begin() + selectedFixedPointIndex);
        selectedFixedPointIndex = -1;
    } else if (selectedBlankPointIndex >= 0 && blankParameter != word->blankParameterValues.end() && selectedBlankPointIndex < static_cast<int>(blankParameter->second.size())) {
        blankParameter->second.erase(blankParameter->second.begin() + selectedBlankPointIndex);
        selectedBlankPointIndex = -1;
    } else {
        double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
        double fixedX = (word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset;
        double blankX = (word->blankTime + word->fixedTime - word->overlapTime) * pixelPerSecond - xOffset + dynamicWidth * pixelPerSecond;
        wxPoint position = event.GetPosition();
        if (position.x >= overlapX && position.x < fixedX) {
            auto& values = word->fixedParameterValues[selectedParameterID];
            values.push_back({(position.x + xOffset) / pixelPerSecond + word->overlapTime, ClampParameterValue(ScreenYToParameterValue(position.y, GetSize().y))});
            std::sort(values.begin(), values.end(), [](const auto& left, const auto& right) { return left.time < right.time; });
        } else if (position.x >= fixedX + dynamicWidth * pixelPerSecond && position.x < blankX) {
            auto& values = word->blankParameterValues[selectedParameterID];
            values.push_back({(position.x + xOffset) / pixelPerSecond - dynamicWidth - (word->fixedTime - word->overlapTime), ClampParameterValue(ScreenYToParameterValue(position.y, GetSize().y))});
            std::sort(values.begin(), values.end(), [](const auto& left, const auto& right) { return left.time < right.time; });
        }
    }
    Refresh();
}

void NesoraDictionalyParameterControl::OnRightUp(wxMouseEvent& event) {
}

void NesoraDictionalyParameterControl::OnMouseMove(wxMouseEvent& event) {
    if (!word || selectedParameterID.empty()) {
        return;
    }

    auto fixedParameter = word->fixedParameterValues.find(selectedParameterID);
    auto blankParameter = word->blankParameterValues.find(selectedParameterID);
    if (isDragging) {
        wxPoint currentPosition = event.GetPosition();
        if (selectedFixedPointIndex >= 0 && fixedParameter != word->fixedParameterValues.end() && selectedFixedPointIndex < static_cast<int>(fixedParameter->second.size())) {
            auto& value = fixedParameter->second[selectedFixedPointIndex];
            value.time = std::clamp((currentPosition.x + xOffset) / pixelPerSecond + word->overlapTime, 0.0, word->fixedTime);
            value.value = ClampParameterValue(ScreenYToParameterValue(currentPosition.y, GetSize().y));
        } else if (selectedBlankPointIndex >= 0 && blankParameter != word->blankParameterValues.end() && selectedBlankPointIndex < static_cast<int>(blankParameter->second.size())) {
            auto& value = blankParameter->second[selectedBlankPointIndex];
            value.time = std::clamp((currentPosition.x + xOffset) / pixelPerSecond - dynamicWidth - (word->fixedTime - word->overlapTime), 0.0, word->blankTime);
            value.value = ClampParameterValue(ScreenYToParameterValue(currentPosition.y, GetSize().y));
        }
        lastMousePosition = currentPosition;
        Refresh();
        return;
    }

    selectedFixedPointIndex = -1;
    if (fixedParameter != word->fixedParameterValues.end()) {
        double overlapX = -word->overlapTime * pixelPerSecond - xOffset;
        for (size_t i = 0; i < fixedParameter->second.size(); ++i) {
            double timeX = fixedParameter->second[i].time * pixelPerSecond + overlapX;
            double valueY = ParameterValueToScreenY(fixedParameter->second[i].value, GetSize().y);
            if (std::abs(event.GetPosition().x - timeX) <= clickMargin && std::abs(event.GetPosition().y - valueY) <= clickMargin) {
                selectedFixedPointIndex = static_cast<int>(i);
                break;
            }
        }
    }
    selectedBlankPointIndex = -1;
    if (blankParameter != word->blankParameterValues.end()) {
        for (size_t i = 0; i < blankParameter->second.size(); ++i) {
            double timeX = (blankParameter->second[i].time + word->fixedTime - word->overlapTime + dynamicWidth) * pixelPerSecond - xOffset;
            double valueY = ParameterValueToScreenY(blankParameter->second[i].value, GetSize().y);
            if (std::abs(event.GetPosition().x - timeX) <= clickMargin && std::abs(event.GetPosition().y - valueY) <= clickMargin) {
                selectedBlankPointIndex = static_cast<int>(i);
                break;
            }
        }
    }
    SetCursor(wxCursor(selectedFixedPointIndex >= 0 || selectedBlankPointIndex >= 0 ? wxCURSOR_HAND : wxCURSOR_ARROW));
    Refresh();
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
    parameterChoice = new wxChoice(mainSizer->GetStaticBox(), wxID_ANY);
    parameterSizer->Add(parameterChoice, 0, wxEXPAND | wxALL);
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
    parameterChoice->Bind(wxEVT_CHOICE, &NesoraDictionalyTimelinePanel::OnParameterChoice, this);
    parameterChoice->Bind(wxEVT_LEFT_DOWN, &NesoraDictionalyTimelinePanel::OnDropdown, this);

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

void NesoraDictionalyTimelinePanel::SetVoice(NesoraMikomiVoice* voice) {
    this->voice = voice;
    if (timelineControl) {
        timelineControl->SetVoice(voice);
    }
    if (envelopeTimelineControl) {
        envelopeTimelineControl->SetVoice(voice);
    }
    if (parameterControl) {
        parameterControl->SetVoice(voice);
    }
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
    int selection = parameterChoice->GetSelection();
    if (selection != wxNOT_FOUND && parameterControl) {
        parameterControl->SetSelectedParameterID(parameterChoice->GetString(selection).ToStdString());
    }

    event.Skip();
}

void NesoraDictionalyTimelinePanel::OnDropdown(wxMouseEvent& event) {
    if (voice == nullptr or voice->GetCurrentParameters() == nullptr) {
        parameterChoice->Clear();
        event.Skip();
        return;
    }

    int selection = parameterChoice->GetSelection();
    std::string selectedParameterID;
    if (selection != wxNOT_FOUND) {
        selectedParameterID = parameterChoice->GetString(selection).ToStdString();
    }

    parameterChoice->Clear();
    for (const auto& [parameterID, value] : *voice->GetCurrentParameters()) {
        parameterChoice->Append(parameterID);
        if (parameterID == selectedParameterID) {
            parameterChoice->SetSelection(parameterChoice->GetCount() - 1);
        }
    }

    event.Skip();
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

void NesoraDictionalyEditPanel::SetVoice(NesoraMikomiVoice* voice) {
    this->voice = voice;
    if (timelinePanel) {
        timelinePanel->SetVoice(voice);
    }
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

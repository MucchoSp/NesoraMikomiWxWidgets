// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraMIDIPanel.h"

// MARK:NesoraPianoRollCanvas

void NesoraPianoRollCanvas::Init() {
    // 背景のちらつきを防ぐ設定
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    // スクロール範囲の設定（横2000px, 縦: 128鍵分 * 20px）
    int ppux = 8;
    int ppuy = 8;
    int scrollSizex = 2000 / ppux;
    int scrollSizey = 128 * NESORA_MIDI_PANEL_NOTE_HEIGHT / ppuy;
    SetScrollbars(ppux, ppuy, scrollSizex, scrollSizey);

    // イベントバインド
    Bind(wxEVT_PAINT, &NesoraPianoRollCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraPianoRollCanvas::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &NesoraPianoRollCanvas::OnLeftUp, this);
    Bind(wxEVT_RIGHT_DOWN, &NesoraPianoRollCanvas::OnRightDown, this);
    Bind(wxEVT_MOTION, &NesoraPianoRollCanvas::OnMouseMove, this);
    Bind(wxEVT_KEY_DOWN, &NesoraPianoRollCanvas::OnKeyDown, this);
}

wxPoint2DDouble NesoraPianoRollCanvas::GetMousePos(const wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    int x, y;
    CalcUnscrolledPosition(pos.x, pos.y, &x, &y);
    return wxPoint2DDouble(x, y);
}

// ノートの右端エリア（リサイズハンドル）の矩形を取得
wxRect2DDouble NesoraPianoRollCanvas::GetResizeHandleRect(const MidiNoteBox& note) {
    return wxRect2DDouble(note.rect.m_x + note.rect.m_width - NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_y,
                            NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_height);
}

// 他のノートと時間が重なっているものを削除・整理する関数
void NesoraPianoRollCanvas::ResolveOverlaps(int changedNoteIdx) {
    double a_start = notes[changedNoteIdx].rect.m_x;
    double a_end = a_start + notes[changedNoteIdx].rect.m_width;

    for (size_t i = 0;i < notes.size();i++) {
        // 自分自身との比較はスキップ
        if (i == changedNoteIdx)
            continue;

        // 時間軸（X軸）での重なり判定
        double b_start = notes[i].rect.m_x;
        double b_end = b_start + notes[i].rect.m_width;

        // 重なりがない場合はスルー
        if (!(a_start < b_end && b_start < a_end)) {
            continue;
        }

        // 1. 完全重複 (AがBを完全に包む、またはBがAより小さい)
        if (a_start <= b_start && a_end >= b_end) {
            notes.erase(notes.begin() + i);
            changedNoteIdx--;
            if (i >= notes.size())
                break;
            i--;
            continue;
        }

        // 2. 内包・分割 (Bの中にAがスッポリ入る)
        if (b_start < a_start && b_end > a_end) {
            // 後ろ半分のノートを新しく作成
            MidiNoteBox splitPart = notes[i];
            splitPart.id = notes.back().id + 1;
            splitPart.rect.m_x = a_end;
            splitPart.rect.m_width = b_end - a_end;
            splitPart.startRectBuffer = splitPart.rect;
            
            // 前半部分の長さを短くする
            notes[i].rect.m_width = a_start - b_start;
            
            notes.push_back(splitPart);
            if (i >= notes.size())
                break;
            continue;
        }

        // 3. 前方重なり (AがBの左側に被る -> Bを後ろへ押し出し/短縮)
        if (a_start <= b_start && a_end < b_end) {
            notes[i].rect.m_width = b_end - a_end;
            notes[i].rect.m_x = a_end;
            continue;
        }

        // 4. 後方重なり (AがBの右側に被る -> Bの末尾を削る)
        if (a_start > b_start && a_end >= b_end) {
            notes[i].rect.m_width = a_start - b_start;
            continue;
        }
    }
}

void NesoraPianoRollCanvas::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    DoPrepareDC(dc); // スクロール位置を計算に含める
    dc.Clear();

    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    // dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        // 1. グリッド（背景）の描画
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        for (int i = 0; i < 128; ++i) {
            double y = i * NESORA_MIDI_PANEL_NOTE_HEIGHT;
            wxPoint2DDouble linePoints[] = {{ 0.0, y }, { 2000.0, y }};
            gc->StrokeLines(2, linePoints); // 水平線
        }

        // 2. ノートの描画
        for (size_t i = 0;i < notes.size();i++) {
            if (notes[i].isSelected) {
                if (hoverNoteIdx == i) {
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_HOVER)));
                    gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY_HOVER)));
                } else {
                    gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY)));
                    gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY)));
                }
            } else if (hoverNoteIdx == i) {
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
                gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY_HOVER)));
            } else {
                gc->SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
                gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY)));
            }
            gc->DrawRectangle(notes[i].rect);
        }

        // 2. 範囲選択枠の描画
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::SelectingRange) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_TRANSPARENT)));
            gc->SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));
            gc->DrawRectangle(currentSelectionRect.m_x, currentSelectionRect.m_y, currentSelectionRect.m_width, currentSelectionRect.m_height);
        }

        delete gc;
    }
}

void NesoraPianoRollCanvas::OnLeftDown(wxMouseEvent& event) {
    wxPoint mousePos = event.GetPosition();
    // 論理座標（スクロール位置を考慮した座標）を取得
    CalcUnscrolledPosition(mousePos.x, mousePos.y, &mousePos.x, &mousePos.y);
    bool hitAnyNote = false;
    startMousePos = mousePos;

    if(hoverNoteIdx == -1) {
        // Shiftを押しながらクリックで範囲選択開始
        if(event.ShiftDown()) {
            mouseDragState = NesoraPianoRollCanvasMouseDragState::SelectingRange;
            currentSelectionRect = wxRect2DDouble(mousePos.x, mousePos.y, 0, 0);
        }
        // そうでなければ選択解除
        else {
            for (auto& n : notes) {
                n.isSelected = false;
            }
            mouseDragState = NesoraPianoRollCanvasMouseDragState::AddNote;
        }

    } else {
        if (notes[hoverNoteIdx].rect.Contains(mousePos)) {
            // Shiftが押されていなければ、他の選択を解除
            if (!event.ShiftDown() && !notes[hoverNoteIdx].isSelected) {
                for (auto& note : notes)
                    note.isSelected = false;
            } else {
                for (auto& note : notes)
                    note.startRectBuffer = note.rect;
            }
            notes[hoverNoteIdx].isSelected = true;
            if (GetResizeHandleRect(notes[hoverNoteIdx]).Contains(mousePos)) {
                mouseDragState = NesoraPianoRollCanvasMouseDragState::ResizingNote;
            }
            else {
                mouseDragState = NesoraPianoRollCanvasMouseDragState::DraggingNotes;
            }
        }
        notes[hoverNoteIdx].startRectBuffer = notes[hoverNoteIdx].rect; // ドラッグ開始前の四角を保存
    }

    CaptureMouse();
    wxWindow::Refresh();
    event.Skip();
}


void NesoraPianoRollCanvas::OnMouseMove(wxMouseEvent& event) {
    int oldHoverNoteIdx = hoverNoteIdx;
    wxPoint2DDouble mousePos = GetMousePos(event);
    if (std::abs(mousePos.m_x - startMousePos.m_x) > 4 or std::abs(mousePos.m_y - startMousePos.m_y) > 4) {
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::AddNote) {
            mouseDragState = NesoraPianoRollCanvasMouseDragState::ResizingNote;
            
            MidiNoteBox newNote;
            newNote.id = notes.empty() ? 1 : notes.back().id + 1;
            newNote.rect = wxRect2DDouble(std::round(startMousePos.m_x / 16.0) * 16.0, std::round(startMousePos.m_y / NESORA_MIDI_PANEL_NOTE_HEIGHT) * NESORA_MIDI_PANEL_NOTE_HEIGHT, 0, NESORA_MIDI_PANEL_NOTE_HEIGHT);
            newNote.startRectBuffer = newNote.rect;
            newNote.isSelected = true;
            notes.push_back(newNote);
        }
    }

    // ドラッグしている場合の処理
    if (event.Dragging() && HasCapture()) {
        // ノートをドラッグしていたら
        switch (mouseDragState) {
        case NesoraPianoRollCanvasMouseDragState::DraggingNotes: {
            double dx = mousePos.m_x - startMousePos.m_x;
            double dy = mousePos.m_y - startMousePos.m_y;

            for (auto& note : notes) {
                if (note.isSelected) {
                    note.rect.m_x = note.startRectBuffer.m_x + dx;
                    note.rect.m_y = note.startRectBuffer.m_y + dy;

                    // クオンタイズ
                    note.rect.m_x = std::round(note.rect.m_x / 16.0) * 16.0;
                    note.rect.m_y = std::round(note.rect.m_y / NESORA_MIDI_PANEL_NOTE_HEIGHT) * NESORA_MIDI_PANEL_NOTE_HEIGHT;
                }
            }
        }
        break;
        // 矩形選択をしていたら
        case NesoraPianoRollCanvasMouseDragState::SelectingRange: {
            // 範囲矩形の更新
            double x = std::min(startMousePos.m_x, mousePos.m_x);
            double y = std::min(startMousePos.m_y, mousePos.m_y);
            double w = std::abs(startMousePos.m_x - mousePos.m_x);
            double h = std::abs(startMousePos.m_y - mousePos.m_y);
            currentSelectionRect = wxRect2DDouble(x, y, w, h);
            
            // リアルタイムで選択状態を更新する場合
            for (auto& note : notes) {
                if (currentSelectionRect.Intersects(note.rect)) {
                    note.isSelected = true;
                }
            }
        }
        break;
        // ノートを伸縮していたら
        case NesoraPianoRollCanvasMouseDragState::ResizingNote: {
            for (auto& note : notes) {
                if (note.isSelected) {
                    note.rect.m_width = note.startRectBuffer.m_x + std::round((mousePos.m_x - startMousePos.m_x) / NESORA_MIDI_PANEL_QUANTIME_WIDTH) * NESORA_MIDI_PANEL_QUANTIME_WIDTH - note.startRectBuffer.m_x + note.startRectBuffer.m_width;
                    if (note.rect.m_width < 0) {
                        note.rect.m_width *= -1;
                        note.rect.m_x = note.startRectBuffer.m_x - note.rect.m_width;
                    }
                }
            }
        }
        break;
        default: {
            // 特に何もなければ何もしない
        }
        }
    }
    // ドラッグしていない場合の処理
    else {
        // ホバーの処理
        hoverNoteIdx = -1;
        bool onHandle = false;
        wxPoint2DDouble mousePos = GetMousePos(event);
        for (size_t i = 0;i < notes.size();i++) {
            if (notes[i].rect.Contains(mousePos)) {
                hoverNoteIdx = i;
                if (GetResizeHandleRect(notes[i]).Contains(mousePos))
                    onHandle = true;
                break;
            }
        }
        if (onHandle) {
            SetCursor(wxCursor(wxCURSOR_SIZEWE)); // 左右矢印カーソル
        } else {
            SetCursor(wxNullCursor); // 通常カーソル
        }
    }

    if ((event.Dragging() && HasCapture()) || oldHoverNoteIdx != hoverNoteIdx)
        Refresh(false);

    event.Skip();
}

void NesoraPianoRollCanvas::OnLeftUp(wxMouseEvent& event) {
    if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes || mouseDragState == NesoraPianoRollCanvasMouseDragState::ResizingNote) {
        // 移動・リサイズしたノートを基準に他を整理
        for (size_t i = 0;i < notes.size();i++) {
            if (notes[i].isSelected)
                ResolveOverlaps(i);
        }
    }
    mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    // ノートの長さがフレーズよりも短かったら削除
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [](MidiNoteBox value) {
            return std::abs(value.rect.m_width) < NESORA_MIDI_PANEL_QUANTIME_WIDTH;
            }),
        notes.end()
    );
    if (HasCapture()) {
        ReleaseMouse();
    }
    wxWindow::Refresh();
    event.Skip();
}

void NesoraPianoRollCanvas::OnRightDown(wxMouseEvent& event) {
    // ノートを削除
    if(hoverNoteIdx != -1) {
        notes.erase(notes.begin() + hoverNoteIdx);
        hoverNoteIdx = -1;
        Refresh();
    }

    event.Skip();
}

void NesoraPianoRollCanvas::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_DELETE or event.GetKeyCode() == WXK_BACK) {
        // 選択されているノートを削除
        notes.erase(std::remove_if(notes.begin(), notes.end(), [](const MidiNoteBox& note) {
            return note.isSelected;
        }), notes.end());
        hoverNoteIdx = -1;
        Refresh();
    }
    event.Skip();
}






// MARK:NesoraMIDIPanel

void NesoraMIDIPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* sourceSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Nesora MIDI Editor"));

    pianoRoll = new NesoraPianoRollCanvas(sourceSizer->GetStaticBox());
    sourceSizer->Add(pianoRoll, 1, wxEXPAND | wxALL);
    
    this->SetSizer(sourceSizer);
}


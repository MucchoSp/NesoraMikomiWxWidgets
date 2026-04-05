// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraMIDIPanel.h"


inline std::vector<NesoraMidiNote> MIDINoteBoxToMidiNote(std::vector<MidiNoteBox>& boxes, double pixelPerSecond) {
    std::vector<NesoraMidiNote> notes;
    for (auto& box : boxes) {
        box.note.length = box.rect.m_width / pixelPerSecond * 1000.0; // 長さをmsに変換
        box.note.intensity = 1.0; // 仮に強さは常に1.0
        box.note.pitch = 440 * std::pow(2, (NESORA_MIDI_PANEL_A4_KEY_Y - (box.rect.m_y - NESORA_MIDI_PANEL_NOTE_HEIGHT / 2)) / 240.0); // ピッチを計算（A4=69とする）
        notes.push_back(box.note);
    }
    return notes;
}


// MARK:NesoraPianoRollCanvas

void NesoraPianoRollCanvas::Init() {
    // 背景のちらつきを防ぐ設定
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    // スクロール範囲の設定（横2000px, 縦: 128鍵分 * 20px）
    int ppux = 8;
    int ppuy = 8;
    int scrollSizex = 2000 / ppux;
    int scrollSizey = NESORA_MIDI_PANEL_KEY_COUNT * NESORA_MIDI_PANEL_NOTE_HEIGHT / ppuy;
    SetScrollbars(ppux, ppuy, scrollSizex, scrollSizey);

    // イベントバインド
    Bind(wxEVT_PAINT, &NesoraPianoRollCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraPianoRollCanvas::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &NesoraPianoRollCanvas::OnLeftUp, this);
    Bind(wxEVT_RIGHT_DOWN, &NesoraPianoRollCanvas::OnRightDown, this);
    Bind(wxEVT_RIGHT_UP, &NesoraPianoRollCanvas::OnRightUp, this);
    Bind(wxEVT_MOTION, &NesoraPianoRollCanvas::OnMouseMove, this);
    Bind(wxEVT_KEY_DOWN, &NesoraPianoRollCanvas::OnKeyDown, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_LINEUP, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_LINEDOWN, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_PAGEUP, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_PAGEDOWN, &NesoraPianoRollCanvas::OnScroll, this);
}

wxPoint2DDouble NesoraPianoRollCanvas::GetMousePos(const wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    int x, y;
    CalcUnscrolledPosition(pos.x, pos.y, &x, &y);
    return wxPoint2DDouble(x, y);
}

// ノートの右端エリア（リサイズハンドル）の矩形を取得
wxRect2DDouble NesoraPianoRollCanvas::GetRightResizeHandleRect(const MidiNoteBox& note) {
    return wxRect2DDouble(note.rect.m_x + note.rect.m_width - NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_y,
                            NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_height);
}

// ノートの左端エリア（リサイズハンドル）の矩形を取得
wxRect2DDouble NesoraPianoRollCanvas::GetLeftResizeHandleRect(const MidiNoteBox& note) {
    return wxRect2DDouble(note.rect.m_x,
                            note.rect.m_y,
                            NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH,
                            note.rect.m_height);
}

// 他のノートを押し出す、または整理する関数
void NesoraPianoRollCanvas::ResolveOverlaps() {
    if (notes.empty()) return;

    // 選択されているノートとそのほかのノートに分ける
    std::vector<MidiNoteBox> selectedNotes;
    std::vector<MidiNoteBox> nonSelectedNotes;
    
    for (const auto& n : notes) {
        if (n.isSelected) selectedNotes.push_back(n);
        else nonSelectedNotes.push_back(n);
    }

    // 両方のグループをX軸でソート
    auto sortFunc = [](const MidiNoteBox& a, const MidiNoteBox& b) {
        return a.rect.m_x < b.rect.m_x;
    };
    std::sort(selectedNotes.begin(), selectedNotes.end(), sortFunc);
    std::sort(nonSelectedNotes.begin(), nonSelectedNotes.end(), sortFunc);

    // 非選択ノートの間に選択ノートの塊を挿入する。
    double groupStartX = selectedNotes.empty() ? 0 : selectedNotes.front().rect.m_x;
    
    std::vector<MidiNoteBox> finalOrder;
    bool groupInserted = false;

    if (selectedNotes.empty()) {
        finalOrder = nonSelectedNotes;
    } else {
        for (const auto& n : nonSelectedNotes) {
            if (!groupInserted && (n.rect.m_x + n.rect.m_width / 2.0) >= groupStartX) {
                for (const auto& sn : selectedNotes) finalOrder.push_back(sn);
                groupInserted = true;
            }
            finalOrder.push_back(n);
        }
        
        if (!groupInserted) {
            for (const auto& sn : selectedNotes) finalOrder.push_back(sn);
        }
    }

    // 隙間なく並べる
    double currentX = 0.0;
    for (size_t i = 0; i < finalOrder.size(); i++) {
        finalOrder[i].rect.m_x = currentX;
        currentX += finalOrder[i].rect.m_width;
    }

    notes = finalOrder;

    midiScript.SetNotes(MIDINoteBoxToMidiNote(finalOrder, pixelPerBeet * bpm / 60.0));
    double samplePerPixel = midiScript.GetSamplingFrequency() / (pixelPerBeet * bpm / 60.0);
    pitchLine = midiScript.GetPitchLinePerSample(midiScript.GetSamplingFrequency() / (pixelPerBeet * bpm / 60.0));
    MakePitchLine();
}

void NesoraPianoRollCanvas::MakePitchLine() {
    // ピッチラインの例: 単純にノートの中央を結ぶ線
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
        // グリッド（背景）の描画
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        for (int i = 0; i < 128; ++i) {
            double y = i * NESORA_MIDI_PANEL_NOTE_HEIGHT;
            wxPoint2DDouble linePoints[] = {{ 0.0, y }, { 2000.0, y }};
            gc->StrokeLines(2, linePoints); // 水平線
        }

        // ノートの描画
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

        // 範囲選択枠の描画
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::SelectingRange) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_TRANSPARENT)));
            gc->SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));
            gc->DrawRectangle(currentSelectionRect.m_x, currentSelectionRect.m_y, currentSelectionRect.m_width, currentSelectionRect.m_height);
        }

        // ピッチラインの描画
        if (pitchLine.size() >= 2) {
            gc->SetPen(wxPen(nsGetColor(nsColorType::SECONDARY), 2));
            std::vector<wxPoint2DDouble> linePoints;
            for (size_t i = 0; i < pitchLine.size(); i++) {
                linePoints.push_back(wxPoint2DDouble(i, std::log2(pitchLine[i] / 440.0) * -240.0 + NESORA_MIDI_PANEL_A4_KEY_Y + NESORA_MIDI_PANEL_NOTE_HEIGHT)); // ピッチをY座標に変換
            }
            gc->StrokeLines(linePoints.size(), linePoints.data());
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
        // 範囲選択開始
        if (tookAction) {
            for (auto& n : notes)
                n.isSelected = false;
            tookAction = false;
        }
        mouseDragState = NesoraPianoRollCanvasMouseDragState::SelectingRange;
        currentSelectionRect = wxRect2DDouble(mousePos.x, mousePos.y, 0, 0);
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
            if (GetRightResizeHandleRect(notes[hoverNoteIdx]).Contains(mousePos)) {
                mouseDragState = NesoraPianoRollCanvasMouseDragState::ResizingNote;
            }
            else if (GetLeftResizeHandleRect(notes[hoverNoteIdx]).Contains(mousePos)) {
                notes[hoverNoteIdx].rect.m_x += notes[hoverNoteIdx].rect.m_width; // 左端をドラッグする場合、基準点を右端に変更
                notes[hoverNoteIdx].rect.m_width = -notes[hoverNoteIdx].rect.m_width;
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
            
            // 既存のノートの中で、今回の開始地点の直前にあるもののリスト
            double prevEnd = 0.0;
            for (const auto& n : notes) {
                double endX = n.rect.m_x + n.rect.m_width;
                if (endX <= startMousePos.m_x && endX > prevEnd) {
                    prevEnd = endX;
                }
            }

            MidiNoteBox newNote;
            newNote.id = notes.empty() ? 1 : notes.back().id + 1;
            // 直前のノートの末尾、もしくは0から開始するように修正
            newNote.rect = wxRect2DDouble(prevEnd, std::floor(startMousePos.m_y / NESORA_MIDI_PANEL_NOTE_HEIGHT) * NESORA_MIDI_PANEL_NOTE_HEIGHT, 0, NESORA_MIDI_PANEL_NOTE_HEIGHT);
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
                    note.rect.m_x = std::floor(note.rect.m_x / 16.0) * 16.0;
                    note.rect.m_y = std::floor(note.rect.m_y / NESORA_MIDI_PANEL_NOTE_HEIGHT) * NESORA_MIDI_PANEL_NOTE_HEIGHT;
                }
            }
            tookAction = true;
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
                    note.rect.m_width = note.startRectBuffer.m_x + std::floor((mousePos.m_x - startMousePos.m_x) / NESORA_MIDI_PANEL_QUANTIME_WIDTH) * NESORA_MIDI_PANEL_QUANTIME_WIDTH - note.startRectBuffer.m_x + note.startRectBuffer.m_width;
                    if (note.rect.m_width < 0) {
                        note.rect.m_width *= -1;
                        note.rect.m_x = note.startRectBuffer.m_x - note.rect.m_width;
                    }
                }
            }
            tookAction = true;
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
                if (GetRightResizeHandleRect(notes[i]).Contains(mousePos) or GetLeftResizeHandleRect(notes[i]).Contains(mousePos))
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

    if ((event.Dragging() && HasCapture()) || oldHoverNoteIdx != hoverNoteIdx) {
        // if (isNotePreview) いつか作る
        //     ResolveOverlaps();
        Refresh(false);
    }

    event.Skip();
}

void NesoraPianoRollCanvas::OnLeftUp(wxMouseEvent& event) {
    wxPoint2DDouble mousePos = GetMousePos(event);
    // ノートの長さがフレーズよりも短かったら削除
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [](MidiNoteBox value) {
            return std::abs(value.rect.m_width) < NESORA_MIDI_PANEL_QUANTIME_WIDTH;
            }),
        notes.end()
    );
    if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes || mouseDragState == NesoraPianoRollCanvasMouseDragState::ResizingNote) {
        if (std::abs(mousePos.m_x - startMousePos.m_x) > 4 or std::abs(mousePos.m_y - startMousePos.m_y) > 4) 
            ResolveOverlaps();
    }
    if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes) {
        if (std::abs(mousePos.m_x - startMousePos.m_x) < 4 and std::abs(mousePos.m_y - startMousePos.m_y) < 4) {
            if (!event.ShiftDown()) {
                for (auto& note : notes)
                    note.isSelected = false;
            }
            notes[hoverNoteIdx].isSelected = true;
        }
    }
    mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    if (HasCapture()) {
        ReleaseMouse();
    }
    wxWindow::Refresh();
    event.Skip();
}

void NesoraPianoRollCanvas::OnRightDown(wxMouseEvent& event) {
    wxPoint mousePos = event.GetPosition();
    // 論理座標（スクロール位置を考慮した座標）を取得
    CalcUnscrolledPosition(mousePos.x, mousePos.y, &mousePos.x, &mousePos.y);
    bool hitAnyNote = false;
    startMousePos = mousePos;

    // ノートを追加
    if(hoverNoteIdx == -1) {
        for (auto& n : notes) {
            n.isSelected = false;
        }
        mouseDragState = NesoraPianoRollCanvasMouseDragState::AddNote;
    } else {
        notes.erase(notes.begin() + hoverNoteIdx);
        hoverNoteIdx = -1;
        ResolveOverlaps();
    }
    CaptureMouse();
    wxWindow::Refresh();
    event.Skip();
}

void NesoraPianoRollCanvas::OnRightUp(wxMouseEvent& event) {
    // ノートの長さがフレーズよりも短かったら削除
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [](MidiNoteBox value) {
            return std::abs(value.rect.m_width) < NESORA_MIDI_PANEL_QUANTIME_WIDTH;
            }),
        notes.end()
    );
    if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes || mouseDragState == NesoraPianoRollCanvasMouseDragState::ResizingNote) {
        ResolveOverlaps();
    }
    mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    if (HasCapture()) {
        ReleaseMouse();
    }
    wxWindow::Refresh();
    event.Skip();
}

void NesoraPianoRollCanvas::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_DELETE or event.GetKeyCode() == WXK_BACK) {
        // 選択されているノートを削除
        notes.erase(std::remove_if(notes.begin(), notes.end(), [](const MidiNoteBox& note) {
            return note.isSelected;
        }), notes.end());
        hoverNoteIdx = -1;
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == WXK_UP) {
        // 選択されているノートを半音上げる
        for (auto& note : notes) {
            if (note.isSelected) {
                note.rect.m_y -= NESORA_MIDI_PANEL_NOTE_HEIGHT;
            }
        }
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == WXK_DOWN) {
        // 選択されているノートを半音下げる
        for (auto& note : notes) {
            if (note.isSelected) {
                note.rect.m_y += NESORA_MIDI_PANEL_NOTE_HEIGHT;
            }
        }
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == WXK_LEFT) {
        // 選択されているノートを左に移動
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == WXK_RIGHT) {
        // 選択されているノートを右に移動
        ResolveOverlaps();
        Refresh();
    }
    if (event.GetKeyCode() == 'A' && event.ControlDown()) {
        // 全てのノートを選択
        for (auto& note : notes) {
            note.isSelected = true;
        }
        hoverNoteIdx = -1;
        Refresh();
    }
    if (event.GetKeyCode() == 'D' && event.ControlDown()) {
        // 選択されているノートを複製
        std::vector<MidiNoteBox> newNotes;
        for (const auto& note : notes) {
            if (note.isSelected) {
                MidiNoteBox newNote = note;
                newNote.id = notes.empty() ? 1 : notes.back().id + 1;
                newNote.rect.m_x += 16; // 少し右にずらして複製
                newNotes.push_back(newNote);
            }
        }
        notes.insert(notes.end(), newNotes.begin(), newNotes.end());
        ResolveOverlaps();
        Refresh();
    }

    event.Skip();
}

void NesoraPianoRollCanvas::OnScroll(wxScrollWinEvent& event) {
    event.Skip(); // デフォルトのスクロール処理を実行

    int x, y;
    GetViewStart(&x, &y);
    int ppux, ppuy;
    GetScrollPixelsPerUnit(&ppux, &ppuy);

    if (m_linkedTimeline) m_linkedTimeline->SetScrollOffset(x * ppux);
    if (m_linkedKeys) m_linkedKeys->SetScrollOffset(y * ppuy);
}


// MARK:NesoraPianoKeys

void NesoraPianoKeys::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    wxSize size = GetClientSize();
    for (int i = 0; i < 128; ++i) {
        double y = i * NESORA_MIDI_PANEL_NOTE_HEIGHT - m_yOffset;
        
        // 画面外の描画をスキップ
        if (y + NESORA_MIDI_PANEL_NOTE_HEIGHT < 0 || y > size.GetHeight()) continue;

        int noteInOctave = (127 - i) % 12;
        bool isBlackKey = (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || noteInOctave == 8 || noteInOctave == 10);

        if (isBlackKey) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PIANOKEY_BLACK)));
        } else {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PIANOKEY_WHITE)));
        }
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->DrawRectangle(0, y, size.GetWidth(), NESORA_MIDI_PANEL_NOTE_HEIGHT);

        // Cの音のラベル
        if (noteInOctave == 0) {
            int octave = (127 - i) / 12 - 1;
            gc->SetFont(GetFont(), nsGetColor(nsColorType::PIANOKEY_BLACK));
            gc->DrawText(wxString::Format("C%d", octave), 5, y + 2);
        }
    }
    delete gc;
}

// MARK:NesoraTimeline

void NesoraTimeline::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    wxSize size = GetClientSize();
    gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    gc->SetFont(GetFont(), nsGetColor(nsColorType::ON_BACKGROUND));

    for (int x = 0; x < 2000; x += m_barWidth) {
        double drawX = x - m_xOffset;
        if (drawX + m_barWidth < 0 || drawX > size.GetWidth()) continue;

        gc->StrokeLine(drawX, size.GetHeight() - 10, drawX, size.GetHeight());
        gc->DrawText(wxString::Format("%d", x / m_barWidth + 1), drawX + 2, 2);
    }
    delete gc;
}


// MARK:NesoraMIDIPanel

void NesoraMIDIPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI Editor"));

    // ピアノロール（中央）を先に作成して、リンクさせる
    pianoRoll = new NesoraPianoRollCanvas(mainSizer->GetStaticBox());
    
    // タイムライン（上部）
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->AddSpacer(60); // ピアノ鍵盤の幅分のスペースを確保
    NesoraTimeline* timeline = new NesoraTimeline(mainSizer->GetStaticBox());
    timeline->SetMinSize(wxSize(-1, 30));
    topSizer->Add(timeline, 1, wxEXPAND);

    wxBoxSizer* middleSizer = new wxBoxSizer(wxHORIZONTAL);

    // ピアノ鍵盤（左側）
    NesoraPianoKeys* keys = new NesoraPianoKeys(mainSizer->GetStaticBox());
    keys->SetMinSize(wxSize(60, -1));
    middleSizer->Add(keys, 0, wxEXPAND);

    middleSizer->Add(pianoRoll, 1, wxEXPAND);

    // スクロール同期設定
    pianoRoll->m_linkedKeys = keys;
    pianoRoll->m_linkedTimeline = timeline;

    mainSizer->Add(topSizer, 0, wxEXPAND);
    mainSizer->Add(middleSizer, 1, wxEXPAND);
    
    this->SetSizer(mainSizer);
}


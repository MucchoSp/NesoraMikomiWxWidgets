// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraMIDIPanel.h"

// MARK: 座標変換関数群(参考用)
inline double TimeToPixel(double timeInMs, double pixelPerSecond) {
    return timeInMs / 1000.0 * pixelPerSecond;
}

inline double PixelToTime(double pixel, double pixelPerSecond) {
    return pixel / pixelPerSecond * 1000.0;
}

inline double PitchToPixel(double pitch, double pixelPerNote, double A4KeyY) {
    return A4KeyY - (pixelPerNote * 12.0) * std::log2(pitch / 440.0);
}

inline double PixelToPitch(double pixel, double pixelPerNote, double A4KeyY) {
    return 440.0 * std::pow(2.0, (A4KeyY - pixel) / (pixelPerNote * 12.0));
}

inline double BeetToPixel(double beet, double pixelPerBeet) {
    return beet * pixelPerBeet;
}

inline double PixelToBeet(double pixel, double pixelPerBeet) {
    return pixel / pixelPerBeet;
}

inline double BarToPixel(double bar, double pixelPerBeet, double timeSignatureNumerator) {
    return bar * timeSignatureNumerator * pixelPerBeet;
}

inline double PixelToBar(double pixel, double pixelPerBeet, double timeSignatureNumerator) {
    return pixel / (pixelPerBeet * timeSignatureNumerator);
}

inline double BarToBeet(double bar, double timeSignatureNumerator) {
    return bar * timeSignatureNumerator;
}

inline double BeetToBar(double beet, double timeSignatureNumerator) {
    return beet / timeSignatureNumerator;
}

inline double TimeToBeet(double timeInMs, double bpm) {
    return timeInMs / 1000.0 * (bpm / 60.0);
}

inline double BeetToTime(double beet, double bpm) {
    return beet / (bpm / 60.0) * 1000.0;
}

inline double BarToTime(double bar, double bpm, double timeSignatureNumerator) {
    return BarToBeet(bar, timeSignatureNumerator) / (bpm / 60.0) * 1000.0;
}

inline double TimeToBar(double timeInMs, double bpm, double timeSignatureNumerator) {
    return BeetToBar(TimeToBeet(timeInMs, bpm), timeSignatureNumerator);
}

inline double BeetToPixelPerSecond(double beet, double bpm) {
    return (bpm / 60.0) * (beet / TimeToBeet(1000.0, bpm));
}

inline double PixelPerSecondToBeet(double pixelPerSecond, double bpm) {
    return TimeToBeet(1000.0, bpm) * (pixelPerSecond / (bpm / 60.0));
}

// マウスポインタが一定以上動いたか
inline bool IsPointMoved(const wxPoint2DDouble& p1, const wxPoint2DDouble& p2, int threshold = 4) {
    return std::abs(p2.m_x - p1.m_x) > threshold or std::abs(p2.m_y - p1.m_y) > threshold;
}

// MIDIノートボックスのリストをMIDIノートのリストに変換する
inline std::vector<NesoraMidiNotePhoneticalInfo> MIDINoteBoxToMidiNote(std::vector<MidiNoteBox>& boxes, double pixelPerSecond, double pixelPerNote, double A4KeyY) {
    std::vector<NesoraMidiNotePhoneticalInfo> notes;
    for (auto& box : boxes) {
        box.note.length = box.rect.m_width / pixelPerSecond * 1000.0; // 長さをmsに変換
        box.note.intensity = 1.0; // 仮に強さは常に1.0
        box.note.pitch = PixelToPitch(box.rect.m_y - pixelPerNote / 2.0, pixelPerNote, A4KeyY); // ピッチを計算（A4=69とする）
        notes.push_back(box.note);
    }
    return notes;
}

MidiNoteBox NesoraPianoRollCanvas::CreateNewMidiNoteBox(MidiNoteBox box) {
    box.note.frontOverlap = 0.0;
    box.note.backOverlap = 0.0;
    box.note.strength = 1.0;
    box.note.frontTransitionTime = 0.0;
    box.note.backTransitionTime = 0.0;

    box.note.cl_length = 0.0;
    box.note.uv_length = 0.0;
    box.note.sv_length = 0.0;

    box.note.frontPitchMoveTime = 100.0;
    box.note.frontPitchMoveTimming = 50.0;
    box.note.frontPitchMoveCurve = NesoraPitchCurveType::SINE;

    box.note.modulationStrength = 50.0;
    box.note.modulationFrequency = 10.0;
    box.note.modulationStartTime = 500.0;

    box.note.modulationFadeInTime = 100.0;
    box.note.modulationFadeOutTime = 100.0;

    box.note.overshootTime = 100.0;
    box.note.overshootPitch = 10.0;

    box.note.preparationTime = 100.0;
    box.note.preparationPitch = 10.0;

    box.controlPoints[MidiNoteBoxControlPointID::FrontPitchMoveTimmingControlPoint] = wxRect2DDouble(TimeToPixel(-box.note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                                 box.rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::OvershootPitchControlPoint] = wxRect2DDouble(TimeToPixel(box.note.frontPitchMoveTime - box.note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                          box.note.overshootPitch * pixelPerNote / 100.0 + box.rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::PreparationPitchControlPoint] = wxRect2DDouble(box.rect.m_x + box.rect.m_width - TimeToPixel(box.note.preparationTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                            box.note.preparationPitch * pixelPerNote / 100.0 + box.rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint] = wxRect2DDouble(TimeToPixel(box.note.modulationStartTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                      box.rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationFrequencyControlPoint] = wxRect2DDouble(TimeToPixel(box.note.modulationStartTime + 1000.0 / box.note.modulationFrequency, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                               box.note.modulationStrength * pixelPerNote / 100.0 + box.rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint] = wxRect2DDouble(TimeToPixel(box.note.modulationStartTime + box.note.modulationFadeInTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                                box.rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
    box.controlPoints[MidiNoteBoxControlPointID::ModulationFadeOutTimeControlPoint] = wxRect2DDouble(TimeToPixel(box.note.length - box.note.preparationTime - box.note.modulationFadeOutTime, pixelPerBeet * bpm / 60.0) + box.rect.m_x - 5,
                                                                                                                 box.rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);

    return box;
}











// MARK:NesoraPianoRollCanvas

void NesoraPianoRollCanvas::Init() {
    // 背景のちらつきを防ぐ設定
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    // スクロール範囲の設定(横: 4小節, 縦: 128鍵分 * 20px)
    screenWidth = pixelPerBeet * timeSignatureNumerator * 4.0;
    screenHeight = NESORA_MIDI_PANEL_KEY_COUNT * pixelPerNote;
    SetScrollbars(ppux, ppuy, screenWidth / ppux, screenHeight / ppuy, 0, screenHeight / 2.0 / ppuy);

    // イベントバインド
    Bind(wxEVT_PAINT, &NesoraPianoRollCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NesoraPianoRollCanvas::OnLeftDown, this);
    Bind(wxEVT_LEFT_DCLICK, &NesoraPianoRollCanvas::OnLeftDClick, this);
    Bind(wxEVT_LEFT_UP, &NesoraPianoRollCanvas::OnLeftUp, this);
    Bind(wxEVT_RIGHT_DOWN, &NesoraPianoRollCanvas::OnRightDown, this);
    Bind(wxEVT_RIGHT_UP, &NesoraPianoRollCanvas::OnRightUp, this);
    Bind(wxEVT_MOTION, &NesoraPianoRollCanvas::OnMouseMove, this);
    Bind(wxEVT_KEY_DOWN, &NesoraPianoRollCanvas::OnKeyDown, this);
    Bind(wxEVT_SIZE, &NesoraPianoRollCanvas::OnSize, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_LINEUP, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_LINEDOWN, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_PAGEUP, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_SCROLLWIN_PAGEDOWN, &NesoraPianoRollCanvas::OnScroll, this);
    Bind(wxEVT_GESTURE_ZOOM, &NesoraPianoRollCanvas::OnZoom, this);
    Bind(wxEVT_MOUSEWHEEL, &NesoraPianoRollCanvas::OnMouseWheel, this);
    Bind(wxEVT_MAGNIFY, &NesoraPianoRollCanvas::OnMagnify, this);

    lyricEditor = new wxTextCtrl(this,
                                 wxID_ANY,
                                 wxEmptyString,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
    lyricEditor->Hide();
    lyricEditor->Bind(wxEVT_KILL_FOCUS, &NesoraPianoRollCanvas::OnLyricEditorKillFocus, this);
    lyricEditor->Bind(wxEVT_CHAR_HOOK, &NesoraPianoRollCanvas::OnLyricEditorCharHook, this);
}

// 歌詞の編集を開始する。noteIdxは編集するノートのインデックス
void NesoraPianoRollCanvas::BeginLyricEdit(int noteIdx) {
    if (noteIdx < 0 or noteIdx >= (int)notes.size() or lyricEditor == nullptr) {
        return;
    }

    editingNoteIdx = noteIdx;
    editingOriginalLyric = notes[noteIdx].note.lyric;

    int sx = 0;
    int sy = 0;
    CalcScrolledPosition(notes[noteIdx].rect.m_x, notes[noteIdx].rect.m_y, &sx, &sy);

    int editorW = std::max(40, (int)notes[noteIdx].rect.m_width);
    int editorH = std::max(18, (int)notes[noteIdx].rect.m_height);
    lyricEditor->SetSize(sx, sy, editorW, editorH);
    lyricEditor->SetValue(wxString::FromUTF8(notes[noteIdx].note.lyric));
    lyricEditor->Show();
    lyricEditor->SetFocus();
    lyricEditor->SelectAll();
}

// 編集を終了する。commitがtrueなら変更を保存、falseなら破棄
void NesoraPianoRollCanvas::EndLyricEdit(bool commit) {
    if (editingNoteIdx < 0 || editingNoteIdx >= (int)notes.size() || lyricEditor == nullptr) {
        editingNoteIdx = -1;
        return;
    }

    if (commit) {
        notes[editingNoteIdx].note.lyric = std::string(lyricEditor->GetValue().utf8_string());
        midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixelPerBeet * bpm / 60.0, pixelPerNote, A4KeyY));
    } else {
        notes[editingNoteIdx].note.lyric = editingOriginalLyric;
    }

    lyricEditor->Hide();
    editingNoteIdx = -1;
    Refresh(false);
}

// 選択されているノートを、Xでソートして順番を返す
std::vector<int> NesoraPianoRollCanvas::GetSelectedNoteOrder() const {
    std::vector<int> selectedIndices;
    for (size_t i = 0; i < notes.size(); ++i) {
        if (notes[i].isSelected) {
            selectedIndices.push_back(i);
        }
    }

    // ソートの基準は、X座標、次にY座標、最後にID（重複しないようにソートしてあるから不要なはず）
    // std::sort(selectedIndices.begin(), selectedIndices.end(), [this](int left, int right) {
    //     const MidiNoteBox& leftNote = notes[left];
    //     const MidiNoteBox& rightNote = notes[right];
    //     if (leftNote.rect.m_x != rightNote.rect.m_x) {
    //         return leftNote.rect.m_x < rightNote.rect.m_x;
    //     }
    //     if (leftNote.rect.m_y != rightNote.rect.m_y) {
    //         return leftNote.rect.m_y < rightNote.rect.m_y;
    //     }
    //     return leftNote.id < rightNote.id;
    // });

    return selectedIndices;
}

// 現在選択されたノートの中で、currentNoteIdxの次に来るノートのインデックスを返す。見つからない場合は-1
int NesoraPianoRollCanvas::GetNextSelectedNoteIndex(int currentNoteIdx) const {
    std::vector<int> selectedIndices = GetSelectedNoteOrder();
    auto currentIt = std::find(selectedIndices.begin(), selectedIndices.end(), currentNoteIdx);
    if (currentIt == selectedIndices.end()) {
        return selectedIndices.empty() ? -1 : selectedIndices.front();
    }

    currentIt++;
    if (currentIt == selectedIndices.end()) {
        return -1;
    }

    return *currentIt;
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
    currentX = 0.0;
    if (!notes.empty()){
        // 選択されているノートとそのほかのノートに分ける
        std::vector<MidiNoteBox> selectedNotes;
        std::vector<MidiNoteBox> nonSelectedNotes;
        
        for (const auto& n : notes) {
            if (n.isSelected)
                selectedNotes.push_back(n);
            else
                nonSelectedNotes.push_back(n);
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
                if (!groupInserted && (n.rect.m_x/* + n.rect.m_width / 2.0*/) >= groupStartX) {
                    for (const auto& sn : selectedNotes) finalOrder.push_back(sn);
                    groupInserted = true;
                }
                finalOrder.push_back(n);
            }
            if (!groupInserted) {
                for (const auto& sn : selectedNotes)
                    finalOrder.push_back(sn);
            }
        }

        // 隙間なく並べる
        for (size_t i = 0; i < finalOrder.size(); i++) {
            finalOrder[i].rect.m_x = currentX;
            currentX += finalOrder[i].rect.m_width;
        }

        notes = finalOrder;
    }

    // MIDIスクリプトのノート情報を更新
    double pixcelPerSecond = pixelPerBeet * bpm / 60.0;
    midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixcelPerSecond, pixelPerNote, A4KeyY));
    midiScript.CalculateNoteParam(pixcelPerSecond);
    pitchLine = midiScript.GetPitchLine();
    m_linkedEnvelopeline->SetEnvelopeline(midiScript.GetEnvelopeLine());
    
    // スクロール範囲の更新
    SetScrollWidth();

    // 選択されているノートが変更されたら
    ChangeSelectNote();
}

void NesoraPianoRollCanvas::PitchControlPointUpdate() {
    if (selectedNoteIdx != -1) {
        notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::FrontPitchMoveTimmingControlPoint] = wxRect2DDouble(TimeToPixel(-notes[selectedNoteIdx].note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                    notes[selectedNoteIdx].rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
        notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::OvershootPitchControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.frontPitchMoveTime - notes[selectedNoteIdx].note.frontPitchMoveTimming, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                            notes[selectedNoteIdx].note.overshootPitch * pixelPerNote / 100.0 + notes[selectedNoteIdx].rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
        notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::PreparationPitchControlPoint] = wxRect2DDouble(notes[selectedNoteIdx].rect.m_width - TimeToPixel(notes[selectedNoteIdx].note.preparationTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                notes[selectedNoteIdx].note.preparationPitch * pixelPerNote / 100.0 + notes[selectedNoteIdx].rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
        notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                        notes[selectedNoteIdx].rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
        notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFrequencyControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + 1000.0 / notes[selectedNoteIdx].note.modulationFrequency, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                -notes[selectedNoteIdx].note.modulationStrength * pixelPerNote / 100.0 + notes[selectedNoteIdx].rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
        notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + notes[selectedNoteIdx].note.modulationFadeInTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                    notes[selectedNoteIdx].rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
        notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFadeOutTimeControlPoint] = wxRect2DDouble(TimeToPixel(notes[selectedNoteIdx].note.length - notes[selectedNoteIdx].note.preparationTime - notes[selectedNoteIdx].note.modulationFadeOutTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5,
                                                                                                                    notes[selectedNoteIdx].rect.m_y - 5 + pixelPerNote / 2.0, 10, 10);
    }
}

// ピッチのコントロールポイントからピッチラインを更新する
void NesoraPianoRollCanvas::PitchLineUpdate() {
    if (selectedNoteIdx != -1 and draggingControlPointIdx != MidiNoteBoxControlPointID::None) {
        switch (draggingControlPointIdx) {
        case MidiNoteBoxControlPointID::FrontPitchMoveTimmingControlPoint: {
            notes[selectedNoteIdx].note.frontPitchMoveTimming = PixelToTime(-(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0), pixelPerBeet * bpm / 60.0);
            notes[selectedNoteIdx].note.frontPitchMoveTime = PixelToTime(notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::OvershootPitchControlPoint].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].note.frontPitchMoveTimming;
        }
        break;
        case MidiNoteBoxControlPointID::OvershootPitchControlPoint: {
            notes[selectedNoteIdx].note.frontPitchMoveTime = PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].note.frontPitchMoveTimming;
            notes[selectedNoteIdx].note.overshootPitch = (notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y - notes[selectedNoteIdx].rect.m_y + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0  - pixelPerNote / 2.0) * 100.0 / pixelPerNote;
        }
        break;
        case MidiNoteBoxControlPointID::ModulationControlPoint: {
            notes[selectedNoteIdx].note.modulationStartTime = PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0);
            notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFrequencyControlPoint].m_x = TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + 1000.0 / notes[selectedNoteIdx].note.modulationFrequency, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5;
            notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint].m_x = TimeToPixel(notes[selectedNoteIdx].note.modulationStartTime + notes[selectedNoteIdx].note.modulationFadeInTime, pixelPerBeet * bpm / 60.0) + notes[selectedNoteIdx].rect.m_x - 5;
        }
        break;
        case MidiNoteBoxControlPointID::ModulationFrequencyControlPoint: {
            notes[selectedNoteIdx].note.modulationFrequency = 1000.0 / PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint].m_x - 5 + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0);
            notes[selectedNoteIdx].note.modulationStrength = std::abs(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y - notes[selectedNoteIdx].rect.m_y + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0 - pixelPerNote / 2.0) * 100.0 / pixelPerNote;
        }
        break;
        case MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint: {
            notes[selectedNoteIdx].note.modulationFadeInTime = PixelToTime(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::ModulationControlPoint].m_x - 5 + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0, pixelPerBeet * bpm / 60.0);
        }
        break;
        case MidiNoteBoxControlPointID::ModulationFadeOutTimeControlPoint: {
            notes[selectedNoteIdx].note.modulationFadeOutTime = PixelToTime(-(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - notes[selectedNoteIdx].controlPoints[MidiNoteBoxControlPointID::PreparationPitchControlPoint].m_x - 5 + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0), pixelPerBeet * bpm / 60.0);
        }
        break;
        case MidiNoteBoxControlPointID::PreparationPitchControlPoint: {
            notes[selectedNoteIdx].note.preparationTime = PixelToTime(-(notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - (notes[selectedNoteIdx].rect.m_x + notes[selectedNoteIdx].rect.m_width) + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0), pixelPerBeet * bpm / 60.0);
            notes[selectedNoteIdx].note.preparationPitch = (notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y - notes[selectedNoteIdx].rect.m_y - notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0) * 100.0 / pixelPerNote;
        }
        default:
        break;
        }
    }
    double pixcelPerSecond = pixelPerBeet * bpm / 60.0;
    midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixcelPerSecond, pixelPerNote, A4KeyY));
    midiScript.CalculateNoteParam(pixcelPerSecond);
    pitchLine = midiScript.GetPitchLine();
    Refresh();
}

void NesoraPianoRollCanvas::EnvelopeLineUpdate() {
    double pixcelPerSecond = pixelPerBeet * bpm / 60.0;
    midiScript.SetNotes(MIDINoteBoxToMidiNote(notes, pixcelPerSecond, pixelPerNote, A4KeyY));
    midiScript.CalculateNoteParam(pixcelPerSecond);
    pitchLine = midiScript.GetPitchLine();
    if (m_linkedEnvelopeline) {
        m_linkedEnvelopeline->SetEnvelopeline(midiScript.GetEnvelopeLine());
    }
    Refresh();
}

// 選択ノートをクリア
void NesoraPianoRollCanvas::NoteSelectClear() {
    for (auto& note : notes)
        note.isSelected = false;
    
    selectedNoteIdx = -1;
    m_linkedEnvelopeline->EnvelopeControlPointUpdate(nullptr);
    return;
}

// 選択されているノートが変更されたら
void NesoraPianoRollCanvas::ChangeSelectNote() {
    // 選択されているノートを分ける
    std::vector<int> selectedNotes;
    
    for (size_t i = 0;i < notes.size();i++) {
        if (notes[i].isSelected)
            selectedNotes.push_back(i);
    }
    
    if (selectedNotes.size() == 1) {
        selectedNoteIdx = selectedNotes[0];
        PitchControlPointUpdate();
        m_linkedEnvelopeline->EnvelopeControlPointUpdate(&notes[selectedNoteIdx]);
    } else {
        selectedNoteIdx = -1;
        m_linkedEnvelopeline->EnvelopeControlPointUpdate(nullptr);
    }
}

void NesoraPianoRollCanvas::SetScrollWidth() {
    // スクロール範囲の更新
    int x, y, w, h;
    GetViewStart(&x, &y);
    GetClientSize(&w, &h);
    scriptLengthInBar = std::max(PixelToBar(w, pixelPerBeet, timeSignatureNumerator), PixelToBar(currentX, pixelPerBeet, timeSignatureNumerator) + 2.0); // 2小節分の余裕を持たせる
    screenWidth = scriptLengthInBar * pixelPerBeet * timeSignatureNumerator;
    SetScrollbars(ppux, ppuy, screenWidth / ppux, screenHeight / ppuy, x, y);
    if (m_linkedTimeline) m_linkedTimeline->SetScriptLengthInBar(scriptLengthInBar);
    if (m_linkedEnvelopeline) m_linkedEnvelopeline->SetScriptLengthInBar(scriptLengthInBar);
}

void NesoraPianoRollCanvas::DoZoom(int deltax, int deltay, const wxPoint2DDouble center) {
    double oldppb = pixelPerBeet, oldppn = pixelPerNote;
    if (deltax) {
        pixelPerBeet += deltax;
        if (pixelPerBeet < 10)
            pixelPerBeet = 10;
        double pixcelPerSecond = pixelPerBeet * bpm / 60.0;

        if (m_linkedTimeline) m_linkedTimeline->SetBarWidth(pixelPerBeet * timeSignatureNumerator);
        if (m_linkedEnvelopeline) m_linkedEnvelopeline->SetpixelPerSecond(pixcelPerSecond);

        for (auto& note : notes) {
            note.rect.m_width = note.note.length * pixcelPerSecond / 1000.0;
        }

        // 隙間なく並べる
        currentX = 0;
        for (size_t i = 0; i < notes.size(); i++) {
            notes[i].rect.m_x = currentX;
            currentX += notes[i].rect.m_width;
        }

        PitchControlPointUpdate();
        midiScript.CalculateNoteParam(pixcelPerSecond);
        pitchLine = midiScript.GetPitchLine();
        m_linkedEnvelopeline->SetEnvelopeline(midiScript.GetEnvelopeLine());
        Refresh();
    }
    if (deltay) {
        pixelPerNote += (double)deltay / 10.0;
        if (pixelPerNote < 10)
            pixelPerNote = 10;

        if (m_linkedKeys) m_linkedKeys->SetNoteHeight(pixelPerNote);
        A4KeyY = NESORA_MIDI_PANEL_A4_KEY_Y * pixelPerNote;

        double pixcelPerSecond = pixelPerBeet * bpm / 60.0;
        for (auto& note : notes) {
            note.rect.m_y = PitchToPixel(note.note.pitch, pixelPerNote, A4KeyY) + pixelPerNote / 2.0;
            note.rect.m_height = pixelPerNote;
        }
        PitchControlPointUpdate();
        midiScript.CalculateNoteParam(pixcelPerSecond);
        pitchLine = midiScript.GetPitchLine();
        m_linkedEnvelopeline->SetEnvelopeline(midiScript.GetEnvelopeLine());
        Refresh();
    }

    // screenWidth = pixelPerBeet * timeSignatureNumerator * 4.0;
    screenHeight = NESORA_MIDI_PANEL_KEY_COUNT * pixelPerNote;
    // 現在の表示（スクロール）位置をピクセル単位で取得
    int viewStartXUnits, viewStartYUnits;
    GetViewStart(&viewStartXUnits, &viewStartYUnits);
    int viewStartXPx = viewStartXUnits * ppux;
    int viewStartYPx = viewStartYUnits * ppuy;

    // 新旧スケール比から、マウス位置（center）が同じ画面位置に留まるように新しいスクロール位置を計算する
    double scaleX = pixelPerBeet / oldppb;
    double scaleY = pixelPerNote / oldppn;

    double newViewXPx = center.m_x * (scaleX - 1.0) + (double)viewStartXPx;
    double newViewYPx = center.m_y * (scaleY - 1.0) + (double)viewStartYPx;

    int x, y, w, h;
    GetClientSize(&w, &h);
    scriptLengthInBar = std::max(PixelToBar(w, pixelPerBeet, timeSignatureNumerator), PixelToBar(currentX, pixelPerBeet, timeSignatureNumerator) + 2.0); // 2小節分の余裕を持たせる
    screenWidth = scriptLengthInBar * pixelPerBeet * timeSignatureNumerator;

    // スクロール位置はスクロール単位に変換して渡す
    int newViewXUnits = std::max(0, (int)std::lround(newViewXPx / (double)ppux));
    int newViewYUnits = std::max(0, (int)std::lround(newViewYPx / (double)ppuy));

    SetScrollbars(ppux, ppuy, screenWidth / ppux, screenHeight / ppuy, newViewXUnits, newViewYUnits);
    if (m_linkedTimeline) {
        m_linkedTimeline->SetScriptLengthInBar(scriptLengthInBar);
        m_linkedTimeline->SetScrollOffset(newViewXPx);
    }
    if (m_linkedEnvelopeline) {
        m_linkedEnvelopeline->SetScriptLengthInBar(scriptLengthInBar);
        m_linkedEnvelopeline->SetBarWidth(pixelPerBeet * timeSignatureNumerator);
        m_linkedEnvelopeline->SetScrollOffset(newViewXPx);
    }
    if (m_linkedKeys) {
        m_linkedKeys->SetNoteHeight(pixelPerNote);
        m_linkedKeys->SetScrollOffset(newViewYUnits * ppuy);
    }

    quantizeWidth = pixelPerBeet / std::pow(2.0, std::floor(std::sqrt(pixelPerBeet / NESORA_MIDI_PANEL_QUANTIME_MIN_WIDTH)));
}

// MARK: OnPaint
void NesoraPianoRollCanvas::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    DoPrepareDC(dc); // スクロール位置を計算に含める
    dc.Clear();

    wxSize size = GetClientSize();
    wxRect2DDouble visibleRect(GetViewStart().x * ppux, GetViewStart().y * ppuy, size.GetWidth(), size.GetHeight());
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        // グリッド（背景）の描画
        wxFont font = GetFont();
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        for (int i = 0; i < NESORA_MIDI_PANEL_KEY_COUNT; ++i) {
            double y = i * pixelPerNote;
            wxPoint2DDouble linePoints[] = {{ visibleRect.m_x, y }, { visibleRect.m_x + visibleRect.m_width, y }};
            gc->StrokeLines(2, linePoints); // 水平線
        }
        for (int x = 0; x < scriptLengthInBar * pixelPerBeet * timeSignatureNumerator; x += pixelPerBeet) {
            double drawX = x;
            if (drawX > scriptLengthInBar * pixelPerBeet * timeSignatureNumerator) break;
            wxPoint2DDouble linePoints[] = {{ drawX, visibleRect.m_y }, { drawX, visibleRect.m_y + visibleRect.m_height }};
            gc->StrokeLines(2, linePoints); // 垂直線
        }

        // ノートの描画
        for (size_t i = 0;i < notes.size();i++) {
            if (notes[i].rect.Intersects(visibleRect)) {
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

                // 歌詞の描画
                if (notes[i].isSelected) {
                    gc->SetFont(font, nsGetColor(nsColorType::ON_SECONDARY));
                } else {
                    gc->SetFont(font, nsGetColor(nsColorType::ON_PRIMARY));
                }
                std::string lyric = notes[i].note.lyric.empty() ? "<br>" : notes[i].note.lyric; // 空の歌詞は<br>として表示
                gc->DrawText(lyric, notes[i].rect.m_x + 4, notes[i].rect.m_y + 4);
            }
        }

        // 範囲選択枠の描画
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::SelectingRange) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::SECONDARY_TRANSPARENT)));
            gc->SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));
            gc->DrawRectangle(currentSelectionRect.m_x, currentSelectionRect.m_y, currentSelectionRect.m_width, currentSelectionRect.m_height);
        }

        // ピッチラインの描画
        if (pitchLine.size() >= 2) {
            gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
            std::vector<wxPoint2DDouble> linePoints;
            for (size_t i = visibleRect.m_x; i < visibleRect.m_x + visibleRect.m_width && i < pitchLine.size(); i++) {
                linePoints.push_back(wxPoint2DDouble(i, PitchToPixel(pitchLine[i], pixelPerNote, A4KeyY) + pixelPerNote)); // ピッチをY座標に変換
            }
            gc->StrokeLines(linePoints.size(), linePoints.data());
        }

        // 現在の再生位置ラインを描画
        const double pixelPerSecond = pixelPerBeet * bpm / 60.0;
        const double playbackTimeForDraw = std::max(0.0, playbackTimeInSec.load() - playbackVisualDelayInSec);
        const double playheadX = playbackTimeForDraw * pixelPerSecond / 1000.0;
        if (playheadX >= visibleRect.m_x && playheadX <= visibleRect.m_x + visibleRect.m_width) {
            gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
            gc->StrokeLine(playheadX, visibleRect.m_y, playheadX, visibleRect.m_y + visibleRect.m_height);
        }

        // ピッチのコントロールポイントを描画
        if (selectedNoteIdx != -1 && selectedNoteIdx < (int)notes.size()) {
            gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2));
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
            for (const auto& controlPoint : notes[selectedNoteIdx].controlPoints) {
                gc->DrawEllipse(controlPoint.second.m_x, controlPoint.second.m_y, controlPoint.second.m_width, controlPoint.second.m_height);
            }
        }

        // 操作点のパラメータ
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        gc->SetPen(wxPen(nsGetColor(nsColorType::BACKGROUND_SHADOW)));
        gc->SetFont(font, nsGetColor(nsColorType::ON_BACKGROUND));
        if(selectedNoteIdx != -1 and draggingControlPointIdx != MidiNoteBoxControlPointID::None){
            wxString outputString = "Error";
            switch (draggingControlPointIdx) {
            case MidiNoteBoxControlPointID::FrontPitchMoveTimmingControlPoint: {
                outputString = wxString::Format(_("Pitch move start(%0.2fms)"), notes[selectedNoteIdx].note.frontPitchMoveTimming);
            }
            break;
            case MidiNoteBoxControlPointID::OvershootPitchControlPoint: {
                outputString = wxString::Format(_("Overshoot(%0.2fms, %0.2fcent)"), notes[selectedNoteIdx].note.frontPitchMoveTime, notes[selectedNoteIdx].note.overshootPitch);
            }
            break;
            case MidiNoteBoxControlPointID::ModulationControlPoint: {
                outputString = wxString::Format(_("Modulation start(%0.2fms)"), notes[selectedNoteIdx].note.modulationStartTime);
            }
            break;
            case MidiNoteBoxControlPointID::ModulationFrequencyControlPoint: {
                outputString = wxString::Format(_("Modulation frequency(%0.2fHz, %0.2fcent)"), notes[selectedNoteIdx].note.modulationFrequency, notes[selectedNoteIdx].note.modulationStrength);
            }
            break;
            case MidiNoteBoxControlPointID::ModulationFadeInTimeControlPoint: {
                outputString = wxString::Format(_("Modulation fade in(%0.2fms)"), notes[selectedNoteIdx].note.modulationFadeInTime);
            }
            break;
            case MidiNoteBoxControlPointID::ModulationFadeOutTimeControlPoint: {
                outputString = wxString::Format(_("Modulation fade out(%0.2fms)"), notes[selectedNoteIdx].note.modulationFadeOutTime);
            }
            break;
            case MidiNoteBoxControlPointID::PreparationPitchControlPoint: {
                outputString = wxString::Format(_("Preparation(%0.2fms, %0.2fcent)"), notes[selectedNoteIdx].note.preparationTime, notes[selectedNoteIdx].note.preparationPitch);
            }
            default:
            break;
            }
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width + tw < size.GetWidth() ? notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width : notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x - tw;
            int y = notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height + th < size.GetHeight() ? notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y + notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height : notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y - th;
            gc->DrawRectangle(x, y, tw, th);
            gc->DrawText(outputString, x, y);
        }
        delete gc;
    }
}

// MARK: マウスイベント

void NesoraPianoRollCanvas::OnMouseMove(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        event.Skip();
        return;
    }

    int oldHoverNoteIdx = hoverNoteIdx;
    wxPoint2DDouble mousePos = GetMousePos(event);
    if (IsPointMoved(startMousePos, mousePos)) {
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
            newNote.rect = wxRect2DDouble(prevEnd, std::floor(startMousePos.m_y / pixelPerNote) * pixelPerNote, 0, pixelPerNote);
            newNote.startRectBuffer = newNote.rect;
            newNote.isSelected = true;
            notes.push_back(CreateNewMidiNoteBox(newNote));
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
                    note.rect.m_y = std::floor(note.rect.m_y / pixelPerNote) * pixelPerNote;
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
                    note.rect.m_width = note.startRectBuffer.m_x + std::floor((mousePos.m_x - startMousePos.m_x) / quantizeWidth) * quantizeWidth - note.startRectBuffer.m_x + note.startRectBuffer.m_width;
                    if (note.rect.m_width < 0) {
                        note.rect.m_width *= -1;
                        note.rect.m_x = note.startRectBuffer.m_x - note.rect.m_width;
                    }
                }
            }
            tookAction = true;
        }
        break;
        // コントロールポイントを編集していたら
        case NesoraPianoRollCanvasMouseDragState::ControlPointDragging: {
            if (selectedNoteIdx != -1 and draggingControlPointIdx != MidiNoteBoxControlPointID::None) {
                notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_x = mousePos.m_x - notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_width / 2.0;
                notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_y = mousePos.m_y - notes[selectedNoteIdx].controlPoints[draggingControlPointIdx].m_height / 2.0;
            }
            PitchLineUpdate();
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
        wxPoint2DDouble mousePos = GetMousePos(event);
        bool onControlPoint = false;
        draggingControlPointIdx = MidiNoteBoxControlPointID::None;
        if (selectedNoteIdx != -1) {
            for (const auto& controlPoint : notes[selectedNoteIdx].controlPoints) {
                if (controlPoint.second.Contains(mousePos)) {
                    draggingControlPointIdx = controlPoint.first;
                    onControlPoint = true;
                    SetCursor(wxNullCursor); // 通常カーソル
                    break;
                }
            }
        }
        hoverNoteIdx = -1;
        if (!onControlPoint) {
            bool onHandle = false;
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
    }

    if ((event.Dragging() && HasCapture()) || oldHoverNoteIdx != hoverNoteIdx) {
        // if (isNotePreview) いつか作る
        //     ResolveOverlaps();
        Refresh(false);
    }

    event.Skip();
}

// MARK: 左クリック押し込み
void NesoraPianoRollCanvas::OnLeftDown(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    wxPoint2DDouble mousePos = GetMousePos(event);
    bool hitAnyNote = false;
    startMousePos = mousePos;

    if(hoverNoteIdx == -1) {
        if (draggingControlPointIdx == MidiNoteBoxControlPointID::None) {
            // 範囲選択開始
            if (tookAction) {
                NoteSelectClear();
                tookAction = false;
            }
            mouseDragState = NesoraPianoRollCanvasMouseDragState::SelectingRange;
            currentSelectionRect = wxRect2DDouble(mousePos.m_x, mousePos.m_y, 0, 0);
        } else {
            mouseDragState = NesoraPianoRollCanvasMouseDragState::ControlPointDragging;
        }
    } else {
        if (notes[hoverNoteIdx].rect.Contains(mousePos)) {
            // Shiftが押されていなければ、他の選択を解除
            if (!event.ShiftDown() && !notes[hoverNoteIdx].isSelected) {
                NoteSelectClear();
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

// MARK: 左ダブルクリック
void NesoraPianoRollCanvas::OnLeftDClick(wxMouseEvent& event) {
    wxPoint2DDouble mousePos = GetMousePos(event);

    int hitNoteIdx = -1;
    for (size_t i = 0; i < notes.size(); i++) {
        if (notes[i].rect.Contains(mousePos)) {
            hitNoteIdx = (int)(i);
            break;
        }
    }

    if (hitNoteIdx != -1) {
        if (!notes[hitNoteIdx].isSelected) {
            NoteSelectClear();
            notes[hitNoteIdx].isSelected = true;
        }

        mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
        ignoreNextLeftUp = true;
        if (HasCapture()) {
            ReleaseMouse();
        }

        BeginLyricEdit(hitNoteIdx);
        Refresh(false);
    } else {
        EndLyricEdit(true);
    }

    event.Skip(false);
}

// MARK: 左クリック離す
void NesoraPianoRollCanvas::OnLeftUp(wxMouseEvent& event) {
    if (ignoreNextLeftUp) {
        ignoreNextLeftUp = false;
        event.Skip();
        return;
    }

    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    wxPoint2DDouble mousePos = GetMousePos(event);
    // ノートの長さがフレーズよりも短かったら削除
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [&](MidiNoteBox value) {
            return std::abs(value.rect.m_width) < quantizeWidth;
            }),
        notes.end()
    );
    if (IsPointMoved(startMousePos, mousePos)) {
        // ドラッグかリサイズをしていたらノートを整列させる
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes || mouseDragState == NesoraPianoRollCanvasMouseDragState::ResizingNote)
            ResolveOverlaps();
        else if (mouseDragState == NesoraPianoRollCanvasMouseDragState::ControlPointDragging) {
            PitchLineUpdate();
        }
    } else {
        if (mouseDragState == NesoraPianoRollCanvasMouseDragState::DraggingNotes) {
            // ホバーしている時にポインタを動かさずにマウスボタンを離したら一つだけ選択するようにする
            if (!event.ShiftDown()) {
                NoteSelectClear();
            }
            notes[hoverNoteIdx].isSelected = true;
        } else if (mouseDragState == NesoraPianoRollCanvasMouseDragState::SelectingRange) {
            // ホバーしていない時にポインタを動かさずにマウスボタンを離したら全ての選択を解除する
            if (!event.ShiftDown()) {
                NoteSelectClear();
            }
        }
    }
    mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    if (HasCapture()) {
        ReleaseMouse();
    }
    ChangeSelectNote();
    wxWindow::Refresh();
    event.Skip();
}

// MARK: 右クリック押し込み
void NesoraPianoRollCanvas::OnRightDown(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    wxPoint mousePos = event.GetPosition();
    // 論理座標（スクロール位置を考慮した座標）を取得
    CalcUnscrolledPosition(mousePos.x, mousePos.y, &mousePos.x, &mousePos.y);
    bool hitAnyNote = false;
    startMousePos = mousePos;

    // ノートを追加
    if(hoverNoteIdx == -1) {
        NoteSelectClear();
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

// MARK: 右クリック離す
void NesoraPianoRollCanvas::OnRightUp(wxMouseEvent& event) {
    if (editingNoteIdx != -1) {
        EndLyricEdit(true);
    }

    // ノートの長さがフレーズよりも短かったら削除
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [&](MidiNoteBox value) {
            return std::abs(value.rect.m_width) < quantizeWidth;
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
    ChangeSelectNote();
    wxWindow::Refresh();
    event.Skip();
}

// MARK: キーボードイベント
void NesoraPianoRollCanvas::OnKeyDown(wxKeyEvent& event) {
    if (editingNoteIdx != -1) {
        event.Skip();
        return;
    }

    if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        std::vector<int> selectedOrder = GetSelectedNoteOrder();
        if (!selectedOrder.empty()) {
            BeginLyricEdit(selectedOrder.front());
            Refresh(false);
        }
        return;
    }

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
                note.rect.m_y -= pixelPerNote;
            }
        }
        ResolveOverlaps();
        Refresh();
        return;
    }
    if (event.GetKeyCode() == WXK_DOWN) {
        // 選択されているノートを半音下げる
        for (auto& note : notes) {
            if (note.isSelected) {
                note.rect.m_y += pixelPerNote;
            }
        }
        ResolveOverlaps();
        Refresh();
        return;
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

void NesoraPianoRollCanvas::OnSize(wxSizeEvent& event) {
    SetScrollWidth();
    event.Skip();
}

// MARK: 歌詞の編集関係

// 歌詞の編集を確定
void NesoraPianoRollCanvas::AdvanceLyricEdit(bool keepEditing) {
    if (editingNoteIdx < 0) {
        return;
    }

    const int nextNoteIdx = keepEditing ? GetNextSelectedNoteIndex(editingNoteIdx) : -1;
    EndLyricEdit(true);

    if (nextNoteIdx != -1) {
        BeginLyricEdit(nextNoteIdx);
    }
}

// 歌詞の編集を確定
void NesoraPianoRollCanvas::OnLyricEditorEnter(wxCommandEvent& event) {
    AdvanceLyricEdit(true);
    event.Skip(false);
}

// 歌詞の編集を確定してエディタを閉じる
void NesoraPianoRollCanvas::OnLyricEditorKillFocus(wxFocusEvent& event) {
    EndLyricEdit(true);
    event.Skip();
}

// 歌詞の編集中に特定のキーが押されたときの処理
void NesoraPianoRollCanvas::OnLyricEditorCharHook(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_ESCAPE) {
        EndLyricEdit(false);
        SetFocus();
        return;
    }
    if (event.GetKeyCode() == WXK_TAB || event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        AdvanceLyricEdit(true);
        return;
    }
    event.Skip();
}

void NesoraPianoRollCanvas::OnScroll(wxScrollWinEvent& event) {
    event.Skip(); // デフォルトのスクロール処理を実行

    static int lastScrollX = 0, lastScrollY = 0;
    int x, y;
    GetViewStart(&x, &y);
    // GetScrollPixelsPerUnit(&ppux, &ppuy);

    // これしないとちょっとズレる
    int dx = x - lastScrollX;
    int dy = y - lastScrollY;

    if (m_linkedTimeline) m_linkedTimeline->SetScrollOffset((x + dx) * ppux);
    if (m_linkedEnvelopeline) m_linkedEnvelopeline->SetScrollOffset((x + dx) * ppux);
    if (m_linkedKeys) m_linkedKeys->SetScrollOffset((y + dy) * ppuy);
    lastScrollX = x;
    lastScrollY = y;
}

void NesoraPianoRollCanvas::OnZoom(wxZoomGestureEvent& event) {
    // なんかできない
    std::cout << event.GetZoomFactor() << std::endl;
    std::cout << "zoom" << std::endl;
    event.Skip();
}

void NesoraPianoRollCanvas::OnMouseWheel(wxMouseEvent& event) {
    if (event.ControlDown()) {
        int delta = event.GetWheelRotation();
        if (event.ShiftDown())
            DoZoom(0, delta, GetMousePos(event));
        else
            DoZoom(delta, 0, GetMousePos(event));
    } else {
        event.Skip();
    }
}

void NesoraPianoRollCanvas::OnMagnify(wxMouseEvent& event) {
    DoZoom(event.GetMagnification() * 100.0, event.GetMagnification() * 100.0, GetMousePos(event));
    event.Skip();
}

double NesoraPianoRollCanvas::GetPitch(double t) {
    playbackTimeInSec.store(t);
    return midiScript.GetPitch(t);
}

double NesoraPianoRollCanvas::GetEnvelope(double t) {
    playbackTimeInSec.store(t);
    return midiScript.GetEnvelope(t);
}

void NesoraPianoRollCanvas::ClearPlaybackLine() {
    playbackTimeInSec.store(0.0);
    Refresh(false);
}

bool NesoraPianoRollCanvas::IsLyricEditing() const {
    return editingNoteIdx != -1;
}


// MARK:NesoraPianoKeys

void NesoraPianoKeys::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    wxSize size = GetClientSize();
    for (int i = 0; i < 128; ++i) {
        double y = i * m_noteHeight - m_yOffset;
        
        // 画面外の描画をスキップ
        if (y + m_noteHeight < 0 || y > size.GetHeight()) continue;

        int noteInOctave = (127 - i) % 12;
        bool isBlackKey = (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || noteInOctave == 8 || noteInOctave == 10);

        if (isBlackKey) {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PIANOKEY_BLACK)));
        } else {
            gc->SetBrush(wxBrush(nsGetColor(nsColorType::PIANOKEY_WHITE)));
        }
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
        gc->DrawRectangle(0, y, size.GetWidth(), m_noteHeight);

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

    for (int x = 0; x < scriptLengthInBar * m_barWidth; x += m_barWidth) {
        double drawX = x - m_xOffset;
        if (drawX + m_barWidth < 0 || drawX > size.GetWidth()) continue;

        gc->StrokeLine(drawX, size.GetHeight() - 10, drawX, size.GetHeight());
        gc->DrawText(wxString::Format("%d", x / m_barWidth + 1), drawX + 2, 2);
    }
    delete gc;
}

// MARK: NesoraEnvelopeline

wxPoint2DDouble NesoraEnvelopeline::GetMousePos(const wxMouseEvent& event) const {
    wxPoint pos = event.GetPosition();
    return wxPoint2DDouble(pos.x, pos.y);
}

wxRect2DDouble NesoraEnvelopeline::GetStrengthControlPointRect() const {
    if (note) {
        const wxSize size = GetClientSize();
        const double x = note->rect.m_x - m_xOffset + 14.0;
        const double usableHeight = std::max(1.0, (double)size.GetHeight() - margin * 2.0);
        const double y = margin + (1.0 - std::clamp(note->note.strength, 0.0, 1.0)) * usableHeight;
        return wxRect2DDouble(x - pointSize / 2.0, y - pointSize / 2.0, pointSize, pointSize);
    }

    return wxRect2DDouble();
}

wxRect2DDouble NesoraEnvelopeline::GetClLengthControlPointRect() const {
    if (note) {
        const double x = note->rect.m_x - m_xOffset + std::clamp(note->note.cl_length, 0.0, note->note.length) * m_pixelPerSecond / 1000.0;
        const wxSize size = GetClientSize();
        const double y = std::max(12.0, (double)size.GetHeight() - 14.0);
        return wxRect2DDouble(x - pointSize / 2.0, y - pointSize / 2.0, pointSize, pointSize);
    }

    return wxRect2DDouble();
}

void NesoraEnvelopeline::ControlPointUpdate() {
    strengthRect = GetStrengthControlPointRect();
    clLengthRect = GetClLengthControlPointRect();
}

void NesoraEnvelopeline::ApplyControlPointDrag(const wxPoint2DDouble& mousePos) {
    if (draggingControlPoint == EnvelopeControlPointID::StrengthControlPoint) {
        const wxSize size = GetClientSize();
        const double usableHeight = std::max(1.0, static_cast<double>(size.GetHeight()) - margin * 2.0);
        const double normalized = 1.0 - ((mousePos.m_y - margin) / usableHeight);
        note->note.strength = std::clamp(normalized, 0.0, 1.0);
    } else if (draggingControlPoint == EnvelopeControlPointID::ClLengthControlPoint) {
        const double clLength = (mousePos.m_x + m_xOffset - note->rect.m_x) * 1000.0 / std::max(1.0, m_pixelPerSecond);
        note->note.cl_length = std::clamp(clLength, 0.0, note->note.length);
    }

    ControlPointUpdate();
    if (m_linkedPianoRoll) {
        m_linkedPianoRoll->EnvelopeLineUpdate();
    } else {
        Refresh(false);
    }

    return;
}

void NesoraEnvelopeline::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    wxSize size = GetClientSize();
    gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND_THIN)));
    gc->SetFont(GetFont(), nsGetColor(nsColorType::ON_BACKGROUND));

    // エンベロープラインの描画
    if (envelopeLine.size() >= 2) {
        gc->SetPen(wxPen(nsGetColor(nsColorType::ON_BACKGROUND), 2));
        std::vector<wxPoint2DDouble> linePoints;
        for (size_t i = 0; i < scriptLengthInBar * m_barWidth && i < envelopeLine.size(); i++) {
            linePoints.push_back(wxPoint2DDouble(i - m_xOffset, (size.GetHeight() - margin) - envelopeLine[i] * (size.GetHeight() - margin * 2.0))); // エンベロープをY座標に変換
        }
        if (linePoints.size() >= 2)
            gc->StrokeLines(linePoints.size(), linePoints.data());
    }
    if (note) {
        gc->SetPen(wxPen(nsGetColor(nsColorType::PRIMARY), 2));
        gc->SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        gc->DrawEllipse(strengthRect.m_x, strengthRect.m_y, strengthRect.m_width, strengthRect.m_height);
        gc->DrawEllipse(clLengthRect.m_x, clLengthRect.m_y, clLengthRect.m_width, clLengthRect.m_height);

        gc->SetFont(GetFont(), nsGetColor(nsColorType::ON_BACKGROUND));
        if (hoverControlPoint == EnvelopeControlPointID::StrengthControlPoint) {
            draggingControlPoint = EnvelopeControlPointID::StrengthControlPoint;

            wxString outputString = wxString::Format("strength(%0.2f)", note->note.strength);
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = strengthRect.m_x + strengthRect.m_width + tw < size.GetWidth() ? strengthRect.m_x + strengthRect.m_width : strengthRect.m_x - tw;
            int y = strengthRect.m_y + strengthRect.m_height + th < size.GetHeight() ? strengthRect.m_y + strengthRect.m_height : strengthRect.m_y - th;
            gc->DrawRectangle(x, y, tw, th);
            gc->DrawText(outputString, x, y);
        } else if (hoverControlPoint == EnvelopeControlPointID::ClLengthControlPoint) {
            draggingControlPoint = EnvelopeControlPointID::ClLengthControlPoint;

            wxString outputString = wxString::Format("cl_length(%0.2fms)", note->note.cl_length);
            double tw, th;
            gc->GetTextExtent(outputString, &tw, &th);
            int x = clLengthRect.m_x + clLengthRect.m_width + tw < size.GetWidth() ? clLengthRect.m_x + clLengthRect.m_width : clLengthRect.m_x - tw;
            int y = clLengthRect.m_y + clLengthRect.m_height + th < size.GetHeight() ? clLengthRect.m_y + clLengthRect.m_height : clLengthRect.m_y - th;
            gc->DrawRectangle(x, y, tw, th);
            gc->DrawText(outputString, x, y);
        }
    }
    delete gc;
}

void NesoraEnvelopeline::EnvelopeControlPointUpdate(MidiNoteBox* selectedNote) {
    note = selectedNote;
    draggingControlPoint = EnvelopeControlPointID::None;
    ControlPointUpdate();
    Refresh(false);
}

void NesoraEnvelopeline::OnLeftDown(wxMouseEvent& event) {
    if (note) {
        const wxPoint2DDouble mousePos = GetMousePos(event);
        if (strengthRect.Contains(mousePos)) {
            draggingControlPoint = EnvelopeControlPointID::StrengthControlPoint;
        } else if (clLengthRect.Contains(mousePos)) {
            draggingControlPoint = EnvelopeControlPointID::ClLengthControlPoint;
        } else {
            event.Skip();
            return;
        }

        CaptureMouse();
        ApplyControlPointDrag(mousePos);
        Refresh(false);
    }
    event.Skip();
}

void NesoraEnvelopeline::OnLeftUp(wxMouseEvent& event) {
    if (HasCapture()) {
        ReleaseMouse();
    }
    draggingControlPoint = EnvelopeControlPointID::None;
    event.Skip();
}

void NesoraEnvelopeline::OnMouseMove(wxMouseEvent& event) {
    const wxPoint2DDouble mousePos = GetMousePos(event);

    if (note) {
        if (event.Dragging() && HasCapture()) {
            ApplyControlPointDrag(mousePos);
            Refresh(false);
            event.Skip(false);
            return;
        }

        if (strengthRect.Contains(mousePos)) {
            hoverControlPoint = EnvelopeControlPointID::StrengthControlPoint;
        } else if (clLengthRect.Contains(mousePos)) {
            hoverControlPoint = EnvelopeControlPointID::ClLengthControlPoint;
        } else {
            hoverControlPoint = EnvelopeControlPointID::None;
            SetCursor(wxNullCursor);
        }

        Refresh(false);
    }
    event.Skip();
}

void NesoraEnvelopeline::OnLeaveWindow(wxMouseEvent& event) {
    if (!HasCapture()) {
        SetCursor(wxNullCursor);
    }
    event.Skip();
}



// MARK:NesoraMIDIPanel

void NesoraMIDIPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* MIDIEditorSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI Editor"));

    // ピアノロール（中央）を先に作成して、リンクさせる
    pianoRoll = new NesoraPianoRollCanvas(MIDIEditorSizer->GetStaticBox());
    
    // タイムライン（上部）
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->AddSpacer(60); // ピアノ鍵盤の幅分のスペースを確保
    NesoraTimeline* timeline = new NesoraTimeline(MIDIEditorSizer->GetStaticBox());
    timeline->SetMinSize(wxSize(-1, 30));
    topSizer->Add(timeline, 1, wxEXPAND);

    wxBoxSizer* middleSizer = new wxBoxSizer(wxHORIZONTAL);
    // ピアノ鍵盤（左側）
    NesoraPianoKeys* keys = new NesoraPianoKeys(MIDIEditorSizer->GetStaticBox());
    keys->SetMinSize(wxSize(60, -1));
    middleSizer->Add(keys, 0, wxEXPAND);
    middleSizer->Add(pianoRoll, 1, wxEXPAND);

    // エンベロープライン（下部）
    wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);
    bottomSizer->AddSpacer(60); // ピアノ鍵盤の幅分のスペースを確保
    NesoraEnvelopeline* envelopeline = new NesoraEnvelopeline(MIDIEditorSizer->GetStaticBox());
    envelopeline->SetMinSize(wxSize(-1, 60));
    bottomSizer->Add(envelopeline, 1, wxEXPAND);

    MIDIEditorSizer->Add(topSizer, 0, wxEXPAND);
    MIDIEditorSizer->Add(middleSizer, 1, wxEXPAND);
    MIDIEditorSizer->Add(bottomSizer, 0, wxEXPAND);
    
    
    mainSizer->Add(MIDIEditorSizer, 1, wxEXPAND | wxALL, 5);
    // mainSizer->Add(MIDINoteEditor, 0, wxEXPAND | wxALL, 5);
    
    this->SetSizer(mainSizer);
    
    // 同期設定
    pianoRoll->SetLinkedPianoKeys(keys);
    pianoRoll->SetLinkedTimeline(timeline);
    pianoRoll->SetLinkedEnvelopeline(envelopeline);
    envelopeline->SetLinkedPianoRoll(pianoRoll);

    playbackLineTimer.SetOwner(this);
    Bind(wxEVT_TIMER, &NesoraMIDIPanel::OnPlaybackTimer, this, playbackLineTimer.GetId());
    playbackLineTimer.Start(16);
}

double NesoraMIDIPanel::GetPitch() {
    return pianoRoll->GetPitch(nowPlayTime);
}

double NesoraMIDIPanel::GetEnvelope() {
    return pianoRoll->GetEnvelope(nowPlayTime);
}

void NesoraMIDIPanel::ProceedTime(double samplingFrequency) {
    nowPlayTime += 1000.0 / samplingFrequency;
}

bool NesoraMIDIPanel::IsLyricEditing() const {
    return pianoRoll != nullptr and pianoRoll->IsLyricEditing();
}

void NesoraMIDIPanel::PlayStop() {
    nowPlayTime = 0.0;
    pianoRoll->ClearPlaybackLine();
    return;
}

void NesoraMIDIPanel::OnPlaybackTimer(wxTimerEvent& event) {
    if (pianoRoll) {
        pianoRoll->Refresh(false);
    }
    event.Skip();
}


// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_MIDI_PANEL_H
#define NESORA_MIDI_PANEL_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <vector>
#include <stack>
#include <memory>

#include "../../NesoraStyle/NesoraStyle.h"

#include "../NesoraIDs.h"
#include "../NesoraEvents.h"

#include "../../Nesora/Nesora.h"

// ノート（音符）を管理する構造体
struct MidiNoteBox {
    int id;
    // int pitch;    // 0-127 (MIDIノート番号)
    // int position; // 時間（X座標）
    // int duration; // 長さ
    wxRect2DDouble rect;  // 描画領域（当たり判定用）
    wxRect2DDouble startRectBuffer;  // 描画領域（バッファー用）
    bool isSelected = false; // 選択状態フラグ
};





// 操作状態を定義
enum class NesoraPianoRollCanvasMouseDragState {
    None,
    AddNote,
    SelectingRange, // 範囲選択中
    DraggingNotes,  // ノート移動中
    ResizingNote    // ノートの長さ変更中
};

const int NESORA_MIDI_PANEL_NOTE_HEIGHT = 20; // 1鍵あたりの高さ
const double NESORA_MIDI_PANEL_QUANTIME_WIDTH = 16.0;
const double NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH = 5.0; // 右端の判定幅(px)

class NesoraPianoRollCanvas : public wxScrolledWindow {
public:
    NesoraPianoRollCanvas(wxWindow* parent) : wxScrolledWindow(parent, wxID_ANY) {
        Init();
    }

    void Init();

private:
    std::vector<MidiNoteBox> notes;
    int  hoverNoteIdx = -1;

    bool isAddNote = false;

    NesoraPianoRollCanvasMouseDragState mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    
    wxRect2DDouble currentSelectionRect;    // 矩形選択の矩形
    wxPoint2DDouble startMousePos;          // ドラッグ開始地点

    wxPoint2DDouble GetMousePos(const wxMouseEvent& event);
    wxRect2DDouble GetResizeHandleRect(const MidiNoteBox& note);
    void ResolveOverlaps(int changedNote);

    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);

    void OnKeyDown(wxKeyEvent& event);
};

class nsMIDIControl : public wxWindow {
public:
    nsMIDIControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxWindow(parent, winid, pos, size, style, name)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
        Bind(wxEVT_PAINT, &nsMIDIControl::OnPaint, this);
    }

private:

    void OnPaint(wxPaintEvent& event){}
    int midiValue = 0;
};

class NesoraMIDIPanel : public wxPanel {
public:
    NesoraMIDIPanel() {
        Init();
    }
    NesoraMIDIPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name)
    {
        Init();
    }
    
    void Init();

private:
    wxSlider* midi_slider;
    wxStaticText* midi_param;

    NesoraPianoRollCanvas* pianoRoll;
};

#endif // NESORA_MIDI_PANEL_H

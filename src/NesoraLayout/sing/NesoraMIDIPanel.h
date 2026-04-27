// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_MIDI_PANEL_H
#define NESORA_MIDI_PANEL_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/clipbrd.h>
#include <vector>
#include <stack>
#include <memory>
#include <atomic>
// #include <sstream>
#include <string>
# include <functional>

#include "../../NesoraStyle/NesoraStyle.h"

#include "../NesoraIDs.h"
#include "../NesoraEvents.h"

#include "../../Nesora/Nesora.h"

enum class MidiNoteBoxControlPointID {
    None,
    FrontPitchMoveTimmingControlPoint,
    OvershootPitchControlPoint,
    PreparationPitchControlPoint,
    ModulationControlPoint,
    ModulationFrequencyControlPoint,
    ModulationFadeInTimeControlPoint,
    ModulationFadeOutTimeControlPoint,
};

// ノート（音符）を管理する構造体
struct MidiNoteBox {
    int id;
    
    NesoraMidiNotePhoneticalInfo note; // MIDIノートの情報（ピッチ、開始時間、長さなど）
    std::map<MidiNoteBoxControlPointID, wxRect2DDouble> controlPoints; // コントロールポイントの位置とサイズ

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
    ResizingNote,    // ノートの長さ変更中
    ControlPointDragging // 操作点をドラッグ中
};

const int NESORA_MIDI_PANEL_NOTE_HEIGHT = 20; // 1鍵あたりの高さ
const int NESORA_MIDI_PANEL_KEY_COUNT = 128; // 鍵の数
const double NESORA_MIDI_PANEL_A4_KEY_Y = 1160.0 - NESORA_MIDI_PANEL_NOTE_HEIGHT / 2.0; // A4の鍵のY座標
const double NESORA_MIDI_PANEL_QUANTIME_WIDTH = 32.0;
const double NESORA_MIDI_PANEL_RESIZE_HANDLE_WIDTH = 8.0; // 右端の判定幅(px)

class NesoraPhoneticalMIDINoteEditor : public wxWindow {
public:
    NesoraPhoneticalMIDINoteEditor(wxWindow* parent) : wxWindow(parent, wxID_ANY) {
        Init();
    }
    void EditNote(MidiNoteBox* note);
    void BindNoteUpdateEvent(std::function<void()>handler);
    void SetParentSizer(wxSizer* sizer) { parentSizer = sizer; }
private:
    void Init();
    void OnPaint(wxPaintEvent& event);
    MidiNoteBox* note;
    std::function<void()> m_noteUpdateHandler;

    wxSizer* parentSizer = nullptr;

    wxTextCtrl* lyricEditor = nullptr;
    
    wxStaticText* frontOverlap_param;
    wxStaticText* backOverlap_param;
    wxStaticText* strength_param;
    wxStaticText* frontTransitionTime_param;
    wxStaticText* backTransitionTime_param;
    wxStaticText* cl_length_param;
    wxStaticText* uv_length_param;
    wxStaticText* sv_length_param;
    wxStaticText* frontPitchMoveTime_param;
    wxStaticText* frontPitchMoveTimming_param;
    wxStaticText* modulationStrength_param;
    wxStaticText* modulationFrequency_param;
    wxStaticText* modulationStartTime_param;
    wxStaticText* modulationFadeInTime_param;
    wxStaticText* modulationFadeOutTime_param;
    wxStaticText* overshootTime_param;
    wxStaticText* overshootPitch_param;
    wxStaticText* preparationTime_param;
    wxStaticText* preparationPitch_param;

    nsSlider* frontOverlap_slider;
    nsSlider* backOverlap_slider;
    nsSlider* strength_slider;
    nsSlider* frontTransitionTime_slider;
    nsSlider* backTransitionTime_slider;
    nsSlider* cl_length_slider;
    nsSlider* uv_length_slider;
    nsSlider* sv_length_slider;
    nsSlider* frontPitchMoveTime_slider;
    nsSlider* frontPitchMoveTimming_slider;
    nsSlider* modulationStrength_slider;
    nsSlider* modulationFrequency_slider;
    nsSlider* modulationStartTime_slider;
    nsSlider* modulationFadeInTime_slider;
    nsSlider* modulationFadeOutTime_slider;
    nsSlider* overshootTime_slider;
    nsSlider* overshootPitch_slider;
    nsSlider* preparationTime_slider;
    nsSlider* preparationPitch_slider;

    void OnFrontOverlapSlide(wxCommandEvent& event);
    void OnBackOverlapSlide(wxCommandEvent& event);
    void OnStrengthSlide(wxCommandEvent& event);
    void OnFrontTransitionTimeSlide(wxCommandEvent& event);
    void OnBackTransitionTimeSlide(wxCommandEvent& event);
    void OnCLLengthSlide(wxCommandEvent& event);
    void OnUVLengthSlide(wxCommandEvent& event);
    void OnSVLengthSlide(wxCommandEvent& event);
    void OnFrontPitchMoveTimeSlide(wxCommandEvent& event);
    void OnFrontPitchMoveTimmingSlide(wxCommandEvent& event);
    void OnModulationStrengthSlide(wxCommandEvent& event);
    void OnModulationFrequencySlide(wxCommandEvent& event);
    void OnModulationStartTimeSlide(wxCommandEvent& event);
    void OnModulationFadeInTimeSlide(wxCommandEvent& event);
    void OnModulationFadeOutTimeSlide(wxCommandEvent& event);
    void OnOvershootTimeSlide(wxCommandEvent& event);
    void OnOvershootPitchSlide(wxCommandEvent& event);
    void OnPreparationTimeSlide(wxCommandEvent& event);
    void OnPreparationPitchSlide(wxCommandEvent& event);
    
    constexpr static double frontOverlapMax = 100.0;
    constexpr static double backOverlapMax = 100.0;
    constexpr static double strengthMax = 100.0;
    constexpr static double transitionTimeMax = 2000.0;
    constexpr static double clLengthMax = 1.0;
    constexpr static double uvLengthMax = 1.0;
    constexpr static double svLengthMax = 1.0;
    constexpr static double frontPitchMoveTimeMax = 1000.0;
    constexpr static double frontPitchMoveTimmingMax = 1000.0;
    constexpr static double modulationStrengthMax = 100.0;
    constexpr static double modulationFrequencyMax = 20.0;
    constexpr static double modulationStartTimeMax = 2000.0;
    constexpr static double modulationFadeTimeMax = 2000.0;
    constexpr static double overshootTimeMax = 1000.0;
    constexpr static double overshootPitchMax = 300.0;
    constexpr static double preparationTimeMax = 2000.0;
    constexpr static double preparationPitchMax = 300.0;
};

class NesoraPianoKeys : public wxWindow {
public:
    NesoraPianoKeys(wxWindow* parent) : wxWindow(parent, wxID_ANY) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &NesoraPianoKeys::OnPaint, this);
    }
    void SetScrollOffset(int yOffset) { m_yOffset = yOffset; Refresh(); }
private:
    void OnPaint(wxPaintEvent& event);
    int m_yOffset = 0;
};

class NesoraTimeline : public wxWindow {
public:
    NesoraTimeline(wxWindow* parent) : wxWindow(parent, wxID_ANY) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &NesoraTimeline::OnPaint, this);
    }
    void SetScrollOffset(int xOffset) { m_xOffset = xOffset; Refresh(); }
    void SetBarWidth(int barWidth) { m_barWidth = barWidth; Refresh(); }
    void SetScriptLengthInBar(double lengthInBar) { scriptLengthInBar = lengthInBar; Refresh(); }
private:
    void OnPaint(wxPaintEvent& event);
    int m_xOffset = 0;
    int m_barWidth = 256; // 1小節の幅（px）
    double scriptLengthInBar = 0.0; // スクリプトの長さ（小節数）
};

class NesoraPianoRollCanvas : public wxScrolledWindow {
public:
    NesoraPianoRollCanvas(wxWindow* parent) : wxScrolledWindow(parent, wxID_ANY) {
        Init();
    }

    void Init();
    double GetPitch(double t);
    void ClearPlaybackLine();
    bool IsLyricEditing() const;

    void SetLinkedPianoKeys(NesoraPianoKeys* keys) {
        m_linkedKeys = keys;
        int x, y;
        GetViewStart(&x, &y);
        int ppux, ppuy;
        GetScrollPixelsPerUnit(&ppux, &ppuy);
        m_linkedKeys->SetScrollOffset(y * ppuy);
    }
    void SetLinkedTimeline(NesoraTimeline* timeline) { m_linkedTimeline = timeline; }
    void SetLinkedMIDINoteEditor(NesoraPhoneticalMIDINoteEditor* editor);
private:
    
    NesoraPianoKeys* m_linkedKeys = nullptr;
    NesoraTimeline* m_linkedTimeline = nullptr;
    NesoraPhoneticalMIDINoteEditor* m_linkedMIDINoteEditor = nullptr;
        
    std::vector<MidiNoteBox> notes;
    NesoraMIDIPhoneticalScript midiScript;
    std::vector<double> pitchLine;
    int  hoverNoteIdx = -1;

    int ppux = 8;
    int ppuy = 8;
    int screenWidth = 0;
    int screenHeight = 0;
    double currentX = 0.0;

    wxTextCtrl* lyricEditor = nullptr;
    int editingNoteIdx = -1;
    std::string editingOriginalLyric = "";
    bool ignoreNextLeftUp = false;

    double pixelPerBeet = 64.0;             // 1拍あたりのピクセル数
    double bpm = 120.0;                     // 1分あたりの拍数
    double scriptLengthInBar = 9.0;         // スクリプトの長さ（小節数）
    double timeSignatureNumerator = 4.0;    // 拍子の分子
    double timeSignatureDenominator = 4.0;  // 拍子の分母

    bool isAddNote = false;         // ノート追加モードかどうか
    bool tookAction = false;        // ドラッグ操作中に実際にノートの追加や移動などのアクションが発生したか
    bool isNotePreview = true;      // プレビュー表示するかどうか(仮)
    std::atomic<double> playbackTimeInSec{0.0}; // 再生位置ライン用の現在時刻(秒)
    double playbackVisualDelayInSec = 0.05;     // 実際の出音遅延を見越した表示補正(秒)

    int selectedNoteIdx = -1;                   // 現在選択されているノートのインデックス
    MidiNoteBoxControlPointID draggingControlPointIdx;      //ドラッグ中の操作点のインデックス

    NesoraPianoRollCanvasMouseDragState mouseDragState = NesoraPianoRollCanvasMouseDragState::None;
    
    wxRect2DDouble currentSelectionRect;    // 矩形選択の矩形
    wxPoint2DDouble startMousePos;          // ドラッグ開始地点

    wxPoint2DDouble GetMousePos(const wxMouseEvent& event);
    wxRect2DDouble GetRightResizeHandleRect(const MidiNoteBox& note);
    wxRect2DDouble GetLeftResizeHandleRect(const MidiNoteBox& note);
    std::vector<int> GetSelectedNoteOrder() const;
    int GetNextSelectedNoteIndex(int currentNoteIdx) const;
    void ResolveOverlaps();
    void PitchControlPointUpdate();
    void PitchLineUpdate();

    void NoteSelectClear();
    void ChangeSelectNote();
    MidiNoteBox CreateNewMidiNoteBox(MidiNoteBox box);

    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftDClick(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnRightUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);

    void BeginLyricEdit(int noteIdx);
    void EndLyricEdit(bool commit);
    void AdvanceLyricEdit(bool keepEditing = true);
    void OnLyricEditorEnter(wxCommandEvent& event);
    void OnLyricEditorKillFocus(wxFocusEvent& event);
    void OnLyricEditorCharHook(wxKeyEvent& event);

    void SetScrollWidth();

    void OnKeyDown(wxKeyEvent& event);
    void OnScroll(wxScrollWinEvent& event);
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
    double GetPitch(double samplingFrequency);
    void PlayStop();
    bool IsLyricEditing() const;

private:
    wxSlider* midi_slider;
    wxStaticText* midi_param;

    NesoraPianoRollCanvas* pianoRoll;
    wxTimer playbackLineTimer;

    double nowPlayTime = 0.0;

    void OnPlaybackTimer(wxTimerEvent& event);
};

#endif // NESORA_MIDI_PANEL_H

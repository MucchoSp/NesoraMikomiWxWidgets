// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_PHONETIC_FILTER_PANEL_H
#define NESORA_PHONETIC_FILTER_PANEL_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>

#include <sstream>

#include "../../../../NesoraStyle/NesoraStyle.h"

#include "../NesoraVoiceMakePanelBase.h"
#include "../../../NesoraIDs.h"
#include "../../../NesoraEvents.h"

#include "../../../../Nesora/Nesora.h"
#include "../../../../Nesora/filter/NesoraParametricSOSIIRFilter/NesoraParametricSOSIIRFilter.h"





// MARK:nsSOSIIRFrequencyResponseControl

// class nsParametricSOSIIRFrequencyResponseControl : public wxWindow {
// public:
//     nsParametricSOSIIRFrequencyResponseControl(wxWindow* parent,
//         wxWindowID winid = wxID_ANY,
//         const wxPoint& pos = wxDefaultPosition,
//         const wxSize& size = wxDefaultSize,
//         long style = wxTAB_TRAVERSAL | wxNO_BORDER,
//         const wxString& name = wxASCII_STR(wxPanelNameStr)); 
//
//     NesoraParametricSOSIIRFilter* filter;
//     // UIを更新
//     void SyncControlPointsFromFilter();
//     void SetSelectedParameter(uint32_t param);
//     private:
//    
//     std::vector<double> frequencyResponse;
//     std::map<uint32_t, double> parameters;
//    
//     uint32_t nowSelectedParameter = 0;
//    
//     std::vector<wxRect2DDouble> controlPoints;              //操作点
//     std::vector<wxRect2DDouble> destinationControlPoints;   //いま選択されているパラメーターにより飛ばされる先の場所
//     std::vector<wxRect2DDouble> nowControlPoints;           //いま選択されているパラメーターにより飛ばされた場所
//     int selectedControlPointIndex = -1;
//     int selectedDestinationControlPointIndex = -1;
//    
//     bool paramater_updated = false;
//     bool shiftKeyDown = false;
//    
//     double samplingFrequency = NesoraDefaultSamplingFrequency;
//     double nyquistFrequency = NesoraDefaultNyquistFrequency;
//    
//     void RecalculationFrequencyResponse();
//     void SetControlPointsFromFilter();
//    
//     void OnChangeSelectedParameter(nsSelectedParameterChangeEvent& event);
//     void OnChangeParameter(nsParameterChangeEvent& event);
//     void OnAddParameter(nsAddParameterEvent& event);
//
//     void OnPaint(wxPaintEvent& event);
//
//     void OnMouseMove(wxMouseEvent& event);
//     void OnMouseLeave(wxMouseEvent& event);
//     void OnMouseDown(wxMouseEvent& event);
//     void OnMouseUp(wxMouseEvent& event);
//     void OnMouseWheel(wxMouseEvent& event);
//     void OnRightDown(wxMouseEvent& event);
//     void OnRightUp(wxMouseEvent& event);
//     void OnSize(wxSizeEvent& event);
//
// };


enum class NesoraPhoneticVowelType {
    open_front_unrounded_vowel = 0,// a
    open_central_unrounded_vowel,  // ä
    open_back_unrounded_vowel,     // A
    open_front_rounded_vowel,      // ɶ
    open_back_rounded_vowel,       // ɑ
    near_open_front_unrounded_vowel,  // æ
    near_open_back_unrounded_vowel,   // ɐ
    near_open_front_rounded_vowel,    // ɒ
    near_open_back_rounded_vowel,     // ɒ
    open_mid_front_unrounded_vowel,   // ɛ
    open_mid_back_unrounded_vowel,    // ʌ
    open_mid_front_rounded_vowel,     // œ
    open_mid_back_rounded_vowel,      // ɔ
    mid_front_unrounded_vowel,        // e
    mid_back_unrounded_vowel,         // ɤ
    mid_front_rounded_vowel,          // ø
    mid_back_rounded_vowel,           // o
    close_front_unrounded_vowel,      // i
    close_back_unrounded_vowel,       // ɯ
    close_front_rounded_vowel,        // y
    close_back_rounded_vowel,         // u
    None, 
};
const std::array<wxString, 22> nsPhoneticVowelNames = {
    wxString::FromUTF8(_("a")),
    wxString::FromUTF8(_("ä")),
    wxString::FromUTF8(_("A")),
    wxString::FromUTF8(_("ɶ")),
    wxString::FromUTF8(_("ɑ")),
    wxString::FromUTF8(_("æ")),
    wxString::FromUTF8(_("ɐ")),
    wxString::FromUTF8(_("ɒ")),
    wxString::FromUTF8(_("ɒ")),
    wxString::FromUTF8(_("ɛ")),
    wxString::FromUTF8(_("ʌ")),
    wxString::FromUTF8(_("œ")),
    wxString::FromUTF8(_("ɔ")),
    wxString::FromUTF8(_("e")),
    wxString::FromUTF8(_("ɤ")),
    wxString::FromUTF8(_("ø")),
    wxString::FromUTF8(_("o")),
    wxString::FromUTF8(_("i")),
    wxString::FromUTF8(_("ɯ")),
    wxString::FromUTF8(_("y")),
    wxString::FromUTF8(_("u")),
    wxString::FromUTF8(_("None"))
};

// MARK:nsPhoneticVowelControl

class nsPhoneticVowelControl : public wxWindow {
public:
    nsPhoneticVowelControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr));

    void SetFilter(NesoraParametricSOSIIRFilter* filter);

    void OnPaint(wxPaintEvent& event);

    void OnMouseMove(wxMouseEvent& event);
    // void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    // void OnRightDown(wxMouseEvent& event);
    // void OnRightUp(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    
private:
    NesoraParametricSOSIIRFilter* filter;

    int nowGraphX = 0;
    int nowGraphY = 0;
    double nowGraphScaleX = 0.5;   // 幅
    double nowGraphScaleY = 0.5;   // 高さ

    wxPoint lastMousePos = {0, 0};
    NesoraPhoneticVowelType hoveredVowelType = NesoraPhoneticVowelType::None;
    NesoraPhoneticVowelType draggingVowelType = NesoraPhoneticVowelType::None;

    wxPoint2DDouble nowVowelPoint = {0, 0};
    std::map<NesoraPhoneticVowelType, wxPoint2DDouble> vowelPoints = {
        {NesoraPhoneticVowelType::open_front_unrounded_vowel, wxPoint2DDouble(1500, 1000)},
        {NesoraPhoneticVowelType::open_central_unrounded_vowel, wxPoint2DDouble(1000, 1000)},
        {NesoraPhoneticVowelType::open_back_unrounded_vowel, wxPoint2DDouble(500, 1000)},
        {NesoraPhoneticVowelType::open_front_rounded_vowel, wxPoint2DDouble(1500, 500)},
        {NesoraPhoneticVowelType::open_back_rounded_vowel, wxPoint2DDouble(500, 500)},
        {NesoraPhoneticVowelType::near_open_front_unrounded_vowel, wxPoint2DDouble(1400, 900)},
        {NesoraPhoneticVowelType::near_open_back_unrounded_vowel, wxPoint2DDouble(600, 900)},
        {NesoraPhoneticVowelType::near_open_front_rounded_vowel, wxPoint2DDouble(1400, 400)},
        {NesoraPhoneticVowelType::near_open_back_rounded_vowel, wxPoint2DDouble(600, 400)},
        {NesoraPhoneticVowelType::open_mid_front_unrounded_vowel, wxPoint2DDouble(1200, 700)},
        {NesoraPhoneticVowelType::open_mid_back_unrounded_vowel, wxPoint2DDouble(800, 700)},
        {NesoraPhoneticVowelType::open_mid_front_rounded_vowel, wxPoint2DDouble(1200, 300)},
        {NesoraPhoneticVowelType::open_mid_back_rounded_vowel, wxPoint2DDouble(800, 300)},
        {NesoraPhoneticVowelType::mid_front_unrounded_vowel, wxPoint2DDouble(1000, 500)},
        {NesoraPhoneticVowelType::mid_back_unrounded_vowel, wxPoint2DDouble(700, 500)},
        {NesoraPhoneticVowelType::mid_front_rounded_vowel, wxPoint2DDouble(1000, 200)},
        {NesoraPhoneticVowelType::mid_back_rounded_vowel, wxPoint2DDouble(700, 200)},
        {NesoraPhoneticVowelType::close_front_unrounded_vowel, wxPoint2DDouble(500, 300)},
        {NesoraPhoneticVowelType::close_back_unrounded_vowel, wxPoint2DDouble(300, 300)},
        {NesoraPhoneticVowelType::close_front_rounded_vowel, wxPoint2DDouble(500, 100)},
        {NesoraPhoneticVowelType::close_back_rounded_vowel, wxPoint2DDouble(300, 100)}
    };
    std::map<NesoraPhoneticVowelType, bool> vowelPointEnabled = {
        {NesoraPhoneticVowelType::open_front_unrounded_vowel, false},
        {NesoraPhoneticVowelType::open_central_unrounded_vowel, true},
        {NesoraPhoneticVowelType::open_back_unrounded_vowel, false},
        {NesoraPhoneticVowelType::open_front_rounded_vowel, false},
        {NesoraPhoneticVowelType::open_back_rounded_vowel, false},
        {NesoraPhoneticVowelType::near_open_front_unrounded_vowel, false},
        {NesoraPhoneticVowelType::near_open_back_unrounded_vowel, false},
        {NesoraPhoneticVowelType::near_open_front_rounded_vowel, false},
        {NesoraPhoneticVowelType::near_open_back_rounded_vowel, false},
        {NesoraPhoneticVowelType::open_mid_front_unrounded_vowel, false},
        {NesoraPhoneticVowelType::open_mid_back_unrounded_vowel, false},
        {NesoraPhoneticVowelType::open_mid_front_rounded_vowel, false},
        {NesoraPhoneticVowelType::open_mid_back_rounded_vowel, false},
        {NesoraPhoneticVowelType::mid_front_unrounded_vowel, true},
        {NesoraPhoneticVowelType::mid_back_unrounded_vowel, false},
        {NesoraPhoneticVowelType::mid_front_rounded_vowel, false},
        {NesoraPhoneticVowelType::mid_back_rounded_vowel, true},
        {NesoraPhoneticVowelType::close_front_unrounded_vowel, true},
        {NesoraPhoneticVowelType::close_back_unrounded_vowel, false},
        {NesoraPhoneticVowelType::close_front_rounded_vowel, false},
        {NesoraPhoneticVowelType::close_back_rounded_vowel, true}
    };

    wxPoint2DDouble GraphToVowelPoint(const wxPoint2DDouble& graphPoint);
    wxPoint2DDouble VowelToGraphPoint(const wxPoint2DDouble& vowelPoint);
};



// MARK:nsPhoneticPersonalityControl

class nsPhoneticPersonalityControl : public wxWindow {
public:
    nsPhoneticPersonalityControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr));

    void SetFilter(NesoraParametricSOSIIRFilter* filter);

private:
    NesoraParametricSOSIIRFilter* filter;

    nsSlider* genderSlider;
    nsSlider* ageSlider;
};



// MARK:nsIIRFrequencyResponseControl

class nsIIRFrequencyResponseControl : public wxWindow {
public:
    nsIIRFrequencyResponseControl(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr));

    void SetFilter(NesoraParametricSOSIIRFilter* filter);

    void OnPaint(wxPaintEvent& event);
    
private:
    NesoraParametricSOSIIRFilter* filter;
};



// MARK:nsPhoneticFilterPanel

class nsPhoneticFilterPanel : public nsFilterPanelBase {
public:
    nsPhoneticFilterPanel() {
        Init();
    }
    nsPhoneticFilterPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : nsFilterPanelBase(parent, winid, pos, size, style, name)
    {
        Init();
    }
    ~nsPhoneticFilterPanel() {
        
    }
    void Init() override;
    void Update() override;

    NesoraFilterBase* GetFilter() override;

private:
    NesoraParametricSOSIIRFilter* filter;
    
    nsPhoneticVowelControl* vowelFilter;
    nsPhoneticPersonalityControl* personalityFilter;
    nsIIRFrequencyResponseControl* frequencyResponse;
};


#endif // NESORA_PHONETIC_FILTER_PANEL_H

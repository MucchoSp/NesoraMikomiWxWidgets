// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <wx/event.h>

#ifndef NESORA_EVENTS_H
#define NESORA_EVENTS_H

// 歌うタブにて、選択しているパラメーターが変更されたら発生するイベント
class nsSelectedParameterChangeEvent : public wxCommandEvent {
public:
    nsSelectedParameterChangeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id) { }

    // イベントデータをコピーするためのCloneメソッド（必須）
    virtual wxEvent* Clone() const override { return new nsSelectedParameterChangeEvent(*this); }

    // 独自のデータを保持する場合
    void SetID(const uint32_t& id) {
        m_ID = id;
    }
    
    uint32_t GetID() const {
        return m_ID;
    }

private:
    uint32_t m_ID;
};

// このマクロは一般的にヘッダファイル内で使用します: これは MY_EVENT イベント型を定義するだけです
wxDECLARE_EVENT(nsEVT_SELECTED_PARAMETER_CHANGED, nsSelectedParameterChangeEvent);


// 歌うタブにて、パラメーターが追加されたら発生するイベント
class nsAddParameterEvent : public wxCommandEvent {
public:
    nsAddParameterEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id) { }

    // イベントデータをコピーするためのCloneメソッド（必須）
    virtual wxEvent* Clone() const override { return new nsAddParameterEvent(*this); }

    // 独自のデータを保持する場合
    void SetData(const uint32_t& id) {
        m_ID = id;
    }
    
    uint32_t GetData() const {
        return m_ID;
    }

private:
    uint32_t m_ID;
};

// このマクロは一般的にヘッダファイル内で使用します: これは MY_EVENT イベント型を定義するだけです
wxDECLARE_EVENT(nsEVT_ADD_PARAMETER, nsAddParameterEvent);


// パラメーターが変更されたら発生するイベント
class nsParameterChangeEvent : public wxCommandEvent {
public:
    nsParameterChangeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id) { }

    // イベントデータをコピーするためのCloneメソッド（必須）
    virtual wxEvent* Clone() const override { return new nsParameterChangeEvent(*this); }

    // 独自のデータを保持する場合
    void SetData(const uint32_t& id, const double& param) {
        m_ID = id;
        m_Param = param;
    }

    uint32_t GetID() const {
        return m_ID;
    }

    double GetParam() const {
        return m_Param;
    }

private:
    uint32_t m_ID;
    double m_Param;
};

wxDECLARE_EVENT(nsEVT_PARAMETER_CHANGED, nsParameterChangeEvent);

#endif // NESORA_EVENTS_H

#pragma once

#include <wx/event.h>

#ifndef NESORA_EVENTS_H
#define NESORA_EVENTS_H

class nsSelectedParameterChangeEvent : public wxCommandEvent {
public:
    nsSelectedParameterChangeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id) { }

    // イベントデータをコピーするためのCloneメソッド（必須）
    virtual wxEvent* Clone() const override { return new nsSelectedParameterChangeEvent(*this); }

    // 独自のデータを保持する場合
    void SetData(const uint32_t& data) { m_customData = data; }
    uint32_t GetData() const { return m_customData; }

private:
    uint32_t m_customData;
};

// このマクロは一般的にヘッダファイル内で使用します: これは MY_EVENT イベント型を定義するだけです
wxDECLARE_EVENT(nsEVT_SELECTED_PARAMETER_CHANGED, nsSelectedParameterChangeEvent);

#endif // NESORA_EVENTS_H

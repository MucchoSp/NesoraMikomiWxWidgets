// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_DICTIONALY_INDEX_Panel_H
#define NESORA_DICTIONALY_INDEX_Panel_H

#include <wx/wx.h>

#include "../../NesoraStyle/NesoraStyle.h"
#include "../../NesoraLayout/NesoraIDs.h"

#include "../../Nesora/Nesora.h"



class NesoraDictionalyIndexWord : public wxPanel {
public:
    NesoraDictionalyIndexWord(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

    void SetSelected(bool selected);
    bool IsSelected() const;

    ParametricNesoraDictionalyWord word;
private:
    bool isSelected;

    wxStaticText* wordText;
    wxStaticText* parameterDeltaCountText;

    void OnLeftDown(wxMouseEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void HandleDelete();


    void ProcessItemClick();
};

class NesoraDictionalyIndexScrollContainer : public wxScrolledWindow {
public:
    NesoraDictionalyIndexScrollContainer(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxScrolledWindow(parent, winid, pos, size, style, name) {
        Init();
    }
    ~NesoraDictionalyIndexScrollContainer();

    void Init();

    void AddCard();
    void RemoveSelectCard();
    void SelectItem(NesoraDictionalyIndexWord* item);
    NesoraDictionalyIndexWord* GetSelectedItem() const;

private:
    NesoraVowelDictionaly vowelDictionaly;
    std::vector<NesoraDictionalyIndexWord*> items;

    NesoraDictionalyIndexWord* selectedItem;

    wxBoxSizer* mainSizer;

    void OnLeftDown(wxMouseEvent& event);
    void OnEscapePressed(wxCommandEvent& event);

};


class NesoraDictionalyIndexPanel : public wxPanel {
public:
    NesoraDictionalyIndexPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxPanel(parent, winid, pos, size, style, name) {
        Init();
    }

    void Init();

private:
    NesoraVowelDictionaly vowelDictionaly;

    nsButton* addButton;
    NesoraDictionalyIndexScrollContainer* scrollContainer;
    
    void OnAddButton(wxCommandEvent& event);
};




#endif // NESORA_DICTIONALY_INDEX_Panel_H
// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef NESORA_DICTIONALY_INDEX_PANEL_H
#define NESORA_DICTIONALY_INDEX_PANEL_H

#include <wx/wx.h>

#include "../../../NesoraStyle/NesoraStyle.h"
#include "../../../NesoraLayout/NesoraIDs.h"

#include "../../../Nesora/Nesora.h"
#include "NesoraDictionalyEditPanel.h"
#include "NesoraDictionalyPlayPanel.h"

// MARK: NesoraDictionalyIndexWord

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

    void SetWordText(const std::string& text);
    std::string GetWordText() const;

    void SetSymbolText(const std::string& text);
    std::string GetSymbolText() const;

    void ChangeWord();

    void SetWord(ParametricNesoraDictionalyWord* word) { this->word = word; }
    ParametricNesoraDictionalyWord* GetWord() const { return word; }

private:
    ParametricNesoraDictionalyWord* word;
    
    bool isSelected;
    
    wxStaticText* wordText;
    wxStaticText* symbolText;

    void OnLeftDown(wxMouseEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void HandleDelete();


    void ProcessItemClick();
};




// MARK: NesoraDictionalyIndexScrollContainer

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

    void SetVoice(NesoraMikomiVoice* voice) { this->voice = voice; }
    void SetDictionaly(NesoraVowelDictionaly* dic) { this->vowelDictionaly = dic; }

    void SetEditPanel(NesoraDictionalyEditPanel* editPanel) { this->editPanel = editPanel; }
    void SetPlayPanel(NesoraDictionalyPlayPanel* playPanel) { this->playPanel = playPanel; }

private:
    NesoraMikomiVoice* voice;

    NesoraVowelDictionaly* vowelDictionaly;
    std::vector<NesoraDictionalyIndexWord*> items;

    NesoraDictionalyEditPanel* editPanel;
    NesoraDictionalyPlayPanel* playPanel;

    NesoraDictionalyIndexWord* selectedItem = nullptr;

    wxBoxSizer* mainSizer;

    void OnLeftDown(wxMouseEvent& event);
    void OnEscapePressed(wxCommandEvent& event);

};





// MARK: NesoraDictionalyIndexPanel

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

    void SetVoice(NesoraMikomiVoice* voice);
    void SetDictionaly(NesoraVowelDictionaly* dic);
    void SetEditPanel(NesoraDictionalyEditPanel* editPanel) { scrollContainer->SetEditPanel(editPanel); }
    void SetPlayPanel(NesoraDictionalyPlayPanel* playPanel) { scrollContainer->SetPlayPanel(playPanel); }

private:
    NesoraMikomiVoice* voice;
    NesoraVowelDictionaly* vowelDictionaly;

    nsButton* addButton;
    NesoraDictionalyIndexScrollContainer* scrollContainer;
    
    void OnAddButton(wxCommandEvent& event);
};


#endif // NESORA_DICTIONALY_INDEX_PANEL_H
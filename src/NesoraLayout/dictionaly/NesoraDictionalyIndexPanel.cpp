// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NesoraDictionalyIndexPanel.h"


// MARK: NesoraDictionalyIndexWord

void NesoraDictionalyIndexWord::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wordText = new wxStaticText(this, wxID_ANY, "Word");
    symbolText = new wxStaticText(this, wxID_ANY, "Symbol");
    sizer->Add(wordText, 0, wxEXPAND | wxALL);
    sizer->Add(symbolText, 0, wxEXPAND | wxALL);

    SetSizer(sizer);

    Bind(wxEVT_LEFT_DOWN, &NesoraDictionalyIndexWord::OnLeftDown, this);
    Bind(wxEVT_CONTEXT_MENU, &NesoraDictionalyIndexWord::OnContextMenu, this);
}

void NesoraDictionalyIndexWord::SetSelected(bool selected) {
    isSelected = selected;
    if (isSelected)
        SetFocus();

    if (selected) {
        SetBackgroundColour(nsGetColor(nsColorType::SECONDARY));
        wordText->SetBackgroundColour(nsGetColor(nsColorType::SECONDARY));
        wordText->SetForegroundColour(nsGetColor(nsColorType::ON_SECONDARY));
        symbolText->SetBackgroundColour(nsGetColor(nsColorType::SECONDARY));
        symbolText->SetForegroundColour(nsGetColor(nsColorType::ON_SECONDARY));
    } else {
        SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
        wordText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
        wordText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
        symbolText->SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
        symbolText->SetForegroundColour(nsGetColor(nsColorType::ON_BACKGROUND));
    }
    Refresh();
}

void NesoraDictionalyIndexWord::ChangeWord() {
    if (word.word.empty()) {
        wordText->SetLabelText("Empty");
    }
    else {
        wordText->SetLabelText(word.word);
    }
    symbolText->SetLabelText("[" + word.symbol + "]");
    Refresh();
}

bool NesoraDictionalyIndexWord::IsSelected() const {
    return isSelected;
}

void NesoraDictionalyIndexWord::SetWordText(const std::string& text) {
    wordText->SetLabel(text);
}

void NesoraDictionalyIndexWord::SetSymbolText(const std::string& text) {
    symbolText->SetLabel(text);
}

std::string NesoraDictionalyIndexWord::GetWordText() const {
    return wordText->GetLabel().ToStdString();
}

std::string NesoraDictionalyIndexWord::GetSymbolText() const {
    return symbolText->GetLabel().ToStdString();
}

void NesoraDictionalyIndexWord::OnLeftDown(wxMouseEvent& event) {
    ProcessItemClick();
    SetBackgroundColour(nsGetColor(nsColorType::SECONDARY));
    Refresh();
    event.Skip();
}

void NesoraDictionalyIndexWord::OnContextMenu(wxContextMenuEvent& event) {
    // メニューの作成
    wxMenu menu;
    menu.Append(nsID_MENU_DELETE, _("Delete"));

    menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) {
        this->HandleDelete();
    }, nsID_MENU_DELETE);

    PopupMenu(&menu);
}

void NesoraDictionalyIndexWord::HandleDelete() {
    NesoraDictionalyIndexScrollContainer* parent = (NesoraDictionalyIndexScrollContainer*)GetParent();
    if (parent) {
        this->Destroy();
        parent->RemoveSelectCard();
        parent->Layout();
        parent->FitInside();
    }
}

void NesoraDictionalyIndexWord::ProcessItemClick() {
    NesoraDictionalyIndexScrollContainer* parent = (NesoraDictionalyIndexScrollContainer*)GetParent();
    if (parent) {
        parent->SelectItem(this);
    }
}




// MARK: NesoraDictionalyIndexScrollContainer

void NesoraDictionalyIndexScrollContainer::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    SetScrollRate(0, 20);
    // 背景クリックイベントをバインド
    Bind(wxEVT_LEFT_DOWN, &NesoraDictionalyIndexScrollContainer::OnLeftDown, this);
    
    wxWindow* dictionalyFrame = wxWindow::FindWindowById(nsID_DICTIONALY_PANEL);
    if (dictionalyFrame) {
        dictionalyFrame->Bind(wxEVT_MENU, &NesoraDictionalyIndexScrollContainer::OnEscapePressed, this, nsID_ESCAPE);
    }
}

NesoraDictionalyIndexScrollContainer::~NesoraDictionalyIndexScrollContainer() {
    wxWindow* dictionalyFrame = wxWindow::FindWindowById(nsID_DICTIONALY_PANEL);
    if (dictionalyFrame) {
        dictionalyFrame->Unbind(wxEVT_MENU, &NesoraDictionalyIndexScrollContainer::OnEscapePressed, this, nsID_ESCAPE);
    }
}

// 背景をクリックしたとき
void NesoraDictionalyIndexScrollContainer::OnLeftDown(wxMouseEvent& event) {
    SelectItem(nullptr);
    event.Skip();
}

// ESCキーが押されたとき
void NesoraDictionalyIndexScrollContainer::OnEscapePressed(wxCommandEvent& event) {
    selectedItem = nullptr;
    Refresh();
    event.Skip();
}

void NesoraDictionalyIndexScrollContainer::AddCard() {
    NesoraDictionalyIndexWord* card = new NesoraDictionalyIndexWord(this);
    
    mainSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    SelectItem(card);

    FitInside();

    if (editPanel) {
        editPanel->SetWord(card);
    }
}

void NesoraDictionalyIndexScrollContainer::RemoveSelectCard() {
    selectedItem = nullptr;
}

void NesoraDictionalyIndexScrollContainer::SelectItem(NesoraDictionalyIndexWord* item) {
    if (selectedItem && selectedItem != item) {
        selectedItem->SetSelected(false);
    }
        
    selectedItem = item;
    if (selectedItem) {
        selectedItem->SetSelected(true);
    }

    if (editPanel) {
        if (selectedItem)
            editPanel->SetWord(selectedItem);
    }
}

NesoraDictionalyIndexWord* NesoraDictionalyIndexScrollContainer::GetSelectedItem() const {
    return selectedItem;
}




// MARK: NesoraDictionalyIndexPanel

void NesoraDictionalyIndexPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetMinSize(wxSize(164, -1)); // 幅を固定
    
    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Words"));
    addButton = new nsButton(mainSizer->GetStaticBox(), wxID_ANY, _("+"));
    addButton->Bind(wxEVT_BUTTON, &NesoraDictionalyIndexPanel::OnAddButton, this);
    scrollContainer = new NesoraDictionalyIndexScrollContainer(mainSizer->GetStaticBox(), wxID_ANY);
    
    mainSizer->Add(scrollContainer, 1, wxEXPAND | wxALL);
    mainSizer->Add(addButton, 0, wxEXPAND | wxALL);

    SetSizer(mainSizer);
}

void NesoraDictionalyIndexPanel::OnAddButton(wxCommandEvent& event) {
    scrollContainer->AddCard();
}


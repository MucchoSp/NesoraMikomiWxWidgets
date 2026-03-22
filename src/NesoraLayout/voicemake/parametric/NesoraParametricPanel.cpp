#include "NesoraParametricPanel.h"

// MARK:nsParameterCard

void nsParameterCard::Init(uint32_t in_ID, double in_param) {
    
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* topbarSizer = new wxBoxSizer(wxHORIZONTAL);
    deleteButton = new nsButton(this, wxID_ANY, "x", wxDefaultPosition, wxSize(16, -1));
    deleteButton->Bind(wxEVT_BUTTON, &nsParameterCard::OnDeleteButton, this);
    topbarSizer->Add(deleteButton, 0, wxEXPAND | wxALL);
    nameStaticText = new wxStaticText(this, wxID_ANY, _("Parameter"));
    topbarSizer->Add(nameStaticText, 1, wxEXPAND | wxALL);

    ID = in_ID;
    wxString IDString = wxString::Format(wxT("%X"), ID);
    wxHexTextValidator IDValue(&IDString, 8);
    IDStaticText = new wxStaticText(this, wxID_ANY, "#" + IDString);
    IDStaticText->Bind(wxEVT_LEFT_DCLICK, [this](wxMouseEvent& event) {
            this->IDTextSwitchToEditMode();
            event.Skip();
        });
    IDTextCtrl = new wxTextCtrl(this, wxID_ANY, IDString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, IDValue);
    IDTextCtrl->Bind(wxEVT_COMMAND_TEXT_ENTER, [this](wxEvent& event) {
            this->IDTextSwitchToDisplayMode();
            event.Skip();
        });
    IDTextCtrl->Bind(wxEVT_COMMAND_KILL_FOCUS, [this](wxEvent& event) {
            this->IDTextSwitchToDisplayMode();
            event.Skip();
        });
    IDTextCtrl->Hide();
    parameter = new nsSlider(this, wxID_ANY, 100, 0, 100, wxDefaultPosition, wxSize(100, 15));
    parameter->UseWheel(false);
    parameter->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParameterCard::OnParameterSlide, this);
    
    sizer->Add(topbarSizer, 0, wxEXPAND | wxALL);
    sizer->Add(IDStaticText, 0, wxEXPAND | wxALL);
    sizer->Add(IDTextCtrl, 0, wxEXPAND | wxALL);
    sizer->Add(parameter, 0, wxEXPAND | wxALL);
    
    this->SetSizer(sizer);
    
    this->Bind(wxEVT_LEFT_DOWN, &nsParameterCard::OnLeftDown, this);
    this->Bind(wxEVT_MOTION, &nsParameterCard::OnMouseMove, this);
    this->Bind(wxEVT_LEFT_UP, &nsParameterCard::OnLeftUp, this);
    this->Bind(wxEVT_DESTROY, &nsParameterCard::OnDestroyWindow, this);
}

void nsParameterCard::OnParameterSlide(wxCommandEvent& event) {

}

void nsParameterCard::OnDeleteButton(wxCommandEvent& event) {
    wxWindow* parent = GetParent();

    this->Destroy();

    if (parent) {
        parent->Layout();

        wxScrolledWindow* scrolled = (wxScrolledWindow*)parent;
        if (scrolled) {
            scrolled->FitInside();
        }
    }
}

void nsParameterCard::OnDestroyWindow(wxWindowDestroyEvent& event) {
    nsParameterCardScrollContainer* parent = (nsParameterCardScrollContainer*)GetParent();

    parent->RemoveSelectCard();
    event.Skip();
}

void nsParameterCard::OnLeftDown(wxMouseEvent& event) {
    // 親に通知
    this->ProcessItemClick();

    this->IDTextSwitchToDisplayMode();

    isDragging = true;
    CaptureMouse();
    SetBackgroundColour(nsGetColor(nsColorType::SECONDARY));
    Refresh();
    event.Skip();
}

void nsParameterCard::OnMouseMove(wxMouseEvent& event) {
    if (!isDragging) return;
    wxWindow* parent = GetParent();
    wxPoint screenPos = ClientToScreen(event.GetPosition());
    wxPoint parentPos = parent->ScreenToClient(screenPos);

    // どこに線を引くべきか計算
    int insertionY = -1;
    wxSizer* sizer = parent->GetSizer();
    
    for (auto* node : sizer->GetChildren()) {
        wxWindow* win = node->GetWindow();
        wxRect rect = win->GetRect();
        
        if (parentPos.y < rect.y + rect.height / 2) {
            insertionY = rect.y - 2;
            break;
        }
        insertionY = rect.y + rect.height + 2;
    }

    // プレビュー線の描画
    if (insertionY != lastInsertionY) {
        lastInsertionY = insertionY;

        wxClientDC dc(parent);
        parent->PrepareDC(dc);

        wxDCOverlay dcOverlay(overlay, &dc);
        dcOverlay.Clear();

        dc.SetPen(wxPen(nsGetColor(nsColorType::SECONDARY_HOVER), 3));
        int xx, yy;
        ((wxScrolledWindow*)parent)->CalcUnscrolledPosition(0, insertionY, &xx, &yy);
        dc.DrawLine(0, yy, parent->GetClientSize().x, yy);
    }
}

void nsParameterCard::OnLeftUp(wxMouseEvent& event) {
    if (!isDragging) return;
    
    if(!isSelected)
        SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND)); // 色を戻す
    Refresh();
    
    wxWindow* parent = GetParent();
    wxPoint screenPos = ClientToScreen(event.GetPosition());
    wxPoint parentPos = parent->ScreenToClient(screenPos);
    
    RearrangeInSizer(parent, parentPos);

    wxClientDC dc(GetParent());
    wxDCOverlay dcOverlay(overlay, &dc);
    dcOverlay.Clear();
    // overlay.Reset();
    lastInsertionY = -1;
    
    isDragging = false;
    if (HasCapture()) ReleaseMouse();
}

void nsParameterCard::RearrangeInSizer(wxWindow* parent, wxPoint dropPos) {
    wxSizer* sizer = parent->GetSizer();
    if (!sizer) return;

    int newIndex = -1;
    int currentIndex = -1;
    int i = 0;

    for (auto* node : sizer->GetChildren()) {
        wxWindow* win = node->GetWindow();
        if (win == this) currentIndex = i;
        
        if (win->GetRect().Contains(dropPos)) {
            newIndex = i;
        }
        i++;
    }

    // 位置を入れ替える
    if (newIndex != -1 && newIndex != currentIndex) {
        sizer->Detach(this);
        sizer->Insert(newIndex, this, 0, wxEXPAND | wxALL, 5);
        parent->Layout();
        
        auto* scrolled = dynamic_cast<wxScrolledWindow*>(parent);
        if (scrolled) scrolled->FitInside();
    }
}

void nsParameterCard::SetSelected(bool selected) {
    isSelected = selected;
    
    SetBackgroundColour(selected ? nsGetColor(nsColorType::SECONDARY) : nsGetColor(nsColorType::BACKGROUND));
    nameStaticText->SetBackgroundColour(selected ? nsGetColor(nsColorType::SECONDARY) : nsGetColor(nsColorType::BACKGROUND));
    nameStaticText->SetForegroundColour(selected ? nsGetColor(nsColorType::ON_SECONDARY) : nsGetColor(nsColorType::ON_BACKGROUND));
    IDStaticText->SetBackgroundColour(selected ? nsGetColor(nsColorType::SECONDARY) : nsGetColor(nsColorType::BACKGROUND));
    IDStaticText->SetForegroundColour(selected ? nsGetColor(nsColorType::ON_SECONDARY) : nsGetColor(nsColorType::ON_BACKGROUND));
    if(!isSelected)
        SwitchToDisplayMode();
    Refresh();
}

bool nsParameterCard::IsSelected() const {
    return isSelected;
}

void nsParameterCard::IDTextSwitchToEditMode() {
    if(!IsSelected())
        this->ProcessItemClick();
    
    this->IDStaticText->Hide();
    this->IDTextCtrl->Show();
    this->IDTextCtrl->SetFocus();
    this->IDTextCtrl->SelectAll();
    this->GetSizer()->Layout();
}

void nsParameterCard::IDTextSwitchToDisplayMode() {
    if (!IDTextCtrl->IsShown()) return;

    IDStaticText->SetLabel("#" + IDTextCtrl->GetValue());
    if (!IDTextCtrl->GetValue().ToUInt(&ID, 16)) {
        ID = 0;
        IDTextCtrl->SetValue("00000000");
        IDStaticText->SetLabel("#00000000");
    }

    this->IDTextCtrl->Hide();
    this->IDStaticText->Show();
    this->GetSizer()->Layout();
}

void nsParameterCard::SwitchToDisplayMode() {
    this->IDTextSwitchToDisplayMode();
}

void nsParameterCard::ProcessItemClick() {
    nsParameterCardScrollContainer* parent = (nsParameterCardScrollContainer*)GetParent();
    if (parent) {
        parent->SelectItem(this);
    }
}














// MARK:nsParameterCardScrollContainer

void nsParameterCardScrollContainer::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    mainSizer = new wxBoxSizer(wxVERTICAL);

    this->SetSizer(mainSizer);

    SetScrollRate(0, 20);
}

void nsParameterCardScrollContainer::AddCard() {
    nsParameterCard* card = new nsParameterCard(this);
    
    mainSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    SelectItem(card);

    FitInside();
}

void nsParameterCardScrollContainer::RemoveSelectCard() {
    selectedItem = {};
}

void nsParameterCardScrollContainer::SelectItem(nsParameterCard* item) {
    if (selectedItem && selectedItem != item) {
        selectedItem->SetSelected(false);
    }
        
    selectedItem = item;
    if (selectedItem) {
        selectedItem->SetSelected(true);
    }

    nsSelectedParameterChangeEvent event(nsEVT_SELECTED_PARAMETER_CHANGED, GetId());
    event.SetEventObject(this);

    event.SetData(selectedItem->ID);

    // 送信する
    this->GetEventHandler()->ProcessEvent(event);
}

nsParameterCard* nsParameterCardScrollContainer::GetSelectedItem() const {
    return selectedItem;
}







// MARK:nsParametricPanel

void nsParametricPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    SetMinSize(wxSize(164, -1)); // 幅を固定
    
    wxStaticBoxSizer* sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Parameters"));
    addButton = new nsButton(sizer->GetStaticBox(), wxID_ANY, _("+"));
    addButton->Bind(wxEVT_BUTTON, &nsParametricPanel::OnAddButton, this);
    scrollWindow = new nsParameterCardScrollContainer(sizer->GetStaticBox(), wxID_ANY);

    sizer->Add(addButton, 0, wxEXPAND | wxALL);
    sizer->Add(scrollWindow, 1, wxEXPAND | wxALL);

    this->SetSizer(sizer);
}

void nsParametricPanel::OnAddButton(wxCommandEvent& event) {
    scrollWindow->AddCard();
}

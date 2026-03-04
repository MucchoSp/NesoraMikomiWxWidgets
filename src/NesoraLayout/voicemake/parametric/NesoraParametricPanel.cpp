#include "NesoraParametricPanel.h"


void nsParameterCard::Init(uint32_t in_ID, double in_param) {
    std::cout << "Init" << std::endl;
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    std::cout << "sizer" << std::endl;
    nameTextCtrl = new wxTextCtrl(this, wxID_ANY, _("Parameter"));
    std::cout << "nameTextCtrl" << std::endl;
    wxIntegerValidator<uint32_t> IDValue(ID, nsPARAMETRIC_ID_MIN, nsPARAMETRIC_ID_MAX, wxNUM_VAL_DEFAULT);
    std::cout << "IDValue" << std::endl;
    IDTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, IDValue);
    std::cout << "IDTextCtrl" << std::endl;
    parameter = new nsSlider(this, wxID_ANY, 100, 0, 100, wxDefaultPosition, wxSize(100, 15));
    parameter->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &nsParameterCard::OnParameterSlide, this);
    std::cout << "parameter" << std::endl;

    sizer->Add(nameTextCtrl, 0, wxEXPAND | wxALL);
    sizer->Add(IDTextCtrl, 0, wxEXPAND | wxALL);
    sizer->Add(parameter, 0, wxEXPAND | wxALL);

    this->SetSizer(sizer);
}

void nsParameterCard::OnParameterSlide(wxCommandEvent& event) {

}


void nsParametricPanel::Init() {
    SetBackgroundColour(nsGetColor(nsColorType::BACKGROUND));
    
    wxStaticBoxSizer* sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Parameters"));
    addButton = new wxButton(sizer->GetStaticBox(), wxID_ANY, _("+"));
    addButton->Bind(wxEVT_BUTTON, &nsParametricPanel::OnAddButton, this);
    mainSizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(addButton, 0, wxEXPAND | wxALL);
    sizer->Add(mainSizer, 0, wxEXPAND | wxALL);

    this->SetSizer(sizer);

    SetScrollRate(0, 20);
}

void nsParametricPanel::OnAddButton(wxCommandEvent& event) {
    auto* item = new nsParameterCard(this);
    // 左右いっぱいに広げ、下に少し余白を作る
    mainSizer->Add(item, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    // 内容が更新されたことを通知
    FitInside();
}
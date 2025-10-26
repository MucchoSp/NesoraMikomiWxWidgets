#include "NesoraSpeakPanel.h"

void nsSpeakPanel::Init() {
    // Initialization code here
    wxStaticText* label = new wxStaticText(this, wxID_ANY, "This is the Speak Panel", wxPoint(10, 10));
}

void nsSpeakPanel::OnSave(wxCommandEvent& event) {
    // Handle save action for Speak Panel
    wxLogMessage("喋るパネルの保存する処理は実装されていませんよ！");
}

void nsSpeakPanel::OnOpen(wxCommandEvent& event) {
    // Handle open action for Speak Panel
    wxLogMessage("喋るパネルの開く処理は実装されていませんよ！");
}

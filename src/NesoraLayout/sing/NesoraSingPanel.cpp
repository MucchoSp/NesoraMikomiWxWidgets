#include "NesoraSingPanel.h"

void nsSingPanel::Init() {
    // Initialization code here
    wxStaticText* label = new wxStaticText(this, wxID_ANY, "This is the Sing Panel", wxPoint(10, 10));
}

void nsSingPanel::OnSave(wxCommandEvent& event) {
    // Handle save action for Sing Panel
    wxLogMessage("歌うパネルの保存する処理はまだ未実装です...");
}

void nsSingPanel::OnOpen(wxCommandEvent& event) {
    // Handle open action for Sing Panel
    wxLogMessage("歌うパネルの開く処理はまだ未実装です...");
}

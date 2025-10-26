#include "NesoraSingPanel.h"

void nsSingPanel::Init() {
    wxStaticText* label = new wxStaticText(this, wxID_ANY, _T("This is the Sing Panel"), wxPoint(10, 10));
}

void nsSingPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_T("歌うパネルの保存する処理はまだ未実装です..."));
}

void nsSingPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_T("歌うパネルの開く処理はまだ未実装です..."));
}

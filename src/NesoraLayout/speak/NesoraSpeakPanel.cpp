#include "NesoraSpeakPanel.h"

void nsSpeakPanel::Init() {
    wxStaticText* label = new wxStaticText(this, wxID_ANY, _T("This is the Speak Panel"), wxPoint(10, 10));
}

void nsSpeakPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_T(""));
}

void nsSpeakPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_T(""));
}

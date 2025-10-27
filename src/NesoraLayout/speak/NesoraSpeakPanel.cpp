#include "NesoraSpeakPanel.h"

void nsSpeakPanel::Init() {
    wxStaticText* label = new wxStaticText(this, wxID_ANY, _("This is the Speak Panel"), wxPoint(10, 10));
}

void nsSpeakPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_("The process for saving the talking panel has not been implemented!"));
}

void nsSpeakPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_("The process for opening the talking panel has not been implemented!"));
}

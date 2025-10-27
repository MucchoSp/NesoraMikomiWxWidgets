#include "NesoraSingPanel.h"

void nsSingPanel::Init() {
    wxStaticText* label = new wxStaticText(this, wxID_ANY, _("This is the Sing Panel"), wxPoint(10, 10));
}

void nsSingPanel::OnSave(wxCommandEvent& event) {
    wxLogMessage(_("The process for saving the singing panel has not yet been implemented..."));
}

void nsSingPanel::OnOpen(wxCommandEvent& event) {
    wxLogMessage(_("The process for opening the singing panel has not yet been implemented..."));
}

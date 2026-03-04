#pragma once

#ifndef NESORA_PARAMETRIC_PANEL_H
#define NESORA_PARAMETRIC_PANEL_H

#include <wx/wx.h>
#include <wx/vscroll.h>
#include <wx/valnum.h>

#include "../../../NesoraStyle/NesoraStyle.h"

#include "../../NesoraIDs.h"

#include "../../../Nesora/Nesora.h"
#include "../../../Nesora/filter/NesoraParametricSOSIIRFilter/NesoraParametricSOSIIRFilter.h"


// MARK:nsParameterCard

class nsParameterCard : public wxPanel {
public:
    nsParameterCard(wxWindow* parent) {
        Init();
    }

    uint32_t* ID;

    void Init(uint32_t ID = 0, double param = 0);

    wxTextCtrl* nameTextCtrl;
    wxTextCtrl* IDTextCtrl;
    nsSlider* parameter;

private:

    void OnParameterSlide(wxCommandEvent& event);

};

// MARK:nsParametricPanel

class nsParametricPanel : public wxScrolledWindow {
public:
    nsParametricPanel() {
        Init();
    }

    nsParametricPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxScrolledWindow(parent, winid, pos, size, style, name)
    {
        Init();
    }

    void Init();

private:

    std::map<uint32_t, double> parameter;
    std::vector<nsParameterCard> parameters;

    wxButton* addButton;
    wxVScrolledWindow* scrollWindow;
    wxBoxSizer* mainSizer;

    void OnAddButton(wxCommandEvent& event);

};







#endif // NESORA_PARAMETRIC_PANEL_H
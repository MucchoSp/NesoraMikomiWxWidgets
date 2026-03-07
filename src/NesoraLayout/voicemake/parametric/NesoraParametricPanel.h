#pragma once

#ifndef NESORA_PARAMETRIC_PANEL_H
#define NESORA_PARAMETRIC_PANEL_H

#include <wx/wx.h>
#include <wx/vscroll.h>
#include <wx/valnum.h>
#include <wx/overlay.h>

#include "../../../NesoraStyle/NesoraStyle.h"

#include "../../NesoraIDs.h"

#include "../../../Nesora/Nesora.h"
#include "../../../Nesora/filter/NesoraParametricSOSIIRFilter/NesoraParametricSOSIIRFilter.h"



// MARK:wxHexTextValidator
// https://forums.wxwidgets.org/viewtopic.php?t=16962

class wxHexTextValidator : public wxValidator
{
protected:
	int m_maxLen, m_minLen;
	wxString * m_strValue;
public:
	wxHexTextValidator(wxString *val, int maxLen = -1, int minLen = 0)
		: m_strValue(val)
		, m_maxLen(maxLen), m_minLen(minLen)
	{
	}
	wxHexTextValidator(const wxHexTextValidator & val)
    : wxValidator()
	{
        Copy(val);
        this->Bind(wxEVT_CHAR, &wxHexTextValidator::OnChar, this);
	}
	// Make a clone of this validator (or return NULL) - currently necessary
	// if you're passing a reference to a validator.
	// Another possibility is to always pass a pointer to a new validator
	// (so the calling code can use a copy constructor of the relevant class).
	wxObject* Clone() const // Note the 'const' here
	{
        return new wxHexTextValidator(*this);
	}
	bool Copy(const wxHexTextValidator& val)
	{
        wxValidator::Copy(val);
		m_strValue = val.m_strValue;
		m_minLen = val.m_minLen;
		m_maxLen = val.m_maxLen;
		return true;
	}
    
	// Called to transfer data from the window
	bool TransferFromWindow()
	{
        if( !CheckValidator()) return false;
		if ( m_strValue) *m_strValue = ((wxTextCtrl *)m_validatorWindow)->GetValue();
		return true;
	}
	// Called to transfer data to the window
	bool TransferToWindow()
	{
        if( !CheckValidator()) return false;
		if ( m_strValue) ((wxTextCtrl *)m_validatorWindow)->SetValue( (*m_strValue).Upper());
		return true;
	}
	// Called when the value in the window must be validated.
	// This function can pop up an error message.
	bool Validate(wxWindow* parent)
	{
        wxString val(((wxTextCtrl*)m_validatorWindow)->GetValue());
        
		// valid the length
		size_t len = val.Len();
		if ( len < m_minLen || (m_maxLen > 0 && len > m_maxLen))
        return false;
        
		// valid the value
		for ( size_t i = 0; i < val.Len(); ++i) {
            if ( !wxIsxdigit(val[i])) return false;
		}
		return true;
	}
	bool CheckValidator() const
	{
        wxCHECK_MSG(m_validatorWindow,false,_T("No window associated with validator"));
		wxCHECK_MSG(m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)),false,_T("wxTextValidator is only for wxTextCtrl's"));
		return true;
	}
    void OnChar(wxKeyEvent& event) 
    {
        if ( m_validatorWindow ) {
            wxTextCtrl * ctrl = (wxTextCtrl *)m_validatorWindow;
            size_t len = ctrl->GetValue().Len() - ctrl->GetStringSelection().Len();
            int keyCode = event.GetKeyCode();

            // we don't filter special keys and Delete
            if ( keyCode < WXK_SPACE || keyCode == WXK_DELETE || keyCode > WXK_START) {
                event.Skip();
                return ;
            }

            // validate length & character/input
            if ( 
                (m_maxLen > 0 &&  (len + 1) > m_maxLen) // valid length
                ||
                !wxIsxdigit(keyCode) // character
                )
            {
                if ( !wxValidator::IsSilent()) wxBell();
                // eat message
                return ;
            }

            // Uppercase
            if ( wxIsxdigit(keyCode)) {
                wxChar c = wxToupper(keyCode);
                ctrl->WriteText( c);
                // eat msg! we have done it
                return;
            }
        }
    }
    // DECLARE_EVENT_TABLE();
};






// MARK:nsParameterCard

class nsParameterCard : public wxPanel {
public:
    nsParameterCard(wxWindow* parent) : wxPanel(parent) {
        Init();
    }

    uint32_t* ID;

    void Init(uint32_t ID = 0, double param = 0);

    void SetSelected(bool selected);
    bool IsSelected() const;

    wxStaticText* nameStaticText;
    wxStaticText* IDStaticText;
    wxTextCtrl* IDTextCtrl;
    nsSlider* parameter;

private:

    nsButton* deleteButton;
    bool isDragging = false;
    bool isSelected = false;
    wxOverlay overlay;
    int lastInsertionY = -1;

    void OnParameterSlide(wxCommandEvent& event);
    void OnDeleteButton(wxCommandEvent& event);
    
    void OnLeftDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void RearrangeInSizer(wxWindow* parent, wxPoint dropPos);

    void IDTextSwitchToEditMode();
    void IDTextSwitchToDisplayMode();
    void SwitchToDisplayMode();
    void ProcessItemClick();

};

// MARK:nsParametricPanel

class nsParameterCardScrollContainer : public wxScrolledWindow {
public:
    nsParameterCardScrollContainer(wxWindow* parent) : wxScrolledWindow(parent, wxID_ANY) {
        Init();
    }

    nsParameterCardScrollContainer(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxScrolledWindow(parent, winid, pos, size, style, name)
    {
        Init();
    }

    void Init();

    void AddCard();
    void SelectItem(nsParameterCard* item);
    nsParameterCard* GetSelectedItem() const;

private:

    std::map<uint32_t, double> parameter;
    std::vector<nsParameterCard> parameters;

    nsParameterCard* selectedItem;

    wxBoxSizer* mainSizer;

};

// MARK:nsParametricPanel

class nsParametricPanel : public wxWindow {
public:
    nsParametricPanel() {
        Init();
    }

    nsParametricPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr)) : wxWindow(parent, winid, pos, size, style, name)
    {
        Init();
    }

    void Init();

private:

    std::map<uint32_t, double> parameter;
    std::vector<nsParameterCard> parameters;

    nsButton* addButton;
    nsParameterCardScrollContainer* scrollWindow;

    void OnAddButton(wxCommandEvent& event);

};







#endif // NESORA_PARAMETRIC_PANEL_H
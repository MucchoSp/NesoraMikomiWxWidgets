#include "NesoraToolBar.hpp"

void nsToolBarButton::onPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    if(isSelected) {
        if(mouseHover) {
            if(mouseDown)
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_HOVER)));
            else
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY_VARIANT)));
        }
        else
            dc.SetBrush(wxBrush(nsGetColor(nsColorType::PRIMARY)));
        dc.SetTextForeground(nsGetColor(nsColorType::ON_PRIMARY));
    }
    else {
        if(mouseHover) {
            if(mouseDown)
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
            else
                dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND_HOVER)));
        }
        else
            dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
        dc.SetTextForeground(nsGetColor(nsColorType::ON_BACKGROUND));
    }
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    dc.SetFont(GetFont());
    wxCoord textWidth, textHeight;
    dc.GetTextExtent(GetLabel(), &textWidth, &textHeight);
    dc.DrawText(GetLabel(), (size.GetWidth() - textWidth) / 2, (size.GetHeight() - textHeight) / 2);
}

void nsToolBarButton::SetSelected(bool selected) {
    isSelected = selected;
    Refresh();
}

void nsToolBarButton::onMouseUp(wxMouseEvent& event) {
    mouseDown = false;
    wxWindow::Refresh();
    event.Skip();
}

void nsToolBarButton::onMouseDown(wxMouseEvent& event) {
    mouseDown = true;
    wxWindow::Refresh();
    event.Skip();
}
void nsToolBarButton::onMouseEnter(wxMouseEvent& event) {
    mouseHover = true;
    wxWindow::Refresh();
    event.Skip();
}
void nsToolBarButton::onMouseLeave(wxMouseEvent& event) {
    mouseHover = false;
    wxWindow::Refresh();
    event.Skip();
}




void nsToolBar::onPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    dc.SetBrush(wxBrush(nsGetColor(nsColorType::BACKGROUND)));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
}

void nsToolBar::onSpeakButtonClicked(wxCommandEvent& event) {
    SetSelectedTool(nsToolBarType::TOOLBAR_SPEAK);
    event.Skip();
}

void nsToolBar::onSingButtonClicked(wxCommandEvent& event) {
    SetSelectedTool(nsToolBarType::TOOLBAR_SING);
    event.Skip();
}

void nsToolBar::onVoiceMakeButtonClicked(wxCommandEvent& event) {
    SetSelectedTool(nsToolBarType::TOOLBAR_VOICE_MAKE);
    event.Skip();
}

void nsToolBar::SetSelectedTool(nsToolBarType tool) {
    nowSelected = tool;
    speakButton->SetSelected(tool == nsToolBarType::TOOLBAR_SPEAK);
    singButton->SetSelected(tool == nsToolBarType::TOOLBAR_SING);
    voiceMakeButton->SetSelected(tool == nsToolBarType::TOOLBAR_VOICE_MAKE);
    wxWindow::Refresh();
}

nsToolBarType nsToolBar::GetSelectedTool() const {
    return nowSelected;
}

#include <wx/wx.h>
#include <wx/sizer.h>

class wxCustomButton : public wxWindow
{

    bool pressedDown;
    wxString text;

    static const int buttonWidth = 100;
    static const int buttonHeight = 20;

public:
    wxCustomButton(wxFrame* parent, wxString text);

    void paintEvent(wxPaintEvent& evt);
    void paintNow();

    void render(wxDC& dc);

    // some useful events
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxCustomButton, wxPanel)

EVT_MOTION(wxCustomButton::mouseMoved)
EVT_LEFT_DOWN(wxCustomButton::mouseDown)
EVT_LEFT_UP(wxCustomButton::mouseReleased)
EVT_RIGHT_DOWN(wxCustomButton::rightClick)
EVT_LEAVE_WINDOW(wxCustomButton::mouseLeftWindow)
EVT_KEY_DOWN(wxCustomButton::keyPressed)
EVT_KEY_UP(wxCustomButton::keyReleased)
EVT_MOUSEWHEEL(wxCustomButton::mouseWheelMoved)

// catch paint events
EVT_PAINT(wxCustomButton::paintEvent)

END_EVENT_TABLE()



wxCustomButton::wxCustomButton(wxFrame* parent, wxString text) :
    wxWindow(parent, wxID_ANY)
{
    SetMinSize(wxSize(buttonWidth, buttonHeight));
    this->text = text;
    pressedDown = false;
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void wxCustomButton::paintEvent(wxPaintEvent& evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void wxCustomButton::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void wxCustomButton::render(wxDC& dc)
{
    if (pressedDown) {
        dc.SetBrush(*wxRED_BRUSH);
        dc.SetPen(*wxRED_PEN);
    }
    else {
        dc.SetBrush(*wxGREY_BRUSH);
        dc.SetPen(*wxGREY_PEN);
    }

    dc.DrawRectangle(0, 0, buttonWidth, buttonHeight);
    dc.DrawText(text, 0, 0);
}

void wxCustomButton::mouseDown(wxMouseEvent& event)
{
    pressedDown = true;
    paintNow();
}
void wxCustomButton::mouseReleased(wxMouseEvent& event)
{
    pressedDown = false;
    paintNow();

    wxMessageBox(wxT("You pressed a custom button"));
}
void wxCustomButton::mouseLeftWindow(wxMouseEvent& event)
{
    if (pressedDown)
    {
        pressedDown = false;
        paintNow();
    }
}

// currently unused events
void wxCustomButton::mouseMoved(wxMouseEvent& event) {}
void wxCustomButton::mouseWheelMoved(wxMouseEvent& event) {}
void wxCustomButton::rightClick(wxMouseEvent& event) {}
void wxCustomButton::keyPressed(wxKeyEvent& event) {}
void wxCustomButton::keyReleased(wxKeyEvent& event) {}


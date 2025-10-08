#pragma once
#include <wx/wx.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../lib/miniaudio_split/miniaudio.h"

#include "NesoraStyle/button/NesoraButton.h"
#include "NesoraStyle/chart/NesoraChart.h"
#include "NesoraStyle/chart/NesoraSimpleChart.h"
#include "NesoraStyle/slider/NesoraSlider.h"
#include "NesoraStyle/NesoraColor.h"

#include "NesoraLayout/NesoraIDs.h"
#include "NesoraLayout/voicemake/NesoraVoiceMakePanel.h"

#include "Nesora/source/NesoraSource.h"

class MyApp : public wxApp {
public:
    bool OnInit() override;
};

class MyFrame : public wxFrame {
public:
    MyFrame();

private:

    nsVoiceMakePanel* voicemakepanel;

    void MenuSetup();


    wxPanel* globalPanel;
    wxPanel* toolSelector;

    nsButton* speakButton;
    nsButton* makeButton;

    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSpeakButton(wxCommandEvent& event);
    void OnMakeButton(wxCommandEvent& event);

};

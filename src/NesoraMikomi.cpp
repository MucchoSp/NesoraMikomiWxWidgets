#include "NesoraMikomi.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    nsMainFrame* frame = new nsMainFrame();
    frame->Show(true);
    return true;
}

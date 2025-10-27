#include "NesoraMikomi.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    locale.Init(wxLANGUAGE_DEFAULT);
    locale.AddCatalogLookupPathPrefix(_T("./locale/"));
    locale.AddCatalog(_T("NesoraMikomiWxWidgets"));
    nsMainFrame* frame = new nsMainFrame();
    frame->Show(true);
    return true;
}

// 音諳一号機零型
// Copyright (c) 2026 MucchoSP
// SPDX-License-Identifier: AGPL-3.0-or-later
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

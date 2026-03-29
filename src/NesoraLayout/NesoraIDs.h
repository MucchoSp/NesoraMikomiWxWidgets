#pragma once

#include <wx/defs.h>

#ifndef NESORA_IDS_H
#define NESORA_IDS_H

enum {
    ID_Hello = wxID_HIGHEST,
    ID_EXIT,
    nsID_SPEAK_BUTTON,
    nsID_SING_BUTTON,
    nsID_VOICE_MAKE_BUTTON,
    nsID_MAIN_FRAME,
    nsID_SPEAK_PANEL,
    nsID_SING_PANEL,
    nsID_VOICE_MAKE_PANEL,
    
    nsID_VOICE_SAVE = wxID_HIGHEST + 1000,
    nsID_SING_SAVE,
    nsID_SPEAK_SAVE,

    nsID_ESCAPE,
    nsID_MENU_EDIT,
    nsID_MENU_DELETE,
};

#endif // NESORA_IDS_H
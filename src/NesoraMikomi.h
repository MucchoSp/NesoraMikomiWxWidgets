/*
* 音諳一号機零型
* Copyright (c) 2026 MucchoSP
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or 
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

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
#include "NesoraLayout/frame/NesoraMainFrame.h"

#include "Nesora/Nesora.h"

class MyApp : public wxApp {
public:
    bool OnInit() override;
private:
    wxLocale locale;
};

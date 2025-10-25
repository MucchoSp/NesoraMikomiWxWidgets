#pragma once

#include <wx/wx.h>

#ifndef NESORA_STYLE_H
#define NESORA_STYLE_H


#include "./button/NesoraButton.h"
#include "./chart/NesoraChart.h"
#include "./chart/NesoraSimpleChart.h"
#include "./slider/NesoraSlider.h"

#include "NesoraColor.h"



inline bool nsHitTest(const wxRect2DDouble& rect, const wxPoint2DDouble& p) {
    if(rect.m_x < p.m_x and rect.m_x + rect.m_width > p.m_x) {
        if(rect.m_y < p.m_y and rect.m_y + rect.m_height > p.m_y) {
            return true;
        }
    }
    return false;
}

inline bool nsHitTest(const wxRect2DDouble& rect, double x, double y) {
    if(rect.m_x < x and rect.m_x + rect.m_width > x) {
        if(rect.m_y < y and rect.m_y + rect.m_height > y) {
            return true;
        }
    }
    return false;
}


#endif // NESORA_STYLE_H
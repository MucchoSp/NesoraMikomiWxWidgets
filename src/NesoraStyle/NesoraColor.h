#pragma once

#include <wx/wx.h>

#ifndef NESORA_COLOR_H
#define NESORA_COLOR_H

enum class nsColorType {
    PRIMARY,
    SECONDARY,
    SUCCESS,
    DANGER,
    WARNING,
    INFO,
    LIGHT,
    DARK
};

class nsColor {
public:
    nsColor() {}

    wxColour GetColor(nsColorType type) const {
        switch (type) {
            case nsColorType::PRIMARY:
                return wxColour(0, 123, 255);
            case nsColorType::SECONDARY:
                return wxColour(108, 117, 125);
            case nsColorType::SUCCESS:
                return wxColour(40, 167, 69);
            case nsColorType::DANGER:
                return wxColour(220, 53, 69);
            case nsColorType::WARNING:
                return wxColour(255, 193, 7);
            case nsColorType::INFO:
                return wxColour(23, 162, 184);
            case nsColorType::LIGHT:
                return wxColour(248, 249, 250);
            case nsColorType::DARK:
                return wxColour(52, 58, 64);
            default:
                return wxColour(0, 0, 0); // Default to black
        }
    }

private:

};

#endif // NESORA_COLOR_H

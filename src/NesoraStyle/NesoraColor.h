#pragma once

#include <wx/wx.h>
#include <wx/settings.h>

#ifndef NESORA_COLOR_H
#define NESORA_COLOR_H

enum class nsColorType {
    PRIMARY,
    PRIMARY_HOVER,
    PRIMARY_VARIANT,
    PRIMARY_SHADOW,
    ON_PRIMARY,
    SECONDARY,
    SECONDARY_HOVER,
    SECONDARY_VARIANT,
    SECONDARY_SHADOW,
    ON_SECONDARY,
    BACKGROUND,
    BACKGROUND_HOVER,
    BACKGROUND_SHADOW,
    ON_BACKGROUND,


};

inline wxColour nsGetColor(nsColorType type) {
    switch(type) {
        case nsColorType::PRIMARY:
            return wxColour(0x21, 0x28, 0x4C);
        case nsColorType::PRIMARY_HOVER:
        return wxColour(0x16, 0x1B, 0x33);
        case nsColorType::PRIMARY_VARIANT:
        return wxColour(0x2C, 0x36, 0x66);
        case nsColorType::PRIMARY_SHADOW:
            return wxColour(0xF9, 0xFD, 0xFF, 0x1F);
        case nsColorType::ON_PRIMARY:
            return wxColour(0xF9, 0xFD, 0xFF);

        case nsColorType::SECONDARY:
            return wxColour(0xC5, 0xDE, 0x8C);
        case nsColorType::SECONDARY_HOVER:
            return wxColour(0xDC, 0xF7, 0x9C);
        case nsColorType::SECONDARY_VARIANT:        
            return wxColour(0xAF, 0xC4, 0x7C);
        case nsColorType::SECONDARY_SHADOW:
            return wxColour(0x00, 0x00, 0x00, 0x1F);
        case nsColorType::ON_SECONDARY:
            return wxColour(0xF9, 0xFD, 0xFF);

        case nsColorType::BACKGROUND:
            return wxColour(0xF9, 0xFD, 0xFF);
        case nsColorType::BACKGROUND_HOVER:
            return wxColour(0xF5, 0xF5, 0xF5);
        case nsColorType::BACKGROUND_SHADOW:
            return wxColour(0x00, 0x00, 0x00, 0x1F);
        case nsColorType::ON_BACKGROUND:
            return wxColour(0x00, 0x00, 0x00);

        default:
            return wxColour(0x00, 0x00, 0x00);
    };
}

#endif // NESORA_COLOR_H

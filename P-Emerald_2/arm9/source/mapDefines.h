/*
Pok�mon Emerald 2 Version
------------------------------

file        : mapDefines.h
author      : Philip Wellnitz
description : General map stuff.

Copyright (C) 2012 - 2015
Philip Wellnitz

This file is part of Pok�mon Emerald 2 Version.

Pok�mon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pok�mon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pok�mon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <nds/ndstypes.h>

namespace MAP {
    struct position {
        u16 m_posX; //Global
        u16 m_posY; //Global
        u8 m_posZ;
    };
    enum direction : u8 {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };
    enum moveMode {
        //Player modes
        WALK = 0,
        SURF = 1,
        BIKE = 2,
        ACRO_BIKE = 3,
        SIT = 4,
        //NPC modes
        NOTHING = 0,
    };

}
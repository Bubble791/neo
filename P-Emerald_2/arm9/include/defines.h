/*
Pok�mon Emerald 2 Version
------------------------------

file        : defines.h
author      : Philip Wellnitz (RedArceus)
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

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

#include <nds/ndstypes.h>
//#define USE_AS_LIB

#define RGB(r, g, b) (RGB15((r), (g), (b)) | BIT(15))

#define COLOR_IDX (u8(253))
#define WHITE_IDX (u8(250))
#define GRAY_IDX (u8(251))
#define BLACK_IDX (u8(252))
#define RED_IDX (u8(253))
#define BLUE_IDX (u8(254))

#define CHOICE_COLOR RGB(16,25,19)

#define BG_PAL( p_sub ) ( ( p_sub ) ? BG_PALETTE_SUB : BG_PALETTE )
#define BG_BMP( p_sub ) ( ( p_sub ) ? BG_BMP_RAM_SUB( 1 ) : BG_BMP_RAM( 1 ) )


#define TIMER_SPEED (BUS_CLOCK/1024)
#define sq(a) ((a)*(a))

//Main sprite's OAM indices
#define BACK_ID  0
#define SAVE_ID  1
#define PKMN_ID  2
#define ID_ID  3
#define DEX_ID  4
#define BAG_ID  5
#define OPTS_ID  6
#define NAV_ID  7
#define A_ID  8
//  9 - unused
// 10 - unused
// 11 - unused
// 12 - unused
#define FWD_ID  13
#define BWD_ID  14
#define CHOICE_ID  15
// 14 additional spaces used
#define BORDER_ID 31
// 9 additional spaces used
#define SQCH_ID 80
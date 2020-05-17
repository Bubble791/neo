/*
Pokémon neo
------------------------------

file        : defines.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <cmath>
#include <cstdio>
#include <string>
#include <nds.h>
#include <nds/ndstypes.h>

#define VERSION ("0.8-" __DATE__ " " __TIME__)
#define VERSION_NAME "Magnetizing Magnemite neo"
#define GAME_TITLE "POKEMON NEO"

// Assumes that the Backup is a 512k flash memory
#define BACKUP_SIZE ( 512 * 1024 )

#define MAX_ITEMS 1280
#define MAX_ITEMS_IN_BAG MAX_ITEMS
#define MAX_ATTACK 800
#define MAX_MOVE MAX_ATTACK
#define MAX_PKMN 890
#define OTLENGTH 8
#define PKMN_NAMELENGTH 15
#define ITEM_NAMELENGTH 20
#define MOVE_NAMELENGTH 20
#define ABILITY_NAMELENGTH 20
#define LOCATION_NAMELENGTH 25

#undef RAND_MAX
#define RAND_MAX 4294967295

enum GameMod : u8 { DEVELOPER, ALPHA, BETA, RELEASE, EMULATOR };
extern GameMod gMod;

u8          getCurrentDaytime( );

// num frames between button input
#define COOLDOWN_COUNT 8

extern bool DRAW_TIME;
extern bool UPDATE_TIME;
extern bool ANIMATE_MAP;
extern u8   FRAME_COUNT;
extern bool SCREENS_SWAPPED;
extern bool PLAYER_IS_FISHING;
extern bool INIT_NITROFS;

extern char** ARGV;

extern int achours, acseconds, acminutes, acday, acmonth, acyear;
extern int hours, seconds, minutes, day, month, year;

extern int pressed, held, last;

extern bool TWL_CONFIG;

extern unsigned int   TEMP[ 12288 ];
extern unsigned short TEMP_PAL[ 256 ];

#define LANGUAGES 2
#define MAX_STRINGS 500

extern const char*       LANGUAGE_NAMES[ LANGUAGES ];
extern const char* const STRINGS[ MAX_STRINGS ][ LANGUAGES ];
#define CURRENT_LANGUAGE SAVE::SAV.getActiveFile( ).m_options.m_language
#define GET_STRING( i ) STRINGS[ i ][ CURRENT_LANGUAGE ]

#define NO_DATA GET_STRING( 0 )
#define FARAWAY_PLACE GET_STRING( 1 )
#define UNKNOWN_SPECIES GET_STRING( 2 )
#define POKE_NAV GET_STRING( 8 )

#define loop( ) while( 1 )

#define IN_RANGE_I( p_touch, p_input )                                                     \
    ( ( p_touch ).px >= ( p_input ).m_targetX1 && ( p_touch ).py >= ( p_input ).m_targetY1 \
      && ( p_touch ).px <= ( p_input ).m_targetX2 && ( p_touch ).py <= ( p_input ).m_targetY2 )
#define IN_RANGE_I_C( p_touch, p_input )                      \
    ( sqrt( sq( ( p_touch ).px - ( p_input ).m_targetX1 )     \
            + sq( ( p_touch ).py - ( p_input ).m_targetY1 ) ) \
      <= ( p_input ).m_targetR )

#define IN_RANGE_R( p_x1, p_y1, p_x2, p_y2 ) \
    IN_RANGE_I( touch, IO::inputTarget( p_x1, p_y1, p_x2, p_y2 ) )
#define IN_RANGE_C( p_x, p_y, p_r ) IN_RANGE_I_C( touch, IO::inputTarget( p_x, p_y, p_r ) )

#define TOUCH_UP ( !touch.px && !touch.py )

#define GET_KEY_COOLDOWN( p_key ) \
    ( pressed & ( p_key ) ) || ( ( held & ( p_key ) ) && !( --cooldown ) )

#define GET_AND_WAIT( p_key ) \
    ( ( pressed & ( p_key ) ) && IO::waitForInput( IO::inputTarget( p_key ) ) )
#define GET_AND_WAIT_R( p_x1, p_y1, p_x2, p_y2 )                     \
    ( IN_RANGE_I( touch, IO::inputTarget( p_x1, p_y1, p_x2, p_y2 ) ) \
      && IO::waitForInput( IO::inputTarget( p_x1, p_y1, p_x2, p_y2 ) ) )
#define GET_AND_WAIT_C( p_x, p_y, p_r )                       \
    ( IN_RANGE_I_C( touch, IO::inputTarget( p_x, p_y, p_r ) ) \
      && IO::waitForInput( IO::inputTarget( p_x, p_y, p_r ) ) )

#define GET_DIR( p_dir )               \
    ( ( (p_dir) &KEY_DOWN )            \
          ? MAP::direction::DOWN       \
          : ( ( (p_dir) &KEY_UP )      \
                  ? MAP::direction::UP \
                  : ( ( (p_dir) &KEY_RIGHT ) ? MAP::direction::RIGHT : MAP::direction::LEFT ) ) )

#define IN_DEX( pidx ) ( SAVE::SAV.m_caughtPkmn[ ( pidx ) / 8 ] & ( 1 << ( ( pidx ) % 8 ) ) )

#define RGB( p_r, p_g, p_b ) ( RGB15( ( p_r ), ( p_g ), ( p_b ) ) | BIT( 15 ) )
#define COMPL( p_color )                                                    \
    ( RGB( 31 - ( ( p_color ) >> 10 ) % 32, 31 - ( ( p_color ) >> 5 ) % 32, \
           31 - ( p_color ) % 32 ) )

#define RED2_IDX ( u8( 247 ) )
#define BLUE2_IDX ( u8( 248 ) )
#define COLOR_IDX ( u8( 249 ) )
#define WHITE_IDX ( u8( 250 ) )
#define GRAY_IDX ( u8( 251 ) )
#define BLACK_IDX ( u8( 252 ) )
#define RED_IDX ( u8( 253 ) )
#define BLUE_IDX ( u8( 254 ) )

#define CHOICE_COLOR RGB( 16, 25, 19 )

#define GREEN RGB( 0, 20, 0 )
#define RED RGB( 28, 0, 0 )
#define RED2 RGB( 10, 0, 0 )
#define BLUE RGB( 0, 10, 31 )
#define BLUE2 RGB( 0, 0, 15 )
#define WHITE RGB( 30, 30, 30 )
#define GRAY RGB( 15, 15, 15 )
#define NORMAL_COLOR RGB( 27, 27, 27 )
#define BLACK RGB( 0, 0, 0 )
#define YELLOW RGB( 24, 24, 0 )
#define PURPLE RGB( 24, 0, 24 )
#define TURQOISE RGB( 0, 24, 24 )
#define ICE_COLOR RGB( 15, 31, 31 )
#define FAIRY_COLOR RGB( 31, 15, 31 )
#define GROUND_COLOR RGB( 31, 31, 15 )
#define POISON_COLOR RGB( 31, 0, 15 )
#define ORANGE RGB( 31, 15, 0 )
#define GHOST_COLOR RGB( 15, 0, 31 )
#define ROCK_COLOR RGB( 28, 23, 7 )
#define BUG_COLOR RGB( 15, 28, 7 )
#define STEEL_COLOR RGB( 24, 24, 24 )
#define DRAGON_COLOR RGB( 7, 7, 24 )
#define UNKNOWN_COLOR RGB( 0, 42, 42 )

#define BG_PAL( p_sub ) ( ( p_sub ) ? BG_PALETTE_SUB : BG_PALETTE )
#define BG_BMP( p_sub ) ( ( p_sub ) ? BG_BMP_RAM_SUB( 1 ) : BG_BMP_RAM( 1 ) )

#define TIMER_SPEED ( BUS_CLOCK / 1024 )
#define sq( a ) ( ( a ) * ( a ) )

// Main sprite's OAM indices
#define BACK_ID 0
#define SAVE_ID 1
#define PKMN_ID 2
#define ID_ID 3
#define DEX_ID 4
#define BAG_ID 5
#define OPTS_ID 6
#define NAV_ID 7
#define A_ID 8
#define ITM( a ) ( 9 + ( a ) )
#define FWD_ID 13
#define BWD_ID 14
#define ITM_BACK 15

#define SQCH_ID 80

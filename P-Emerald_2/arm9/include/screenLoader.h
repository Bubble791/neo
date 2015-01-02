/*
    Pok�mon Emerald 2 Version
    ------------------------------

    file        : screenLoader.h
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

#pragma once

#include <nds.h>

#include "sprite.h"
#include "pokemon.h"
#include "defines.h"
#include <string>

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
class saveGame;
extern saveGame SAV;

extern PrintConsole Top, Bottom;
extern ConsoleFont cfont;
extern int achours, acseconds, acminutes, acday, acmonth, acyear;
extern int hours, seconds, minutes, day, month, year;
extern u32 ticks;

extern POKEMON::pokemon::boxPokemon stored_pkmn[ MAXSTOREDPKMN ];
extern std::vector<int> box_of_st_pkmn[ MAXPKMN ];
extern std::vector<int> free_spaces;

extern SpriteInfo spriteInfo[ SPRITE_COUNT ];
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];
extern OAMTable *Oam, *OamTop;

extern int drawBox( u16 );

extern void initMapSprites( );
extern bool movePlayerOnMap( s16, s16, s16, bool );

#define MAXMAPPOS 75
struct MapRegionPos {
    u8 m_lx,
        m_ly,
        m_rx,
        m_ry;
    u16 m_ind;
};
extern const MapRegionPos MapLocations[ 3 ][ MAXMAPPOS ];
void printMapLocation( const MapRegionPos& p_m );
extern void printMapLocation( const touchPosition& t );

class move;
extern move* AttackList[ 560 ];
extern void shoUseAttack( u16 p_pkmIdx, bool p_female, bool p_shiny );

enum Region {
    NONE = 0,
    HOENN = 1,
    KANTO = 2,
    JOHTO = 3
};
extern std::map<u16, std::string> Locations;
extern Region acMapRegion;
extern bool showmappointer;

#define MAXBG 3
#define START_BG 0
struct backgroundSet {
    std::string             m_name;
    const unsigned int      *m_mainMenu;
    const unsigned short    *m_mainMenuPal;
    bool                    m_loadFromRom;
    bool                    m_allowsOverlay;
    u8                      *m_mainMenuSpritePoses;
};
extern backgroundSet BGs[ MAXBG ];

void vramSetup( );

void updateTime( s8 p_mapMode = 0 );
void animateMap( u8 p_frame );

void initVideoSub( );
void drawSub( );
void drawSub( u8 p_newIdx );

void animateBack( );
void setMainSpriteVisibility( bool p_hidden, bool p_save = false );

void drawTypeIcon( OAMTable *p_oam, SpriteInfo * p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16 & p_nextAvailableTileIdx, Type p_type, u16 p_posX, u16 p_posY, bool p_bottom );

u8 getCurrentDaytime( );

extern std::string bagnames[ 8 ];
class screenLoader {
private:
    s8 _pos;

public:
    screenLoader( s8 p_pos ) : _pos( p_pos ) { }

    void draw( s8 p_mode );
    void init( );

    void run_bag( );
    void run_pkmn( );
    void run_dex( u16 p_num = 0 );
};

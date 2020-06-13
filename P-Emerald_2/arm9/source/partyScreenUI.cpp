/*
Pokémon neo
------------------------------

file        : partyScreenUI.cpp
author      : Philip Wellnitz
description : Draw UI for the pkmn party screen

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

#include "partyScreenUI.h"
#include "defines.h"
#include "saveGame.h"
#include "screenFade.h"
#include "uio.h"
#include "pokemonNames.h"

#include "hpbar.h"
#include "itemicon.h"
#include "party_1.h"
#include "party_2.h"
#include "party_3.h"
#include "party_4.h"
#include "party_5.h"
#include "party_6.h"
#include "party_blank1.h"
#include "party_blank2.h"
#include "party_box1.h"
#include "party_box2.h"
#include "party_box_sel1.h"
#include "party_box_sel2.h"
#include "party_fnt1.h"
#include "party_fnt2.h"
#include "party_fnt_sel1.h"
#include "party_fnt_sel2.h"
#include "party_mark1.h"
#include "party_mark2.h"
#include "partybg.h"
#include "partybg2.h"
#include "partysub.h"
#include "status_brn.h"
#include "status_fnt.h"
#include "status_frz.h"
#include "status_par.h"
#include "status_psn.h"
#include "status_shiny.h"
#include "status_slp.h"
#include "status_txc.h"

#include "NoPkmn.h"

#include "noselection_160_64_1.h"
#include "noselection_160_64_2.h"
#include "noselection_32_32.h"
#include "noselection_32_64.h"
#include "noselection_96_32_1.h"
#include "noselection_96_32_2.h"
#include "noselection_blank_32_32.h"
#include "noselection_faint_32_32.h"
#include "selection_32_32.h"
#include "selection_faint_32_32.h"

#include "arrow.h"
#include "x_16_16.h"

// Top screen defines

#define SPR_HP_BAR_OAM( p_pos ) ( p_pos )
#define SPR_PKMN_BG_OAM( p_pos ) ( 6 + 2 * ( p_pos ) )
#define SPR_ITEM_ICON_OAM( p_pos ) ( 18 + ( p_pos ) )
#define SPR_STATUS_ICON_OAM( p_pos ) ( 24 + ( p_pos ) )
#define SPR_PKMN_ICON_OAM( p_pos ) ( 30 + ( p_pos ) )
#define SPR_SHINY_ICON_OAM( p_pos ) ( 36 + ( p_pos ) )
#define SPR_SWAP_OAM( p_pos ) ( 42 + 3 * ( p_pos ) )
#define SPR_MARK_OAM( p_pos, p_color ) ( 60 + 6 * ( p_pos ) + ( p_color ) )
#define SPR_PKMN_BG_PAL 0
#define SPR_PKMN_ICON_PAL( p_pos ) ( 4 + ( p_pos ) )
#define SPR_ITEM_ICON_PAL 10
#define SPR_STATUS_ICON_PAL 11
#define SPR_HP_BAR_PAL 12
#define SPR_SHINY_ICON_PAL 13
#define SPR_SWAP_PAL 14
#define SPR_MARK_PAL 15

#define SPR_PKMN_BG_GFX( p_type ) ( 0x40 * 2 * ( p_type ) )

// Bottom screen defines

#define SPR_PKMN_BG_OAM_SUB( p_pos ) ( ( p_pos ) )
#define SPR_ITEM_ICON_OAM_SUB( p_pos ) ( 6 + ( p_pos ) )
#define SPR_PKMN_ICON_OAM_SUB( p_pos ) ( 12 + ( p_pos ) )
#define SPR_ARROW_LEFT_OAM_SUB 18
#define SPR_ARROW_RIGHT_OAM_SUB 19
#define SPR_X_OAM_SUB 20
#define SPR_PAGE_LEFT_OAM_SUB 21
#define SPR_PAGE_RIGHT_OAM_SUB 24
#define SPR_PAGE_OAM_SUB( p_page ) ( 22 + ( p_page ) )
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 25 + 6 * ( p_pos ) )
#define SPR_MSG_BOX_OAM_SUB 67
#define SPR_ITEM_OAM_SUB 74
#define SPR_WINDOW_PAL_SUB 0
#define SPR_PKMN_ICON_PAL_SUB( p_pos ) ( 2 + ( p_pos ) )
#define SPR_ITEM_ICON_PAL_SUB 8
#define SPR_ARROW_X_PAL_SUB 9
#define SPR_BOX_PAL_SUB 10
#define SPR_ITEM_PAL_SUB 11

namespace STS {
    char BUFFER[ 50 ];

    partyScreenUI::partyScreenUI( pokemon p_team[ 6 ], u8 p_teamLength, u8 p_toSelect ) {
        _team       = p_team;
        _teamLength = p_teamLength;
        _toSelect   = p_toSelect;
    }

#ifdef DESQUID
    void partyScreenUI::updateTeamLength( u8 p_newLength ) {
        _teamLength = p_newLength;
    }
#endif

    u16 partyScreenUI::initTopScreen( bool p_bottom ) {
        IO::clearScreen( p_bottom );
        IO::initOAMTable( p_bottom );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );

        u16 tileCnt = SPR_PKMN_BG_GFX( 3 );
        // preload sprites to avoid position calculations later
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        // left half
        for( size_t i = 0; i < 3; i++ ) {
            u8 pos = 2 * i;
            // background "box"
            IO::loadSprite( SPR_PKMN_BG_OAM( pos ), SPR_PKMN_BG_PAL, SPR_PKMN_BG_GFX( 2 ), 4,
                            4 + 61 * i, 64, 64, party_blank1Pal, party_blank1Tiles,
                            party_blank1TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                            OBJMODE_BLENDED );
            IO::loadSprite( SPR_PKMN_BG_OAM( pos ) + 1, SPR_PKMN_BG_PAL,
                            SPR_PKMN_BG_GFX( 2 ) + 0x40, 68, 4 + 61 * i, 64, 64, party_blank1Pal,
                            party_blank2Tiles, party_blank2TilesLen, false, false, false,
                            OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

            // PKMN icon
            tileCnt = IO::loadSprite( SPR_PKMN_ICON_OAM( pos ), SPR_PKMN_ICON_PAL( pos ), tileCnt,
                                      4, partyTopScreenPkmnIconPosY( pos ), 32, 32, NoPkmnPal,
                                      NoPkmnTiles, NoPkmnTilesLen, false, false, true,
                                      OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );

            if( i ) {
                // Item icon
                IO::loadSprite( SPR_ITEM_ICON_OAM( pos ), SPR_ITEM_ICON_PAL,
                                oam[ SPR_ITEM_ICON_OAM( 0 ) ].gfxIndex, 3 + 32 - 9,
                                partyTopScreenPkmnIconPosY( pos ) + 32 - 8, 8, 8, itemiconPal,
                                itemiconTiles, itemiconTilesLen, false, false, true, OBJPRIORITY_0,
                                p_bottom, OBJMODE_NORMAL );

                // HP bar
                IO::loadSprite( SPR_HP_BAR_OAM( pos ), SPR_HP_BAR_PAL,
                                oam[ SPR_HP_BAR_OAM( 0 ) ].gfxIndex, 120 - 48, 34 + 61 * i, 64, 32,
                                hpbarPal, hpbarTiles, hpbarTilesLen, false, false, true,
                                OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );

                // mark
                IO::loadSprite( SPR_SWAP_OAM( pos ), SPR_SWAP_PAL,
                                oam[ SPR_SWAP_OAM( 0 ) ].gfxIndex, 0, 2 + 61 * i, 64, 64,
                                party_mark2Pal, party_mark2Tiles, party_mark2TilesLen, true, true,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_SWAP_OAM( pos ) + 1, SPR_SWAP_PAL,
                                oam[ SPR_SWAP_OAM( 0 ) + 1 ].gfxIndex, 37, 3 + 61 * i, 32, 64,
                                party_mark1Pal, party_mark1Tiles, party_mark1TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_SWAP_OAM( pos ) + 2, SPR_SWAP_PAL,
                                oam[ SPR_SWAP_OAM( 0 ) ].gfxIndex, 65, 3 + 61 * i, 64, 64,
                                party_mark2Pal, party_mark2Tiles, party_mark2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_MARK_OAM( pos, 0 ), SPR_MARK_PAL,
                                oam[ SPR_MARK_OAM( 0, 0 ) ].gfxIndex, 24, 4 + 61 * i, 32, 16,
                                party_1Pal, party_1Tiles, party_1TilesLen, false, false, true,
                                OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_MARK_OAM( pos, 1 ), SPR_MARK_PAL,
                                oam[ SPR_MARK_OAM( 0, 1 ) ].gfxIndex, 24, 4 + 61 * i, 32, 16,
                                party_1Pal, party_2Tiles, party_1TilesLen, false, false, true,
                                OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_MARK_OAM( pos, 2 ), SPR_MARK_PAL,
                                oam[ SPR_MARK_OAM( 0, 2 ) ].gfxIndex, 24, 4 + 61 * i, 32, 16,
                                party_1Pal, party_3Tiles, party_1TilesLen, false, false, true,
                                OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_MARK_OAM( pos, 3 ), SPR_MARK_PAL,
                                oam[ SPR_MARK_OAM( 0, 3 ) ].gfxIndex, 24, 4 + 61 * i, 32, 16,
                                party_1Pal, party_4Tiles, party_1TilesLen, false, false, true,
                                OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_MARK_OAM( pos, 4 ), SPR_MARK_PAL,
                                oam[ SPR_MARK_OAM( 0, 4 ) ].gfxIndex, 24, 4 + 61 * i, 32, 16,
                                party_1Pal, party_5Tiles, party_1TilesLen, false, false, true,
                                OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_MARK_OAM( pos, 5 ), SPR_MARK_PAL,
                                oam[ SPR_MARK_OAM( 0, 5 ) ].gfxIndex, 24, 4 + 61 * i, 32, 16,
                                party_1Pal, party_6Tiles, party_1TilesLen, false, false, true,
                                OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
            } else {
                // Item icon
                tileCnt = IO::loadSprite( SPR_ITEM_ICON_OAM( pos ), SPR_ITEM_ICON_PAL, tileCnt,
                                          3 + 32 - 9, partyTopScreenPkmnIconPosY( pos ) + 32 - 8, 8,
                                          8, itemiconPal, itemiconTiles, itemiconTilesLen, false,
                                          false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

                // HP bar
                tileCnt
                    = IO::loadSprite( SPR_HP_BAR_OAM( pos ), SPR_HP_BAR_PAL, tileCnt, 120 - 48,
                                      34 + 61 * i, 64, 32, hpbarPal, hpbarTiles, hpbarTilesLen,
                                      false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );

                // mark
                tileCnt
                    = IO::loadSprite( SPR_SWAP_OAM( pos ), SPR_SWAP_PAL, tileCnt, 0, 2 + 61 * i, 64,
                                      64, party_mark2Pal, party_mark2Tiles, party_mark2TilesLen,
                                      true, true, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                tileCnt = IO::loadSprite( SPR_SWAP_OAM( pos ) + 1, SPR_SWAP_PAL, tileCnt, 37,
                                          3 + 61 * i, 32, 64, party_mark1Pal, party_mark1Tiles,
                                          party_mark1TilesLen, false, false, true, OBJPRIORITY_3,
                                          p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_SWAP_OAM( pos ) + 2, SPR_SWAP_PAL,
                                oam[ SPR_SWAP_OAM( pos ) ].gfxIndex, 65, 3 + 61 * i, 64, 64,
                                party_mark2Pal, party_mark2Tiles, party_mark2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                tileCnt
                    = IO::loadSprite( SPR_MARK_OAM( pos, 0 ), SPR_MARK_PAL, tileCnt, 24, 4 + 61 * i,
                                      32, 16, party_1Pal, party_1Tiles, party_1TilesLen, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                tileCnt
                    = IO::loadSprite( SPR_MARK_OAM( pos, 1 ), SPR_MARK_PAL, tileCnt, 24, 4 + 61 * i,
                                      32, 16, party_1Pal, party_2Tiles, party_1TilesLen, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                tileCnt
                    = IO::loadSprite( SPR_MARK_OAM( pos, 2 ), SPR_MARK_PAL, tileCnt, 24, 4 + 61 * i,
                                      32, 16, party_1Pal, party_3Tiles, party_1TilesLen, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                tileCnt
                    = IO::loadSprite( SPR_MARK_OAM( pos, 3 ), SPR_MARK_PAL, tileCnt, 24, 4 + 61 * i,
                                      32, 16, party_1Pal, party_4Tiles, party_1TilesLen, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                tileCnt
                    = IO::loadSprite( SPR_MARK_OAM( pos, 4 ), SPR_MARK_PAL, tileCnt, 24, 4 + 61 * i,
                                      32, 16, party_1Pal, party_5Tiles, party_1TilesLen, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
                tileCnt
                    = IO::loadSprite( SPR_MARK_OAM( pos, 5 ), SPR_MARK_PAL, tileCnt, 24, 4 + 61 * i,
                                      32, 16, party_1Pal, party_6Tiles, party_1TilesLen, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
            }

            // Status icon
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM( pos ), SPR_STATUS_ICON_PAL, tileCnt,
                                      130 - 62 - 22, 33 + 61 * i, 8, 8, status_parPal,
                                      status_parTiles, status_parTilesLen / 2, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            // Shiny icon
            tileCnt = IO::loadSprite( SPR_SHINY_ICON_OAM( pos ), SPR_SHINY_ICON_PAL, tileCnt,
                                      130 - 62 - 32, 33 + 61 * i, 8, 8, status_shinyPal,
                                      status_shinyTiles, status_shinyTilesLen, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        }

        // right half
        for( size_t i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;

            // background "box"
            IO::loadSprite( SPR_PKMN_BG_OAM( pos ), SPR_PKMN_BG_PAL, SPR_PKMN_BG_GFX( 2 ), 131,
                            12 + 61 * i, 64, 64, party_blank1Pal, party_blank1Tiles,
                            party_blank1TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                            OBJMODE_BLENDED );
            IO::loadSprite( SPR_PKMN_BG_OAM( pos ) + 1, SPR_PKMN_BG_PAL,
                            SPR_PKMN_BG_GFX( 2 ) + 0x40, 195, 12 + 61 * i, 64, 64, party_blank1Pal,
                            party_blank2Tiles, party_blank2TilesLen, false, false, false,
                            OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

            // PKMN icon
            tileCnt = IO::loadSprite( SPR_PKMN_ICON_OAM( pos ), SPR_PKMN_ICON_PAL( pos ), tileCnt,
                                      131, partyTopScreenPkmnIconPosY( pos ), 32, 32, NoPkmnPal,
                                      NoPkmnTiles, NoPkmnTilesLen, false, false, true,
                                      OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );

            // Item icon
            IO::loadSprite( SPR_ITEM_ICON_OAM( pos ), SPR_ITEM_ICON_PAL,
                            oam[ SPR_ITEM_ICON_OAM( 0 ) ].gfxIndex, 130 + 32 - 9,
                            partyTopScreenPkmnIconPosY( pos ) + 32 - 8, 8, 8, itemiconPal,
                            itemiconTiles, itemiconTilesLen, false, false, true, OBJPRIORITY_0,
                            p_bottom, OBJMODE_NORMAL );

            // Status icon
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM( pos ), SPR_STATUS_ICON_PAL, tileCnt,
                                      257 - 62 - 22, 41 + 61 * i, 8, 8, status_parPal,
                                      status_parTiles, status_parTilesLen / 2, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            // Shiny icon
            tileCnt = IO::loadSprite( SPR_SHINY_ICON_OAM( pos ), SPR_SHINY_ICON_PAL, tileCnt,
                                      257 - 62 - 32, 41 + 61 * i, 8, 8, status_shinyPal,
                                      status_shinyTiles, status_shinyTilesLen, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

            // HP bar
            IO::loadSprite( SPR_HP_BAR_OAM( pos ), SPR_HP_BAR_PAL,
                            oam[ SPR_HP_BAR_OAM( 0 ) ].gfxIndex, 247 - 48, 42 + 61 * i, 64, 32,
                            hpbarPal, hpbarTiles, hpbarTilesLen, false, false, true, OBJPRIORITY_3,
                            p_bottom, OBJMODE_NORMAL );

            // mark
            IO::loadSprite( SPR_SWAP_OAM( pos ), SPR_SWAP_PAL, oam[ SPR_SWAP_OAM( 0 ) ].gfxIndex,
                            127, 10 + 61 * i, 64, 64, party_mark2Pal, party_mark2Tiles,
                            party_mark2TilesLen, true, true, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_BLENDED );
            IO::loadSprite( SPR_SWAP_OAM( pos ) + 1, SPR_SWAP_PAL,
                            oam[ SPR_SWAP_OAM( 0 ) + 1 ].gfxIndex, 164, 11 + 61 * i, 32, 64,
                            party_mark1Pal, party_mark1Tiles, party_mark1TilesLen, false, false,
                            true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            IO::loadSprite( SPR_SWAP_OAM( pos ) + 2, SPR_SWAP_PAL,
                            oam[ SPR_SWAP_OAM( 0 ) ].gfxIndex, 132 + 60, 11 + 61 * i, 64, 64,
                            party_mark2Pal, party_mark2Tiles, party_mark2TilesLen, false, false,
                            true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            IO::loadSprite( SPR_MARK_OAM( pos, 0 ), SPR_MARK_PAL,
                            oam[ SPR_MARK_OAM( 0, 0 ) ].gfxIndex, 151, 12 + 61 * i, 32, 16,
                            party_1Pal, party_1Tiles, party_1TilesLen, false, false, true,
                            OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
            IO::loadSprite( SPR_MARK_OAM( pos, 1 ), SPR_MARK_PAL,
                            oam[ SPR_MARK_OAM( 0, 1 ) ].gfxIndex, 151, 12 + 61 * i, 32, 16,
                            party_1Pal, party_2Tiles, party_1TilesLen, false, false, true,
                            OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
            IO::loadSprite( SPR_MARK_OAM( pos, 2 ), SPR_MARK_PAL,
                            oam[ SPR_MARK_OAM( 0, 2 ) ].gfxIndex, 151, 12 + 61 * i, 32, 16,
                            party_1Pal, party_3Tiles, party_1TilesLen, false, false, true,
                            OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
            IO::loadSprite( SPR_MARK_OAM( pos, 3 ), SPR_MARK_PAL,
                            oam[ SPR_MARK_OAM( 0, 3 ) ].gfxIndex, 151, 12 + 61 * i, 32, 16,
                            party_1Pal, party_4Tiles, party_1TilesLen, false, false, true,
                            OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
            IO::loadSprite( SPR_MARK_OAM( pos, 4 ), SPR_MARK_PAL,
                            oam[ SPR_MARK_OAM( 0, 4 ) ].gfxIndex, 151, 12 + 61 * i, 32, 16,
                            party_1Pal, party_5Tiles, party_1TilesLen, false, false, true,
                            OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
            IO::loadSprite( SPR_MARK_OAM( pos, 5 ), SPR_MARK_PAL,
                            oam[ SPR_MARK_OAM( 0, 5 ) ].gfxIndex, 151, 12 + 61 * i, 32, 16,
                            party_1Pal, party_6Tiles, party_1TilesLen, false, false, true,
                            OBJPRIORITY_0, p_bottom, OBJMODE_BLENDED );
        }
        return tileCnt;
    }

    u16 partyScreenUI::initBottomScreen( bool p_bottom ) {
        IO::clearScreen( p_bottom );
        if( p_bottom ) {
            dmaCopy( partysubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        } else {
            dmaCopy( partysubBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 256 );
        }

        IO::initOAMTable( p_bottom );

        SpriteEntry* oam     = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          tileCnt = 0;

        // 6 154
        for( u8 i = 0; i < 6; ++i ) {
            if( i ) {
                // Item icon
                IO::loadSprite( SPR_ITEM_ICON_OAM_SUB( i ), SPR_ITEM_ICON_PAL_SUB,
                                oam[ SPR_ITEM_ICON_OAM_SUB( 0 ) ].gfxIndex, 6 - 8 + 36 * i + 32,
                                154 - 10 + 32, 8, 8, itemiconPal, itemiconTiles, itemiconTilesLen,
                                false, false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            } else {
                // Item icon
                tileCnt = IO::loadSprite( SPR_ITEM_ICON_OAM_SUB( i ), SPR_ITEM_ICON_PAL_SUB,
                                          tileCnt, 6 - 8 + 36 * i + 32, 154 - 10 + 32, 8, 8,
                                          itemiconPal, itemiconTiles, itemiconTilesLen, false,
                                          false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            }
            // Pkmn BG box
            tileCnt = IO::loadSprite(
                SPR_PKMN_BG_OAM_SUB( i ), SPR_WINDOW_PAL_SUB, tileCnt, 6 + 36 * i, 154, 32, 32,
                noselection_32_32Pal, noselection_blank_32_32Tiles, noselection_blank_32_32TilesLen,
                false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

            // Pkmn icon
            tileCnt
                = IO::loadSprite( SPR_PKMN_ICON_OAM_SUB( i ), SPR_PKMN_ICON_PAL_SUB( i ), tileCnt,
                                  6 + 36 * i, 154, 32, 32, NoPkmnPal, NoPkmnTiles, NoPkmnTilesLen,
                                  false, false, true, OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );
        }

        // Arrows
        tileCnt = IO::loadSprite( SPR_ARROW_LEFT_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 4, 76, 16,
                                  16, arrowPal, arrowTiles, arrowTilesLen, false, false, true,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ARROW_RIGHT_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                        oam[ SPR_ARROW_LEFT_OAM_SUB ].gfxIndex, 236, 76, 16, 16, arrowPal,
                        arrowTiles, arrowTilesLen, false, true, true, OBJPRIORITY_1, p_bottom,
                        OBJMODE_NORMAL );

        // x
        tileCnt = IO::loadSprite( SPR_X_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 236, 172, 16, 16,
                                  x_16_16Pal, x_16_16Tiles, x_16_16TilesLen, false, false, false,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        // page windows
        tileCnt = IO::loadSprite( SPR_PAGE_LEFT_OAM_SUB, SPR_BOX_PAL_SUB, tileCnt, 0 - 8, 57 - 12,
                                  32, 64, noselection_32_64Pal, noselection_32_64Tiles,
                                  noselection_32_64TilesLen, true, true, true, OBJPRIORITY_2,
                                  p_bottom, OBJMODE_NORMAL );
        for( u8 i = 0; i < 3; i++ ) {
            IO::loadSprite( SPR_PAGE_OAM_SUB( i ), SPR_BOX_PAL_SUB,
                            oam[ SPR_PAGE_LEFT_OAM_SUB ].gfxIndex, 256 - 24, 4 + 28 * i, 32, 64,
                            noselection_32_64Pal, noselection_32_64Tiles, noselection_32_64TilesLen,
                            false, false, true, OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );
        }

        // choice windows

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i;

            if( !i ) {
                tileCnt
                    = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB, tileCnt, 29,
                                      32 + i * 36, 16, 32, noselection_96_32_1Pal,
                                      noselection_96_32_1Tiles, noselection_96_32_1TilesLen, false,
                                      false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
                tileCnt
                    = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB, tileCnt,
                                      29 + 16, 32 + i * 36, 16, 32, noselection_96_32_2Pal,
                                      noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false,
                                      false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            } else {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29, 32 + i * 36, 16,
                                32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                                noselection_96_32_1TilesLen, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_NORMAL );
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 16,
                                32 + i * 36, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }
            for( u8 j = 2; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 16,
                                32 + i * 36, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29 + 5 * 16, 32 + i * 36,
                            16, 32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                            noselection_96_32_1TilesLen, true, true, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_NORMAL );
        }
        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, 32 + i * 36, 16, 32,
                            noselection_96_32_1Pal, noselection_96_32_1Tiles,
                            noselection_96_32_1TilesLen, false, false, true, OBJPRIORITY_3,
                            p_bottom, OBJMODE_NORMAL );
            for( u8 j = 1; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 16,
                                32 + i * 36, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131 + 5 * 16,
                            32 + i * 36, 16, 32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                            noselection_96_32_1TilesLen, true, true, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_NORMAL );
        }

        // message box

        tileCnt = IO::loadSprite( SPR_MSG_BOX_OAM_SUB, SPR_BOX_PAL_SUB, tileCnt, 28, 52, 32, 64, 0,
                                  noselection_160_64_1Tiles, noselection_160_64_1TilesLen, false,
                                  false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        tileCnt
            = IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 1, SPR_BOX_PAL_SUB, tileCnt, 28 + 32, 52, 32,
                              64, 0, noselection_160_64_2Tiles, noselection_160_64_2TilesLen, false,
                              false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 2, SPR_BOX_PAL_SUB,
                        oam[ SPR_MSG_BOX_OAM_SUB + 1 ].gfxIndex, 68, 52, 32, 64, 0,
                        noselection_160_64_2Tiles, noselection_160_64_2TilesLen, false, false, true,
                        OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 3, SPR_BOX_PAL_SUB,
                        oam[ SPR_MSG_BOX_OAM_SUB + 1 ].gfxIndex, 68 + 32, 52, 32, 64, 0,
                        noselection_160_64_2Tiles, noselection_160_64_2TilesLen, false, false, true,
                        OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 4, SPR_BOX_PAL_SUB,
                        oam[ SPR_MSG_BOX_OAM_SUB + 1 ].gfxIndex, 68 + 64, 52, 32, 64, 0,
                        noselection_160_64_2Tiles, noselection_160_64_2TilesLen, false, false, true,
                        OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 5, SPR_BOX_PAL_SUB,
                        oam[ SPR_MSG_BOX_OAM_SUB + 1 ].gfxIndex, 68 + 96, 52, 32, 64, 0,
                        noselection_160_64_2Tiles, noselection_160_64_2TilesLen, false, false, true,
                        OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 6, SPR_BOX_PAL_SUB,
                        oam[ SPR_MSG_BOX_OAM_SUB ].gfxIndex, 68 + 128, 52, 32, 64, 0,
                        noselection_160_64_1Tiles, noselection_160_64_1TilesLen, true, true, true,
                        OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );

        tileCnt = IO::loadSprite( SPR_ITEM_OAM_SUB, SPR_ITEM_PAL_SUB, tileCnt, 32, 68, 32, 32,
                                  NoPkmnPal, NoPkmnTiles, NoPkmnTilesLen, false, false, true,
                                  OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

        // build the shared pals
        IO::copySpritePal( noselection_32_32Pal, SPR_WINDOW_PAL_SUB, 0, 2 * 4, p_bottom );
        IO::copySpritePal( noselection_faint_32_32Pal + 4, SPR_WINDOW_PAL_SUB, 4, 2 * 3, p_bottom );
        IO::copySpritePal( noselection_blank_32_32Pal + 7, SPR_WINDOW_PAL_SUB, 7, 2 * 1, p_bottom );
        IO::copySpritePal( selection_32_32Pal + 8, SPR_WINDOW_PAL_SUB, 8, 2 * 4, p_bottom );
        IO::copySpritePal( selection_faint_32_32Pal + 12, SPR_WINDOW_PAL_SUB, 12, 2 * 4, p_bottom );

        IO::copySpritePal( arrowPal, SPR_ARROW_X_PAL_SUB, 0, 2 * 4, p_bottom );
        IO::copySpritePal( x_16_16Pal + 4, SPR_ARROW_X_PAL_SUB, 4, 2 * 3, p_bottom );

        IO::updateOAM( p_bottom );
        return tileCnt;
    }

    void partyScreenUI::drawPartyPkmn( u8 p_pos, bool p_selected, bool p_redraw,
                                       const char* p_message, bool p_bottom ) {
        drawPartyPkmnSub( p_pos, p_selected, p_redraw, p_message, !p_bottom );

        SpriteEntry* oam      = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          anchor_x = oam[ SPR_PKMN_BG_OAM( p_pos ) ].x;
        u16          anchor_y = oam[ SPR_PKMN_BG_OAM( p_pos ) ].y;

        if( p_redraw ) {
            IO::printRectangle( anchor_x, anchor_y, anchor_x + 124, anchor_y + 61, p_bottom, 0 );
            oam[ SPR_HP_BAR_OAM( p_pos ) ].isHidden      = true; // hp bar
            oam[ SPR_ITEM_ICON_OAM( p_pos ) ].isHidden   = true; // item
            oam[ SPR_STATUS_ICON_OAM( p_pos ) ].isHidden = true; // status
            oam[ SPR_PKMN_BG_OAM( p_pos ) ].isHidden     = true; // pkmn
            oam[ SPR_SHINY_ICON_OAM( p_pos ) ].isHidden  = true; // shiny
        }

        if( p_pos >= _teamLength ) {
            // No Pkmn -> draw empty box
            SpriteEntry old = oam[ SPR_PKMN_BG_OAM( p_pos ) ];
            IO::loadSprite( SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL, SPR_PKMN_BG_GFX( 2 ), old.x,
                            old.y, 64, 64, party_blank1Pal, party_blank1Tiles, party_blank1TilesLen,
                            false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            old = oam[ 1 + SPR_PKMN_BG_OAM( p_pos ) ];
            IO::loadSprite( 1 + SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL,
                            SPR_PKMN_BG_GFX( 2 ) + 0x40, old.x, old.y, 64, 64, party_blank1Pal,
                            party_blank2Tiles, party_blank2TilesLen, false, false, false,
                            OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

            IO::updateOAM( p_bottom );
            return;
        }
        if( _team[ p_pos ].m_stats.m_curHP ) {
            // Pkmn is not fainted
            if( p_selected ) {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 3, SPR_PKMN_BG_GFX( 0 ),
                                old.x, old.y, 64, 64, party_box_sel1Pal, party_box_sel1Tiles,
                                party_box_sel1TilesLen, false, false, false, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
                old = oam[ 1 + SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( 1 + SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 3,
                                SPR_PKMN_BG_GFX( 0 ) + 0x40, old.x, old.y, 64, 64,
                                party_box_sel1Pal, party_box_sel2Tiles, party_box_sel2TilesLen,
                                false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            } else {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 1, SPR_PKMN_BG_GFX( 1 ),
                                old.x, old.y, 64, 64, party_box1Pal, party_box1Tiles,
                                party_box1TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                                OBJMODE_BLENDED );
                old = oam[ 1 + SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( 1 + SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 1,
                                SPR_PKMN_BG_GFX( 1 ) + 0x40, old.x, old.y, 64, 64, party_box1Pal,
                                party_box2Tiles, party_box2TilesLen, false, false, false,
                                OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            }
        } else {
            // Pkmn is fainted
            if( p_selected ) {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 3, SPR_PKMN_BG_GFX( 0 ),
                                old.x, old.y, 64, 64, party_fnt_sel1Pal, party_fnt_sel1Tiles,
                                party_fnt_sel1TilesLen, false, false, false, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
                old = oam[ 1 + SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( 1 + SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 3,
                                SPR_PKMN_BG_GFX( 0 ) + 0x40, old.x, old.y, 64, 64,
                                party_fnt_sel1Pal, party_fnt_sel2Tiles, party_fnt_sel2TilesLen,
                                false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            } else {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 2, SPR_PKMN_BG_GFX( 1 ),
                                old.x, old.y, 64, 64, party_fnt1Pal, party_fnt1Tiles,
                                party_fnt1TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                                OBJMODE_BLENDED );
                old = oam[ 1 + SPR_PKMN_BG_OAM( p_pos ) ];
                IO::loadSprite( 1 + SPR_PKMN_BG_OAM( p_pos ), SPR_PKMN_BG_PAL + 2,
                                SPR_PKMN_BG_GFX( 1 ) + 0x40, old.x, old.y, 64, 64, party_fnt1Pal,
                                party_fnt2Tiles, party_fnt2TilesLen, false, false, false,
                                OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            }
        }

        if( _team[ p_pos ].isEgg( ) ) {
            if( p_redraw ) {
                // general data
                IO::regularFont->printString( GET_STRING( 34 ), anchor_x + 32, anchor_y + 12,
                                              false );
                IO::loadEggIcon( oam[ SPR_PKMN_ICON_OAM( p_pos ) ].x,
                                 oam[ SPR_PKMN_ICON_OAM( p_pos ) ].y, SPR_PKMN_ICON_OAM( p_pos ),
                                 SPR_PKMN_ICON_PAL( p_pos ),
                                 oam[ SPR_PKMN_ICON_OAM( p_pos ) ].gfxIndex, p_bottom,
                                 _team[ p_pos ].getSpecies( ) == PKMN_MANAPHY );
            }
        } else {
            if( p_redraw ) {
                // general data
                if( IO::regularFont->stringWidth( _team[ p_pos ].m_boxdata.m_name ) > 80 ) {
                    IO::regularFont->printStringC( _team[ p_pos ].m_boxdata.m_name, anchor_x + 32,
                                              anchor_y + 12, false );
                } else {
                    IO::regularFont->printString( _team[ p_pos ].m_boxdata.m_name, anchor_x + 32,
                                              anchor_y + 12, false );
                }

                if( _team[ p_pos ].getSpecies( ) != PKMN_NIDORAN_F &&
                        _team[ p_pos ].getSpecies( ) != PKMN_NIDORAN_M ) {
                    if( _team[ p_pos ].m_boxdata.m_isFemale ) {
                        IO::regularFont->setColor( IO::RED_IDX, 1 );
                        IO::regularFont->setColor( IO::RED2_IDX, 2 );
                        IO::regularFont->printString( "}", anchor_x + 109, anchor_y + 12, false );
                        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                    } else if( !_team[ p_pos ].m_boxdata.m_isGenderless ) {
                        IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                        IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
                        IO::regularFont->printString( "{", anchor_x + 109, anchor_y + 12, false );
                        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                    }
                }

                // HP
                u8 barWidth = 45 * _team[ p_pos ].m_stats.m_curHP / _team[ p_pos ].m_stats.m_maxHP;
                if( _team[ p_pos ].m_stats.m_curHP * 2 >= _team[ p_pos ].m_stats.m_maxHP ) {
                    IO::smallFont->setColor( 240, 2 );
                    IO::smallFont->setColor( 241, 1 );
                    IO::smallFont->setColor( 242, 3 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 31, anchor_x + 69 + barWidth,
                                        anchor_y + 32, false, 241 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 33, anchor_x + 69 + barWidth,
                                        anchor_y + 33, false, 242 );
                } else if( _team[ p_pos ].m_stats.m_curHP * 4 >= _team[ p_pos ].m_stats.m_maxHP ) {
                    IO::smallFont->setColor( 240, 2 );
                    IO::smallFont->setColor( 243, 1 );
                    IO::smallFont->setColor( 244, 3 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 31, anchor_x + 69 + barWidth,
                                        anchor_y + 32, false, 243 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 33, anchor_x + 69 + barWidth,
                                        anchor_y + 33, false, 244 );
                } else {
                    IO::smallFont->setColor( 240, 2 );
                    IO::smallFont->setColor( 245, 1 );
                    IO::smallFont->setColor( 246, 3 );
                    if( _team[ p_pos ].m_stats.m_curHP ) {
                        IO::printRectangle( anchor_x + 69, anchor_y + 31, anchor_x + 69 + barWidth,
                                            anchor_y + 32, false, 245 );
                        IO::printRectangle( anchor_x + 69, anchor_y + 33, anchor_x + 69 + barWidth,
                                            anchor_y + 33, false, 246 );
                    }
                }

                IO::smallFont->printString( GET_STRING( 186 ), anchor_x + 116 - 62, anchor_y + 20,
                                            false ); // HP "icon"
                IO::smallFont->setColor( IO::WHITE_IDX, 1 );
                IO::smallFont->setColor( IO::GRAY_IDX, 2 );

                IO::smallFont->printString( "!", anchor_x + 5, anchor_y + 33, false );

                char buffer[ 10 ];
                snprintf( buffer, 8, "%d", _team[ p_pos ].m_level );

                IO::smallFont->printString( buffer, anchor_x + 14, anchor_y + 32, false );

                snprintf( buffer, 8, "%3d", _team[ p_pos ].m_stats.m_curHP );
                IO::smallFont->printString( buffer, anchor_x + 116 - 32 - 24, anchor_y + 32,
                                            false );
                snprintf( buffer, 8, "/%d", _team[ p_pos ].m_stats.m_maxHP );
                IO::smallFont->printString( buffer, anchor_x + 116 - 32, anchor_y + 32, false );

                // update sprites
                oam[ SPR_HP_BAR_OAM( p_pos ) ].isHidden = false;
                oam[ SPR_ITEM_ICON_OAM( p_pos ) ].isHidden
                    = !_team[ p_pos ].m_boxdata.getItem( ); // item

                oam[ SPR_SHINY_ICON_OAM( p_pos ) ].isHidden
                    = !_team[ p_pos ].isShiny( ); // shiny status

                // other status conditions
                if( !_team[ p_pos ].m_stats.m_curHP ) {
                    IO::loadSprite( SPR_STATUS_ICON_OAM( p_pos ), SPR_STATUS_ICON_PAL,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].gfxIndex,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].y, 8, 8, status_fntPal,
                                    status_fntTiles, status_fntTilesLen / 2, false, false, false,
                                    OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isParalyzed ) {
                    IO::loadSprite( SPR_STATUS_ICON_OAM( p_pos ), SPR_STATUS_ICON_PAL,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].gfxIndex,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].y, 8, 8, status_parPal,
                                    status_parTiles, status_parTilesLen / 2, false, false, false,
                                    OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isAsleep ) {
                    IO::loadSprite( SPR_STATUS_ICON_OAM( p_pos ), SPR_STATUS_ICON_PAL,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].gfxIndex,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].y, 8, 8, status_slpPal,
                                    status_slpTiles, status_slpTilesLen / 2, false, false, false,
                                    OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isBadlyPoisoned ) {
                    IO::loadSprite( SPR_STATUS_ICON_OAM( p_pos ), SPR_STATUS_ICON_PAL,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].gfxIndex,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].y, 8, 8, status_txcPal,
                                    status_txcTiles, status_txcTilesLen / 2, false, false, false,
                                    OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isBurned ) {
                    IO::loadSprite( SPR_STATUS_ICON_OAM( p_pos ), SPR_STATUS_ICON_PAL,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].gfxIndex,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].y, 8, 8, status_brnPal,
                                    status_brnTiles, status_brnTilesLen / 2, false, false, false,
                                    OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isFrozen ) {
                    IO::loadSprite( SPR_STATUS_ICON_OAM( p_pos ), SPR_STATUS_ICON_PAL,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].gfxIndex,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].y, 8, 8, status_frzPal,
                                    status_frzTiles, status_frzTilesLen / 2, false, false, false,
                                    OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isPoisoned ) {
                    IO::loadSprite( SPR_STATUS_ICON_OAM( p_pos ), SPR_STATUS_ICON_PAL,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].gfxIndex,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_ICON_OAM( p_pos ) ].y, 8, 8, status_psnPal,
                                    status_psnTiles, status_psnTilesLen / 2, false, false, false,
                                    OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
                }
                IO::updateOAM( p_bottom ); // Shipout fast stuff first
                IO::loadPKMNIcon(
                    _team[ p_pos ].m_boxdata.m_speciesId, oam[ SPR_PKMN_ICON_OAM( p_pos ) ].x,
                    oam[ SPR_PKMN_ICON_OAM( p_pos ) ].y, SPR_PKMN_ICON_OAM( p_pos ),
                    SPR_PKMN_ICON_PAL( p_pos ), oam[ SPR_PKMN_ICON_OAM( p_pos ) ].gfxIndex,
                    p_bottom, _team[ p_pos ].getForme( ), _team[ p_pos ].isShiny( ),
                    _team[ p_pos ].isFemale( ) );
            }
        }

        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::drawPartyPkmnSub( u8 p_pos, bool p_selected, bool p_redraw,
                                          const char* p_message, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        if( p_redraw ) {
            oam[ SPR_ITEM_ICON_OAM_SUB( p_pos ) ].isHidden = true; // item
            oam[ SPR_PKMN_BG_OAM_SUB( p_pos ) ].isHidden   = true; // pkmn
        }

        if( p_pos >= _teamLength ) {
            // No Pkmn -> draw empty box
            SpriteEntry old = oam[ SPR_PKMN_BG_OAM_SUB( p_pos ) ];
            IO::loadSprite( SPR_PKMN_BG_OAM_SUB( p_pos ), SPR_WINDOW_PAL_SUB, old.gfxIndex, old.x,
                            old.y, 32, 32, 0, noselection_blank_32_32Tiles,
                            noselection_blank_32_32TilesLen, false, false, false, OBJPRIORITY_3,
                            p_bottom, OBJMODE_BLENDED );
            IO::updateOAM( p_bottom );
            return;
        }
        if( _team[ p_pos ].m_stats.m_curHP ) {
            // Pkmn is not fainted
            if( p_selected ) {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM_SUB( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM_SUB( p_pos ), SPR_WINDOW_PAL_SUB, old.gfxIndex,
                                old.x, old.y, 32, 32, 0, selection_32_32Tiles,
                                noselection_blank_32_32TilesLen, false, false, false, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            } else {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM_SUB( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM_SUB( p_pos ), SPR_WINDOW_PAL_SUB, old.gfxIndex,
                                old.x, old.y, 32, 32, 0, noselection_32_32Tiles,
                                noselection_blank_32_32TilesLen, false, false, false, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            }
        } else {
            // Pkmn is fainted
            if( p_selected ) {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM_SUB( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM_SUB( p_pos ), SPR_WINDOW_PAL_SUB, old.gfxIndex,
                                old.x, old.y, 32, 32, 0, selection_faint_32_32Tiles,
                                noselection_blank_32_32TilesLen, false, false, false, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            } else {
                SpriteEntry old = oam[ SPR_PKMN_BG_OAM_SUB( p_pos ) ];
                IO::loadSprite( SPR_PKMN_BG_OAM_SUB( p_pos ), SPR_WINDOW_PAL_SUB, old.gfxIndex,
                                old.x, old.y, 32, 32, 0, noselection_faint_32_32Tiles,
                                noselection_blank_32_32TilesLen, false, false, false, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            }
        }

        if( _team[ p_pos ].isEgg( ) ) {
            if( p_redraw ) {
                // general data
                IO::loadEggIcon( oam[ SPR_PKMN_ICON_OAM_SUB( p_pos ) ].x,
                                 oam[ SPR_PKMN_ICON_OAM_SUB( p_pos ) ].y,
                                 SPR_PKMN_ICON_OAM_SUB( p_pos ), SPR_PKMN_ICON_PAL_SUB( p_pos ),
                                 oam[ SPR_PKMN_ICON_OAM_SUB( p_pos ) ].gfxIndex, p_bottom,
                                 _team[ p_pos ].getSpecies( ) == PKMN_MANAPHY );
            }
        } else {
            if( p_redraw ) {
                // general data

                // update sprites
                oam[ SPR_ITEM_ICON_OAM_SUB( p_pos ) ].isHidden
                    = !_team[ p_pos ].m_boxdata.getItem( ); // item

                IO::updateOAM( p_bottom ); // Shipout fast stuff first
                IO::loadPKMNIcon(
                    _team[ p_pos ].m_boxdata.m_speciesId, oam[ SPR_PKMN_ICON_OAM_SUB( p_pos ) ].x,
                    oam[ SPR_PKMN_ICON_OAM_SUB( p_pos ) ].y, SPR_PKMN_ICON_OAM_SUB( p_pos ),
                    SPR_PKMN_ICON_PAL_SUB( p_pos ), oam[ SPR_PKMN_ICON_OAM_SUB( p_pos ) ].gfxIndex,
                    p_bottom, _team[ p_pos ].getForme( ), _team[ p_pos ].isShiny( ),
                    _team[ p_pos ].isFemale( ) );
            }
        }

        if( p_selected ) {
            if( p_bottom ) {
                dmaCopy( partysubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
            } else {
                dmaCopy( partysubBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 256 );
            }

            if( p_message ) {
                IO::regularFont->printString( p_message, 32, 2, p_bottom );
            } else {
                if( !_swapping ) {
                    if( !_toSelect ) {
                        snprintf( BUFFER, 49, GET_STRING( 57 ),
                                  _team[ p_pos ].isEgg( ) ? GET_STRING( 34 )
                                                          : _team[ p_pos ].m_boxdata.m_name );
                    } else {
                        if( _toSelect > 1 ) {
                            snprintf( BUFFER, 49, GET_STRING( 332 ), _toSelect );
                        } else {
                            snprintf( BUFFER, 49, GET_STRING( 333 ) );
                        }
                    }
                } else {
                    snprintf( BUFFER, 49, GET_STRING( 166 ) );
                }
                IO::regularFont->printString( BUFFER, 32, 2, p_bottom );
            }
        }

        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::drawPartyPkmnChoice( u8 p_selectedPkmn, const u16 p_choices[],
                                             u8 p_choiceCnt, bool p_nextButton, bool p_prevButton,
                                             u8 p_selectedChoice, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        // show choice bgs
        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = i >= p_choiceCnt;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( i == p_selectedChoice ? SPR_WINDOW_PAL_SUB : SPR_BOX_PAL_SUB );
            }
            if( i >= p_choiceCnt || !p_choices ) { continue; }

            if( p_choices[ i ] & CHOICE_FIELD_MOVE ) {
                u16 move = _team[ p_selectedPkmn ].m_boxdata.m_moves[ p_choices[ i ] & 3 ];

                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                IO::regularFont->setColor( IO::BLUE2_IDX, 2 );

                IO::regularFont->printString(
                    MOVE::getMoveName( move ).c_str( ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48,
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, p_bottom, IO::font::CENTER );

                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            } else {
                IO::regularFont->printString(
                    GET_STRING( p_choices[ i ] ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48,
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, p_bottom, IO::font::CENTER );
            }
        }
        oam[ SPR_PAGE_LEFT_OAM_SUB ].isHidden   = !p_prevButton;
        oam[ SPR_ARROW_LEFT_OAM_SUB ].isHidden  = !p_prevButton;
        oam[ SPR_PAGE_RIGHT_OAM_SUB ].isHidden  = !p_nextButton;
        oam[ SPR_ARROW_RIGHT_OAM_SUB ].isHidden = !p_nextButton;
        IO::updateOAM( p_bottom );

        if( _needsInit ) {
            _needsInit = false;
            dmaCopy( partybgPal, BG_PALETTE, 3 * 2 );
            dmaCopy( partybgPal, BG_PALETTE_SUB, 3 * 2 );
            dmaCopy( partysubPal + 3, BG_PALETTE_SUB + 3, 8 * 2 );

            IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );

            dmaCopy( partybgPal, BG_PALETTE, 3 * 2 );
            dmaCopy( partybgPal, BG_PALETTE_SUB, 3 * 2 );
            dmaCopy( partysubPal + 3, BG_PALETTE_SUB + 3, 8 * 2 );

            for( u8 i = 0; i < 2; ++i ) {
                u16* pal             = IO::BG_PAL( i );
                pal[ IO::WHITE_IDX ] = IO::WHITE;
                pal[ IO::GRAY_IDX ]  = IO::GRAY;
                pal[ IO::BLACK_IDX ] = IO::BLACK;
                pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
                pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
                pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
                pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );

                pal[ 240 ] = IO::RGB( 6, 6, 6 );    // hp bar border color
                pal[ 241 ] = IO::RGB( 12, 30, 12 ); // hp bar green 1
                pal[ 242 ] = IO::RGB( 3, 23, 4 );   // hp bar green 2
                pal[ 243 ] = IO::RGB( 30, 30, 12 ); // hp bar yellow 1
                pal[ 244 ] = IO::RGB( 23, 23, 5 );  // hp bar yellow 2
                pal[ 245 ] = IO::RGB( 30, 15, 12 ); // hp bar red 1
                pal[ 246 ] = IO::RGB( 20, 7, 7 );   // hp bar red 2
            }

            bgSetScale( IO::bg3sub, 1 << 7, 1 << 7 );
            bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
            bgSetScroll( IO::bg3sub, 0, 0 );
            bgSetScroll( IO::bg3, 0, 0 );
            dmaCopy( partybg2Bitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x06 << 8 );
            REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
            bgUpdate( );
        }
    }

    std::pair<u16, u16> partyScreenUI::getChoiceAnchorPosition( u8 p_choiceIdx, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          x   = oam[ SPR_CHOICE_START_OAM_SUB( p_choiceIdx ) ].x;
        u16          y   = oam[ SPR_CHOICE_START_OAM_SUB( p_choiceIdx ) ].y;

        return std::pair<u16, u16>( x, y );
    }

    void partyScreenUI::animatePartyPkmn( u8 p_frame, bool p_bottom ) {
        SpriteEntry* oam    = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        SpriteEntry* oamSub = ( !p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        if( ( p_frame & 7 ) != 7 ) {
            oam[ SPR_PKMN_ICON_OAM( _selectedIdx ) ].y
                = partyTopScreenPkmnIconPosY( _selectedIdx ) + ( ( p_frame >> 2 ) & 1 ) * 4 - 2;
            oamSub[ SPR_PKMN_ICON_OAM_SUB( _selectedIdx ) ].y
                = 152 + ( ( p_frame >> 2 ) & 1 ) * 4 - 2;
        } else {
            for( u8 i = 0; i < 6; i++ ) {
                oam[ SPR_PKMN_ICON_OAM( i ) ].y
                    = partyTopScreenPkmnIconPosY( i ) + ( ( p_frame >> 3 ) & 1 ) * 2 - 1;
                oamSub[ SPR_PKMN_ICON_OAM_SUB( i ) ].y = 153 + ( ( p_frame >> 3 ) & 1 ) * 2 - 1;
            }
        }

        IO::updateOAM( true );
        IO::updateOAM( false );
    }

    void partyScreenUI::animateMessageBox( u8 p_frame, bool p_bottom ) {
        if( ( p_frame & 31 ) == 15 ) { IO::regularFont->drawContinue( 216, 104, p_bottom ); }
        if( ( p_frame & 31 ) == 31 ) { IO::regularFont->hideContinue( 216, 104, 0, p_bottom ); }
    }

#ifdef DESQUID
    void partyScreenUI::showDesquidWindow( bool p_bottom ) {
        (void) p_bottom;
        // SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        // for( u8 i = 0; i < 7; i++ ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }
        // IO::updateOAM( p_bottom );
    }

    void partyScreenUI::hideDesquidWindow( bool p_bottom ) {
        (void) p_bottom;
        // SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        // for( u8 i = 0; i < 7; i++ ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = true; }
        // IO::updateOAM( p_bottom );
    }

    void partyScreenUI::drawDesquidItem( u8 p_idx, const char* p_string, u32 p_value,
                                         u32 p_maxValue, bool p_highlight, u8 p_highlightDigit,
                                         bool p_bottom ) {
        SpriteEntry* oam    = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        const u8     height = 18;

        IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, 32 + p_idx * height,
                            256 - oam[ SPR_MSG_BOX_OAM_SUB ].x - 1, 31 + ( p_idx + 1 ) * height,
                            p_bottom, 0 );
        if( p_highlight ) {
            IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, 32 + p_idx * height,
                                oam[ SPR_MSG_BOX_OAM_SUB ].x, 31 + ( p_idx + 1 ) * height, p_bottom,
                                IO::RED_IDX );
            IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, 32 + p_idx * height,
                                256 - oam[ SPR_MSG_BOX_OAM_SUB ].x - 1, 32 + p_idx * height,
                                p_bottom, IO::RED_IDX );
            IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, 31 + ( p_idx + 1 ) * height,
                                256 - oam[ SPR_MSG_BOX_OAM_SUB ].x - 1, 31 + ( p_idx + 1 ) * height,
                                p_bottom, IO::RED_IDX );
            IO::printRectangle( 256 - oam[ SPR_MSG_BOX_OAM_SUB ].x - 1, 32 + p_idx * height,
                                256 - oam[ SPR_MSG_BOX_OAM_SUB ].x - 1, 31 + ( p_idx + 1 ) * height,
                                p_bottom, IO::RED_IDX );
        }

        IO::regularFont->printString( p_string, oam[ SPR_MSG_BOX_OAM_SUB ].x + 8,
                                      33 + p_idx * height, p_bottom );

        u8 dg = 0;
        for( u32 tmp = p_maxValue; tmp > 0; ++dg, tmp /= 10 )
            ;

        IO::regularFont->printCounter(
            p_value, dg, 256 - oam[ SPR_MSG_BOX_OAM_SUB ].x - ( dg + 1 ) * 8, 33 + p_idx * height,
            p_highlightDigit, IO::WHITE_IDX, IO::BLACK_IDX, p_bottom );
    }
#endif

    void partyScreenUI::init( u8 p_initialSelection ) {
        _selectedIdx = p_initialSelection;

        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::vramSetup( true );
        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );

        initTopScreen( );
        initBottomScreen( );

        for( u8 i = 0; i < 6; i++ ) { drawPartyPkmn( i, i == _selectedIdx ); }
        _needsInit = true;
    }

    void partyScreenUI::animate( u8 p_frame ) {
        animatePartyPkmn( p_frame );
        IO::animateBG( p_frame, IO::bg3 );
        IO::animateBG( p_frame, IO::bg3sub );

        if( _animateMsg ) { animateMessageBox( p_frame ); }

        bgUpdate( );
    }

    void partyScreenUI::select( u8 p_selectedIdx, const char* p_message ) {
        if( _selectedIdx != p_selectedIdx ) {
            drawPartyPkmn( _selectedIdx, false, false, p_message );
            _selectedIdx = p_selectedIdx;
            drawPartyPkmn( _selectedIdx, true, false, p_message );
        } else {
            drawPartyPkmn( _selectedIdx, true, true, p_message );
        }
    }

    void partyScreenUI::mark( u8 p_markIdx, u8 p_color, bool p_bottom ) {
        // Assumes that pkmn doesn't already have a mark
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        if( p_color == SWAP_COLOR ) {
            _swapping = true;

            oam[ SPR_SWAP_OAM( p_markIdx ) ].isHidden     = false;
            oam[ SPR_SWAP_OAM( p_markIdx ) + 1 ].isHidden = false;
            oam[ SPR_SWAP_OAM( p_markIdx ) + 2 ].isHidden = false;
        } else {
            if( oam[ SPR_MARK_OAM( p_markIdx, p_color - 1 ) ].isHidden ) {
                --_toSelect;
                oam[ SPR_MARK_OAM( p_markIdx, p_color - 1 ) ].isHidden = false;
            }
        }
        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::unmark( u8 p_markIdx, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        for( u8 i = 0; i < 6; ++i ) {
            if( !oam[ SPR_MARK_OAM( p_markIdx, i ) ].isHidden ) {
                oam[ SPR_MARK_OAM( p_markIdx, i ) ].isHidden = true;
                ++_toSelect;
                break;
            }
        }
        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::unswap( u8 p_markIdx, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        if( !oam[ SPR_SWAP_OAM( p_markIdx ) ].isHidden ) { _swapping = false; }
        oam[ SPR_SWAP_OAM( p_markIdx ) ].isHidden     = true;
        oam[ SPR_SWAP_OAM( p_markIdx ) + 1 ].isHidden = true;
        oam[ SPR_SWAP_OAM( p_markIdx ) + 2 ].isHidden = true;

        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::swap( u8 p_idx1, u8 p_idx2, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        REG_BLDCNT       = BLEND_ALPHA;
        REG_BLDCNT_SUB   = BLEND_ALPHA;
//        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        for( u8 i = 0; i < 6; ++i ) {
            bool tmp                                  = oam[ SPR_MARK_OAM( p_idx1, i ) ].isHidden;
            oam[ SPR_MARK_OAM( p_idx1, i ) ].isHidden = oam[ SPR_MARK_OAM( p_idx2, i ) ].isHidden;
            oam[ SPR_MARK_OAM( p_idx2, i ) ].isHidden = tmp;
        }
        drawPartyPkmn( p_idx1, p_idx1 == _selectedIdx, true, 0, p_bottom );
        drawPartyPkmn( p_idx2, p_idx2 == _selectedIdx, true, 0, p_bottom );
        unswap( p_idx1, p_bottom );
        unswap( p_idx2, p_bottom );
//        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x06 << 8 );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        bgUpdate( );
    }

    void partyScreenUI::printMessage( const char* p_message, u16 p_itemIcon, bool p_bottom ) {
        _animateMsg      = true;
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        for( u8 i = 0; i < 7; i++ ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }
        if( p_itemIcon ) {
            oam[ SPR_ITEM_OAM_SUB ].isHidden = false;
            IO::loadItemIcon( p_itemIcon, oam[ SPR_ITEM_OAM_SUB ].x, oam[ SPR_ITEM_OAM_SUB ].y,
                              SPR_ITEM_OAM_SUB, SPR_ITEM_PAL_SUB, oam[ SPR_ITEM_OAM_SUB ].gfxIndex,
                              p_bottom );
            if( p_message ) {
                IO::regularFont->printString( p_message, oam[ SPR_ITEM_OAM_SUB ].x + 36,
                                              oam[ SPR_ITEM_OAM_SUB ].y, p_bottom, IO::font::LEFT );
            }
        } else if( p_message ) {
            IO::regularFont->printString( p_message, 128, oam[ SPR_ITEM_OAM_SUB ].y, p_bottom,
                                          IO::font::CENTER );
        }

        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::hideMessageBox( bool p_bottom ) {
        _animateMsg      = false;
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        for( u8 i = 0; i < 7; i++ ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = true; }
        oam[ SPR_ITEM_OAM_SUB ].isHidden = true;
        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::printYNMessage( const char* p_message, u8 p_selection, u16 p_itemIcon,
                                        bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        for( u8 i = 0; i < 7; i++ ) {
            if( p_message ) {
                oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false;
                oam[ SPR_MSG_BOX_OAM_SUB + i ].y -= 18;
            }
        }
        if( p_itemIcon ) {
            oam[ SPR_ITEM_OAM_SUB ].isHidden = false;
            IO::loadItemIcon( p_itemIcon, oam[ SPR_ITEM_OAM_SUB ].x, oam[ SPR_ITEM_OAM_SUB ].y,
                              SPR_ITEM_OAM_SUB, SPR_ITEM_PAL_SUB, oam[ SPR_ITEM_OAM_SUB ].gfxIndex,
                              p_bottom );
            if( p_message ) {
                IO::regularFont->printString( p_message, oam[ SPR_ITEM_OAM_SUB ].x + 36,
                                              oam[ SPR_ITEM_OAM_SUB ].y - 18, p_bottom,
                                              IO::font::LEFT );
            }
        } else if( p_message ) {
            IO::regularFont->printString( p_message, 128, oam[ SPR_ITEM_OAM_SUB ].y - 18, p_bottom,
                                          IO::font::CENTER );
        }

        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( ( i & 1 ) == ( p_selection & 1 ) ) ? SPR_WINDOW_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }
        if( p_message || p_selection >= 254 ) {
            u8 shift = 0;
            if( !p_message && p_selection == 254 ) {
                // Use the middle choice boxes
                shift = 2;
            }

            for( u8 i = 4 - shift; i < 6 - shift; i++ ) {
                for( u8 j = 0; j < 6; j++ ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                }
            }

            IO::regularFont->printString(
                GET_STRING( 80 ), oam[ SPR_CHOICE_START_OAM_SUB( 4 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 4 - shift ) ].y + 8, p_bottom, IO::font::CENTER );
            IO::regularFont->printString(
                GET_STRING( 81 ), oam[ SPR_CHOICE_START_OAM_SUB( 5 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 5 - shift ) ].y + 8, p_bottom, IO::font::CENTER );
        }
        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::hideYNMessageBox( bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        for( u8 i = 0; i < 7; i++ ) {
            oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = true;
            oam[ SPR_MSG_BOX_OAM_SUB + i ].y += 18;
        }
        oam[ SPR_ITEM_OAM_SUB ].isHidden = true;
        for( u8 i = 4; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
            }
        }
        IO::updateOAM( p_bottom );
    }

} // namespace STS

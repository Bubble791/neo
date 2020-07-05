/*
Pokémon neo
------------------------------

file        : nav.cpp
author      : Philip Wellnitz
description :

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

#include <map>

#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "keyboard.h"
#include "nav.h"
#include "screenFade.h"
#include "sound.h"
#include "sprite.h"
#include "uio.h"
#include "yesNoBox.h"

#include "berry.h"
#include "item.h"
#include "itemNames.h"
#include "moveNames.h"
#include "pokemon.h"

#include "bagUI.h"
#include "bagViewer.h"

#include "partyScreen.h"
#include "statusScreen.h"

#include "boxUI.h"
#include "boxViewer.h"

#include "dex.h"
#include "dexUI.h"

#include "mapDrawer.h"
#include "mapObject.h"
#include "mapSlice.h"

#include "battle.h"
#include "battleTrainer.h"

#include "Border.h"

#include "noselection_96_32_1.h"
#include "noselection_96_32_2.h"
#include "noselection_96_32_4.h"
#include "x_16_16.h"

namespace NAV {
#define SPR_MSGTEXT_OAM 108
#define SPR_MSGCONT_OAM 112
#define SPR_MSGBOX_OAM 113

#define SPR_MSGBOX_GFX 348
#define SPR_MSG_GFX 380
#define SPR_MSGCONT_GFX 508

#define SPR_MENU_OAM_SUB( p_idx ) ( 0 + ( p_idx ) )
#define SPR_MENU_SEL_OAM_SUB 6
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 7 + 6 * ( p_pos ) )
#define SPR_X_OAM_SUB 44

#define SPR_MENU_PAL_SUB( p_idx ) ( 0 + ( p_idx ) )
#define SPR_MENU_SEL_PAL_SUB 6
#define SPR_BOX_PAL_SUB 7
#define SPR_BOX_SEL_PAL_SUB 8
#define SPR_X_PAL_SUB 9

    u16         TEXT_BUF[ 32 * 256 ] = { 0 };
    u16         CONT_BUF[ 16 * 16 ]  = { 0 };
    u16         TEXT_PAL[ 16 ]       = { 0, IO::BLACK, IO::GRAY, IO::WHITE };
    std::string TEXT_CACHE           = "";

    void hideMessageBox( ) {
        for( u8 i = 0; i < 14; ++i ) {
            IO::OamTop->oamBuffer[ SPR_MSGBOX_OAM + i ].isHidden = true;
        }
        IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + i ].isHidden = true;
        }
        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        TEXT_CACHE = "";
        IO::updateOAM( false );
    }

    void init( bool p_bottom ) {
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::clearScreen( p_bottom, false, true );
        IO::initOAMTable( p_bottom );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        auto ptr  = !p_bottom ? bgGetGfxPtr( IO::bg2 ) : bgGetGfxPtr( IO::bg2sub );
        auto ptr3 = !p_bottom ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        auto pal  = !p_bottom ? BG_PALETTE : BG_PALETTE_SUB;

        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        bgUpdate( );

        FS::readPictureData(
            ptr3, "nitro:/PICS/NAV/",
            std::to_string( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx ).c_str( ), 192 * 2,
            192 * 256, p_bottom );

        dmaCopy( BorderBitmap, ptr, 256 * 192 );
        dmaCopy( BorderPal + 192, pal + 192, 64 );

        u16 tileCnt = 0;

        // Main menu icons
        tileCnt = IO::loadSprite( "MM/party", SPR_MENU_OAM_SUB( 0 ), SPR_MENU_PAL_SUB( 0 ), tileCnt,
                                  256 - 29, 192 - 6 * 29, 32, 32, false, false,
                                  !SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), OBJPRIORITY_2,
                                  p_bottom );
        tileCnt = IO::loadSprite( "MM/dex", SPR_MENU_OAM_SUB( 1 ), SPR_MENU_PAL_SUB( 1 ), tileCnt,
                                  256 - 29, 192 - 5 * 29, 32, 32, false, false,
                                  !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_DEX_OBTAINED ),
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite( "MM/bag", SPR_MENU_OAM_SUB( 2 ), SPR_MENU_PAL_SUB( 2 ), tileCnt,
                                  256 - 29, 192 - 4 * 29, 32, 32, false, false, false,
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite( "MM/id", SPR_MENU_OAM_SUB( 3 ), SPR_MENU_PAL_SUB( 3 ), tileCnt,
                                  256 - 29, 192 - 3 * 29, 32, 32, false, false, false,
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite( "MM/save", SPR_MENU_OAM_SUB( 4 ), SPR_MENU_PAL_SUB( 4 ), tileCnt,
                                  256 - 29, 192 - 2 * 29, 32, 32, false, false, false,
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite( "MM/settings", SPR_MENU_OAM_SUB( 5 ), SPR_MENU_PAL_SUB( 5 ),
                                  tileCnt, 256 - 29, 192 - 1 * 29, 32, 32, false, false, false,
                                  OBJPRIORITY_2, p_bottom );

        tileCnt = IO::loadSprite( "MM/select", SPR_MENU_SEL_OAM_SUB, SPR_MENU_SEL_PAL_SUB, tileCnt,
                                  256 - 31, 192 - 1 * 31, 32, 32, false, false, true, OBJPRIORITY_2,
                                  p_bottom );

        // x
        tileCnt = IO::loadSprite( SPR_X_OAM_SUB, SPR_X_PAL_SUB, tileCnt, 236, 172, 16, 16,
                                  x_16_16Pal, x_16_16Tiles, x_16_16TilesLen, false, false, true,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        // Choice boxes

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i;

            if( !i ) {
                tileCnt
                    = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB, tileCnt, 29,
                                      42 + i * 36, 16, 32, noselection_96_32_1Pal,
                                      noselection_96_32_1Tiles, noselection_96_32_1TilesLen, false,
                                      false, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                tileCnt
                    = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB, tileCnt,
                                      29 + 16, 42 + i * 36, 16, 32, noselection_96_32_2Pal,
                                      noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false,
                                      false, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            } else {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29, 42 + i * 36, 16,
                                32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                                noselection_96_32_1TilesLen, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 16,
                                42 + i * 36, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            }
            for( u8 j = 2; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 16,
                                42 + i * 36, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29 + 5 * 16, 42 + i * 36,
                            16, 32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                            noselection_96_32_1TilesLen, true, true, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_BLENDED );
        }

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, 42 + i * 36, 16, 32,
                            noselection_96_32_1Pal, noselection_96_32_1Tiles,
                            noselection_96_32_1TilesLen, false, false, true, OBJPRIORITY_3,
                            p_bottom, OBJMODE_BLENDED );
            for( u8 j = 1; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 16,
                                42 + i * 36, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131 + 5 * 16,
                            42 + i * 36, 16, 32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                            noselection_96_32_1TilesLen, true, true, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_BLENDED );
        }

        IO::copySpritePal( noselection_96_32_4Pal, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );
        IO::updateOAM( p_bottom );
        hideMessageBox( );
    }

    void _printMessage( const char* p_message ) {
        printMessage( p_message );
    }

    void doPrintMessage( const char* p_message, style p_style ) {
        u16 x = 12, y = 192 - 40;
        if( p_message ) {
            if( p_style == MSG_NORMAL || p_style == MSG_NOCLOSE ) {
                IO::loadSpriteB( "UI/mbox1", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 0, 192 - 46, 32, 64,
                                 false, false, false, OBJPRIORITY_0, false );

                for( u8 i = 0; i < 13; ++i ) {
                    IO::loadSpriteB( SPR_MSGBOX_OAM + 13 - i, SPR_MSGBOX_GFX, 32 + 16 * i, 192 - 46,
                                     32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
                }
                IO::regularFont->setColor( 1, 1 );
                IO::regularFont->setColor( 2, 2 );
            } else if( p_style == MSG_INFO || p_style == MSG_INFO_NOCLOSE ) {
                IO::loadSpriteB( "UI/mbox2", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 2, 192 - 46, 32, 64,
                                 false, false, false, OBJPRIORITY_0, false );

                for( u8 i = 0; i < 13; ++i ) {
                    IO::loadSpriteB( SPR_MSGBOX_OAM + 13 - i, SPR_MSGBOX_GFX, 30 + 16 * i, 192 - 46,
                                     32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
                }

                IO::regularFont->setColor( 3, 1 );
                IO::regularFont->setColor( 2, 2 );
            }
        }

        if( !p_message ) {
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            TEXT_CACHE = "";
        } else {
            TEXT_CACHE = TEXT_CACHE + p_message;
            // TODO: Auto rotate msgbox text.
            /* auto ln = */ IO::regularFont->printStringBC( TEXT_CACHE.c_str( ), TEXT_PAL, TEXT_BUF,
                                                            256, IO::font::LEFT, 16 );
            u16 tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM, SPR_MSG_GFX, x, y, 64, 32, TEXT_BUF,
                                           64 * 32 / 2, false, false, false, OBJPRIORITY_0, false );
            tileCnt     = IO::loadSpriteB( SPR_MSGTEXT_OAM + 1, tileCnt, x + 64, y, 64, 32,
                                       TEXT_BUF + 64 * 32, 64 * 32 / 2, false, false, false,
                                       OBJPRIORITY_0, false );
            tileCnt     = IO::loadSpriteB( SPR_MSGTEXT_OAM + 2, tileCnt, x + 128, y, 64, 32,
                                       TEXT_BUF + 2 * 64 * 32, 64 * 32 / 2, false, false, false,
                                       OBJPRIORITY_0, false );
            tileCnt     = IO::loadSpriteB( SPR_MSGTEXT_OAM + 3, tileCnt, x + 64 + 128, y, 64, 32,
                                       TEXT_BUF + 3 * 64 * 32, 64 * 32 / 2, false, false, false,
                                       OBJPRIORITY_0, false );

            if( p_style == MSG_NORMAL || p_style == MSG_INFO ) {
                // "Continue" char
                IO::regularFont->printCharB( 172, TEXT_PAL, CONT_BUF, 16, 0, 0 );
                tileCnt = IO::loadSpriteB( SPR_MSGCONT_OAM, SPR_MSGCONT_GFX, 254 - x, y + 24, 16,
                                           16, CONT_BUF, 16 * 16 / 2, false, false, false,
                                           OBJPRIORITY_0, false );
            } else {
                IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;
            }
        }

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::updateOAM( false );
    }

    void animateMB( u8 p_frame ) {
        if( ( p_frame & 15 ) == 0 ) {
            SpriteEntry* oam                = IO::OamTop->oamBuffer;
            oam[ SPR_MSGCONT_OAM ].isHidden = !oam[ SPR_MSGCONT_OAM ].isHidden;
            IO::updateOAM( false );
        }
    }

    void waitForInteract( ) {
        cooldown = COOLDOWN_COUNT;
        u8 frame = 0;
        loop( ) {
            animateMB( ++frame );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_A ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    animateMB( ++frame );
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                break;
            }
        }
    }

    void printMessage( const char* p_message, style p_style ) {
        doPrintMessage( p_message, p_style );

        if( p_style == MSG_NORMAL || p_style == MSG_INFO ) {
            waitForInteract( );
            hideMessageBox( );
        }
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const char* p_message, style p_style, u8 p_selection ) {
        doPrintMessage( p_message, p_style );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>> res
            = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( ( i & 1 ) == ( p_selection & 1 ) ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }
        for( u8 i = 2; i < 4; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
            }
        }

        if( p_message || p_selection >= 254 ) {

            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152,
                                 true );
            for( u8 i = 0; i < 7; ++i ) { oam[ SPR_MENU_OAM_SUB( i ) ].isHidden = true; }

            IO::regularFont->printString(
                GET_STRING( 80 ), oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 32 ),
                           IO::yesNoBox::YES ) );

            IO::regularFont->printString(
                GET_STRING( 81 ), oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 32 ),
                           IO::yesNoBox::NO ) );
        }

        IO::updateOAM( true );
        return res;
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    printChoiceMessage( const char* p_message, style p_style, const std::vector<u16>& p_choices,
                        u8 p_selection ) {
        doPrintMessage( p_message, p_style );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( i == p_selection ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        if( p_message || p_selection >= 254 ) {

            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152,
                                 true );
            for( u8 i = 0; i < 7; ++i ) { oam[ SPR_MENU_OAM_SUB( i ) ].isHidden = true; }

            for( u8 i = 0; i < p_choices.size( ); i++ ) {
                for( u8 j = 0; j < 6; j++ ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                }

                IO::regularFont->printString(
                    GET_STRING( p_choices[ i ] ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48,
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true, IO::font::CENTER );

                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               i ) );
            }
        }

        IO::updateOAM( true );

        return res;
    }

    std::vector<std::pair<IO::inputTarget, menuOption>> getTouchPositions( bool p_bottom = true ) {
        auto         res = std::vector<std::pair<IO::inputTarget, menuOption>>( );
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        for( u8 i = 0; i < 6; ++i ) {
            if( !oam[ SPR_MENU_OAM_SUB( i ) ].isHidden ) {
                res.push_back( std::pair( IO::inputTarget( oam[ SPR_MENU_OAM_SUB( i ) ].x,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].y,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].x + 27,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].y + 27 ),
                                          menuOption( i ) ) );
            }
        }

        return res;
    }

    void drawBadges( u8 p_page ) {
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 20 );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::GRAY_IDX, 1 );
        IO::regularFont->setColor( 0, 2 );
        u16 tileCnt = 0;
        // x
        tileCnt = IO::loadSprite( 9, 15, tileCnt, 236, 172, 16, 16, x_16_16Pal, x_16_16Tiles,
                                  x_16_16TilesLen, false, false, false, OBJPRIORITY_1, true,
                                  OBJMODE_NORMAL );

        for( u8 i = 0; i < 8; ++i ) { IO::Oam->oamBuffer[ i ].isHidden = true; }

        switch( p_page ) {
        case 0: { // Hoenn badges
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "bc1", 480, 49152,
                                 true );
            BG_PALETTE_SUB[ IO::GRAY_IDX ] = RGB15( 24, 24, 24 );
            IO::regularFont->printStringC( GET_STRING( 434 ), 2, 0, true );

            constexpr u16 spos[ 9 ][ 2 ] = { { 8, 19 }, { 67, 19 }, { 128, 19 }, { 182, 19 },
                                             { 9, 89 }, { 68, 90 }, { 128, 90 }, { 182, 90 } };

            for( u8 i = 0; i < 8; ++i ) {
                if( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << i ) ) {
                    tileCnt = IO::loadSprite( ( "ba/b" + std::to_string( i + 1 ) ).c_str( ), i, i,
                                              tileCnt, spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64,
                                              false, false, false, OBJPRIORITY_0, true );
                }
            }

            break;
        }
        case 1: { // battle frontier
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "bc2", 480, 49152,
                                 true );
            BG_PALETTE_SUB[ IO::GRAY_IDX ] = RGB15( 24, 24, 24 );
            IO::regularFont->printStringC( GET_STRING( 435 ), 2, 0, true );

            constexpr u16 spos[ 7 ][ 2 ] = { { 6, 18 },  { 66, 18 }, { 126, 18 }, { 186, 18 },
                                             { 36, 88 }, { 96, 88 }, { 156, 88 } };

            for( u8 i = 0; i < 7; ++i ) {
                if( SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges & ( 1 << ( 7 + i ) ) ) {
                    tileCnt = IO::loadSprite( ( "ba/s" + std::to_string( i + 1 ) + "2" ).c_str( ),
                                              i, i, tileCnt, spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64,
                                              false, false, false, OBJPRIORITY_0, true );

                } else if( SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges & ( 1 << i ) ) {
                    tileCnt = IO::loadSprite( ( "ba/s" + std::to_string( i + 1 ) + "1" ).c_str( ),
                                              i, i, tileCnt, spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64,
                                              false, false, false, OBJPRIORITY_0, true );
                }
            }

            break;
        }
        default:
            return;
        }

        IO::updateOAM( true );
    }

    void runIDViewer( ) {
        IO::vramSetup( );
        IO::clearScreen( true, true, true );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );

        SAVE::SAV.getActiveFile( ).drawTrainersCard( false );

        u8 currentPage = 0;

        drawBadges( currentPage );

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown = COOLDOWN_COUNT;
                return;
            }

            if( GET_KEY_COOLDOWN( KEY_RIGHT ) || GET_KEY_COOLDOWN( KEY_R ) ) { // next badge case
                if( SAVE::SAV.getActiveFile( ).hasBadgeCase( currentPage + 1 ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    drawBadges( ++currentPage );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( currentPage
                       && ( GET_KEY_COOLDOWN( KEY_LEFT )
                            || GET_KEY_COOLDOWN( KEY_L ) ) ) { // next badge case
                if( SAVE::SAV.getActiveFile( ).hasBadgeCase( currentPage - 1 ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    drawBadges( --currentPage );
                }
                cooldown = COOLDOWN_COUNT;
            }
        }
    }

    void handleMenuSelection( menuOption p_selection, const char* p_path ) {
        switch( p_selection ) {
        case VIEW_PARTY: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            u8 teamSize = 0;
            for( ; teamSize < 6; ++teamSize ) {
                if( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ teamSize ].m_boxdata.m_speciesId ) {
                    break;
                }
            }
            STS::partyScreen sts
                = STS::partyScreen( SAVE::SAV.getActiveFile( ).m_pkmnTeam, teamSize );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            auto res = sts.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            IO::resetScale( true, false );
            bgUpdate( );

            ANIMATE_MAP = true;
            SOUND::restoreVolume( );

            init( );
            MAP::curMap->draw( );
            if( res.m_selectedMove ) {
                for( u8 j = 0; j < 2; ++j ) {
                    if( MOVE::possible( res.m_selectedMove, j ) ) {
                        MOVE::use( res.m_selectedMove, j );
                        break;
                    }
                }
            }
            return;
        }
        case VIEW_DEX: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            DEX::dex( DEX::dex::SHOW_CAUGHT, MAX_PKMN ).run( SAVE::SAV.getActiveFile( ).m_lstDex );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            IO::clearScreenConsole( true, true );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            MAP::curMap->draw( );
            init( );
            return;
        }
        case VIEW_BAG: {
            BAG::bagViewer bv = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam );
            ANIMATE_MAP       = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            u16 res = bv.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );

            if( res ) { ITEM::use( res, _printMessage ); }
            return;
        }
        case VIEW_ID: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            runIDViewer( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );
            return;
        }
        case SAVE: {
            IO::yesNoBox yn;
            if( yn.getResult( GET_STRING( 92 ), MSG_INFO_NOCLOSE ) == IO::yesNoBox::YES ) {
                init( );
                if( FS::writeSave( p_path, [ & ]( u16 p_perc, u16 p_total ) {
                        printMessage( 0, MSG_INFO_NOCLOSE );
                        u16         stat = p_perc * 18 / p_total;
                        char        buffer[ 100 ];
                        std::string buf2 = "";
                        for( u8 i = 0; i < stat; ++i ) {
                            buf2 += "\x03";
                            if( i % 3 == 2 ) { buf2 += " "; }
                        }
                        for( u8 i = stat; i < 18; ++i ) {
                            buf2 += "\x04";
                            if( i % 3 == 2 ) { buf2 += " "; }
                        }
                        snprintf( buffer, 99, GET_STRING( 93 ), buf2.c_str( ) );
                        printMessage( buffer, MSG_INFO_NOCLOSE );
                    } ) ) {
                    printMessage( 0, MSG_INFO_NOCLOSE );
                    SOUND::playSoundEffect( SFX_SAVE );
                    printMessage( GET_STRING( 94 ), MSG_INFO );
                } else {
                    printMessage( 0, MSG_INFO_NOCLOSE );
                    printMessage( GET_STRING( 95 ), MSG_INFO );
                }
            } else {
                init( );
            }

            return;
        }
        case SETTINGS: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            SAVE::runSettings( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );

            return;
        }
        default:
            return;
        }
    }

    std::vector<std::pair<IO::inputTarget, u8>> drawMenu( ) {
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152, true );

        oam[ SPR_X_OAM_SUB ].isHidden = false;
        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
            }

            if( !oam[ SPR_MENU_OAM_SUB( i ) ].isHidden ) {
                if( i != 3 ) {
                    IO::regularFont->printString(
                        GET_STRING( 413 + i ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48 + 13,
                        oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true, IO::font::CENTER );
                } else {
                    IO::regularFont->printString( SAVE::SAV.getActiveFile( ).m_playername,
                                                  oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48 + 13,
                                                  oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true,
                                                  IO::font::CENTER );
                }

                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               i ) );
                oam[ SPR_MENU_OAM_SUB( i ) ].y = oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y;
                oam[ SPR_MENU_OAM_SUB( i ) ].x = oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x;
            } else {
                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               IO::choiceBox::DISABLED_CHOICE ) );
            }
        }
        oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;

        IO::updateOAM( true );

        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::EXIT_CHOICE ) );
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::BACK_CHOICE ) );

        return res;
    } // namespace NAV

    void selectMenuItem( u8 p_selection ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( i == p_selection ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        IO::updateOAM( true );
    }

    void focusMenu( const char* p_path ) {
        SOUND::playSoundEffect( SFX_MENU );

        IO::choiceBox menu = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        auto          res  = menu.getResult( [ & ]( u8 ) { return drawMenu( ); }, selectMenuItem );

        init( );
        if( res != IO::choiceBox::BACK_CHOICE ) {
            handleMenuSelection( NAV::menuOption( res ), p_path );
        }
    }

    void handleInput( const char* p_path ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        if( pressed & KEY_Y ) {
            // registered item
            IO::waitForKeysUp( KEY_Y );
            if( SAVE::SAV.getActiveFile( ).m_registeredItem ) {
                if( ITEM::isUsable( SAVE::SAV.getActiveFile( ).m_registeredItem ) ) {
                    ITEM::use( SAVE::SAV.getActiveFile( ).m_registeredItem, []( const char* ) {} );
                    //  updateItems( );
                } else {
                    printMessage( GET_STRING( 58 ) );
                }
            } else {
                printMessage( GET_STRING( 98 ) );
            }
            return;
        }

        if( ( pressed & KEY_X ) || ( pressed & KEY_START ) ) {
            // Open menu
            focusMenu( p_path );
            return;
        }

        for( auto c : getTouchPositions( ) ) {
            if( c.first.inRange( touch ) ) {
                oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = false;
                oam[ SPR_MENU_SEL_OAM_SUB ].x = oam[ SPR_MENU_OAM_SUB( u8( c.second ) ) ].x - 2;
                oam[ SPR_MENU_SEL_OAM_SUB ].y = oam[ SPR_MENU_OAM_SUB( u8( c.second ) ) ].y - 2;

                IO::updateOAM( true );

                bool change = true;
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );

                    if( !c.first.inRange( touch ) ) {
                        change                               = 0;
                        oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;
                        IO::updateOAM( true );
                        break;
                    }
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                if( change ) {
                    handleMenuSelection( c.second, p_path );
                    oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;
                    IO::updateOAM( true );
                }
            }
        }

#ifdef DESQUID
        if( pressed & KEY_SELECT ) {
            std::vector<u16> choices
                = { DESQUID_STRING + 46, DESQUID_STRING + 47, DESQUID_STRING + 48 };

            IO::choiceBox test = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
            switch( test.getResult( GET_STRING( DESQUID_STRING + 49 ), MSG_NOCLOSE, choices ) ) {
            case 0: {
                memset( SAVE::SAV.getActiveFile( ).m_pkmnTeam, 0,
                        sizeof( SAVE::SAV.getActiveFile( ).m_pkmnTeam ) );
                std::vector<u16> tmp
                    = { 201, 493, 521, 649, u16( 1 + rand( ) % MAX_PKMN ), MAX_PKMN };
                for( int i = 0; i < 6; ++i ) {
                    pokemon& a = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ];

                    a = pokemon( tmp[ i ], 50, !i ? ( rand( ) % 28 ) : 0, 0, i );

                    a.m_stats.m_curHP *= i / 5.0;
                    a.m_boxdata.m_experienceGained += 750;

                    // Hand out some ribbons
                    for( u8 j = 0; j < 4; ++j ) {
                        a.m_boxdata.m_ribbons0[ j ] = rand( ) % 255;
                        a.m_boxdata.m_ribbons1[ j ] = rand( ) % 255;
                        a.m_boxdata.m_ribbons2[ j ] = rand( ) % 255;
                    }
                    a.m_boxdata.m_ribbons1[ 2 ] = rand( ) % 63;
                    a.m_boxdata.m_ribbons1[ 3 ] = 0;
                    if( a.m_boxdata.m_speciesId == 493 ) {
                        u8 plate = rand( ) % 17;
                        if( plate < 16 )
                            a.giveItem( I_FLAME_PLATE + plate );
                        else
                            a.giveItem( I_PIXIE_PLATE );
                    } else {
                        a.m_boxdata.m_heldItem = 1 + rand( ) % 400;
                    }

                    for( u16 j = 1; j <= MAX_PKMN; ++j )
                        SAVE::SAV.getActiveFile( ).m_caughtPkmn[ ( j ) / 8 ] |= ( 1 << ( j % 8 ) );
                }
                for( u16 j : { 493, 649, 648, 647, 487, 492, 641, 642, 646, 645, 643, 644 } ) {
                    auto a       = pokemon( j, 50, 0, 0, j ).m_boxdata;
                    a.m_gotPlace = j;
                    SAVE::SAV.getActiveFile( ).storePkmn( a );
                }

                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 0 ] = M_SURF;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 1 ] = M_WATERFALL;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 0 ] = M_ROCK_CLIMB;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 3 ].m_boxdata.m_moves[ 0 ] = M_SWEET_SCENT;

                init( );
                swiWaitForVBlank( );
                break;
            }
            case 1:
                for( u16 j = 1; j < MAX_ITEMS; ++j ) {
                    auto c = ITEM::getItemData( j );
                    if( c.m_itemType )
                        SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::toBagType( c.m_itemType ), j,
                                                                 1 );
                }
                init( );
                break;
            case 2: {
                BOX::boxViewer bxv;
                ANIMATE_MAP = false;
                videoSetMode( MODE_5_2D );
                bgUpdate( );
                SOUND::dimVolume( );

                bxv.run( );

                FADE_TOP_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                IO::initVideoSub( );
                IO::resetScale( true, false );
                ANIMATE_MAP = true;
                SOUND::restoreVolume( );
                init( );
                MAP::curMap->draw( );
                break;
            }
            }
        }
#endif
    }
} // namespace NAV

/*
                void drawMapMug( ) {
                    auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr(
IO::bg3sub ); char buffer[ 100 ]; snprintf( buffer, 99, "%03hu/%hu_%hhu", CURRENT_BANK /
FS::ITEMS_PER_DIR, CURRENT_BANK, getCurrentDaytime( ) % 4 ); FS::readPictureData( ptr,
"nitro:/PICS/MAP_MUG/", buffer, 512, 49152, !SCREENS_SWAPPED ); drawBorder( );
                }

                void draw( bool p_initMainSrites, u8 p_newIdx ) {
                    if( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx == p_newIdx )
                        return;
                    else if( p_newIdx == u8( 255 ) )
                        p_newIdx = SAVE::SAV.getActiveFile( ).m_options.m_bgIdx;

                    auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr(
IO::bg3sub ); auto pal = SCREENS_SWAPPED ? BG_PALETTE : BG_PALETTE_SUB;

                    if( STATE != MAP_MUG ) {
                        if( !BGs[ p_newIdx ].m_loadFromRom ) {
                            dmaCopy( BGs[ p_newIdx ].m_mainMenu, ptr, 256 * 192 );
                            dmaCopy( BGs[ p_newIdx ].m_mainMenuPal, pal, 192 * 2 );
                            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx = p_newIdx;
                        } else if( !FS::readNavScreenData( ptr, BGs[ p_newIdx ].m_name.c_str( ),
                                                           p_newIdx ) ) {
                            dmaCopy( BGs[ 0 ].m_mainMenu, ptr, 256 * 192 );
                            dmaCopy( BGs[ 0 ].m_mainMenuPal, pal, 192 * 2 );
                            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx = 0;
                        } else
                            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx = p_newIdx;
                        drawBorder( );
                    } else if( STATE == MAP_MUG ) {
                        drawMapMug( );
                    }
                    if( p_initMainSrites && ALLOW_INIT ) initMainSprites( STATE != HOME );
                }

                void showNewMap( u8 p_newMap ) {
                    if( p_newMap == CURRENT_BANK ) return;
                    CURRENT_BANK = p_newMap;
                    CURRENT_MAP  = MAP::curMap->getCurrentLocationId( );
                    char buffer[ 100 ];
                    snprintf( buffer, 99, "%03hhu/%hu_%hhu", CURRENT_BANK / FS::ITEMS_PER_DIR,
                              CURRENT_BANK, getCurrentDaytime( ) % 4 );
                    if( FS::exists( "nitro:/PICS/MAP_MUG/", buffer ) )
                        STATE = MAP_MUG;
                    else if( STATE == MAP_MUG )
                        STATE = HOME;

                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = ( STATE == HOME );
                    IO::updateOAM( true );

                    draw( false );
                }

                void updateMap( u16 p_newMap ) {
                    if( p_newMap != CURRENT_MAP ) {
                        CURRENT_MAP = p_newMap;
                        draw( false );
                    }
                }

            } // namespace NAV
*/

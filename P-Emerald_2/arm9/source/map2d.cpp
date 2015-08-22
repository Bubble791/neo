///*
//Pok�mon Emerald 2 Version
//------------------------------
//
//file        : map2d.cpp
//author      : Philip Wellnitz 
//description :
//
//Copyright (C) 2012 - 2015
//Philip Wellnitz 
//
//This file is part of Pok�mon Emerald 2 Version.
//
//Pok�mon Emerald 2 Version is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Pok�mon Emerald 2 Version is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Pok�mon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
//*/
//
//
//
//#include "map2d.h"
//
//#include "buffer.h"
//#include "uio.h"
//
//#include <fstream>
//#include <vector>
//#include <fat.h>
//#include <nds.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <math.h>
//
//namespace MAP {
//#define PLAYERSPRITE 0
//
//
//    int tcnt = 0;
//
//    Map::pos Map::getFlyPos( const char* p_path, const char* p_name ) {
//        sprintf( buffer, "%s%s.fp", p_path, p_name );
//        FILE* mapF = fopen( buffer, "rb" );
//        if( !mapF )
//            return{ -1, -1, -1 };
//
//        u16 x, y;
//        u8 z;
//        fscanf( mapF, "%hu %hu %hhu", &x, &y, &z );
//        fclose( mapF );
//        return{ x, y, z };
//    }
//}
//
//
//void animateMap( u8 p_frame ) {
//    u8* tileMemory = (u8*)BG_TILE_RAM( 1 );
//    for( size_t i = 0; i < acMap->m_animations.size( ); ++i ) {
//        MAP::Animation& a = acMap->m_animations[ i ];
//        if( ( p_frame ) % ( a.m_speed ) == 0 || a.m_speed == 1 ) {
//            a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
//            swiCopy( &a.m_animationTiles[ a.m_acFrame ], tileMemory + a.m_tileIdx * 32, 16 );
//        }
//    }
//}
//
//void initMapSprites( ) {
//    IO::initOAMTable( false );
//
//    IO::SpriteInfo * SQCHAInfo = &IO::spriteInfoTop[ 0 ];
//    SpriteEntry * SQCHA = &IO::OamTop->oamBuffer[ 0 ];
//    SQCHAInfo->m_oamId = 0;
//    SQCHAInfo->m_width = 16;
//    SQCHAInfo->m_height = 32;
//    SQCHAInfo->m_angle = 0;
//    SQCHAInfo->m_entry = SQCHA;
//    SQCHA->y = 72;
//    SQCHA->isRotateScale = false;
//    SQCHA->blendMode = OBJMODE_NORMAL;
//    SQCHA->isMosaic = true;
//    SQCHA->colorMode = OBJCOLOR_16;
//    SQCHA->shape = OBJSHAPE_TALL;
//    SQCHA->isHidden = false;
//    SQCHA->x = 120;
//    SQCHA->size = OBJSIZE_32;
//    SQCHA->gfxIndex = 0;
//    SQCHA->priority = OBJPRIORITY_2;
//    SQCHA->palette = 0;
//
//    loadframe( SQCHAInfo, FS::SAV->m_overWorldIdx, 0 );
//
//    SQCHAInfo = &IO::spriteInfoTop[ 1 ];
//    SQCHA = &IO::OamTop->oamBuffer[ 1 ];
//    SQCHAInfo->m_oamId = 1;
//    SQCHAInfo->m_width = 32;
//    SQCHAInfo->m_height = 32;
//    SQCHAInfo->m_angle = 0;
//    SQCHAInfo->m_entry = SQCHA;
//    SQCHA->y = 72;
//    SQCHA->isRotateScale = false;
//    SQCHA->blendMode = OBJMODE_NORMAL;
//    SQCHA->isMosaic = true;
//    SQCHA->colorMode = OBJCOLOR_16;
//    SQCHA->shape = OBJSHAPE_SQUARE;
//    SQCHA->isHidden = true;
//    SQCHA->x = 112;
//    SQCHA->size = OBJSIZE_32;
//    SQCHA->gfxIndex = 16;
//    SQCHA->priority = OBJPRIORITY_2;
//    SQCHA->palette = 0;
//
//    IO::SpriteInfo * B2Info = &IO::spriteInfoTop[ 2 ];
//    SpriteEntry * B2 = &IO::OamTop->oamBuffer[ 2 ];
//    B2Info->m_oamId = 2;
//    B2Info->m_width = 64;
//    B2Info->m_height = 64;
//    B2Info->m_angle = 0;
//    B2Info->m_entry = B2;
//    B2->isRotateScale = false;
//    B2->blendMode = OBJMODE_NORMAL;
//    B2->isMosaic = false;
//    B2->colorMode = OBJCOLOR_16;
//    B2->shape = OBJSHAPE_SQUARE;
//    B2->isHidden = true;
//    B2->size = OBJSIZE_64;
//    B2->gfxIndex = 32;
//    B2->priority = OBJPRIORITY_1;
//    B2->palette = 1;
//    B2->x = 64;
//    B2->y = 32;
//
//    B2 = &IO::OamTop->oamBuffer[ 3 ];
//    B2->isRotateScale = false;
//    B2->blendMode = OBJMODE_NORMAL;
//    B2->isMosaic = false;
//    B2->colorMode = OBJCOLOR_16;
//    B2->shape = OBJSHAPE_SQUARE;
//    B2->isHidden = true;
//    B2->size = OBJSIZE_64;
//    B2->gfxIndex = 32;
//    B2->priority = OBJPRIORITY_1;
//    B2->palette = 1;
//    B2->x = 128;
//    B2->y = 32;
//    B2->hFlip = true;
//
//    B2 = &IO::OamTop->oamBuffer[ 4 ];
//    B2->isRotateScale = false;
//    B2->blendMode = OBJMODE_NORMAL;
//    B2->isMosaic = false;
//    B2->colorMode = OBJCOLOR_16;
//    B2->shape = OBJSHAPE_SQUARE;
//    B2->isHidden = true;
//    B2->size = OBJSIZE_64;
//    B2->gfxIndex = 32;
//    B2->priority = OBJPRIORITY_1;
//    B2->palette = 1;
//    B2->x = 64;
//    B2->y = 96;
//    B2->hFlip = false;
//    B2->vFlip = true;
//
//    B2 = &IO::OamTop->oamBuffer[ 5 ];
//    B2->isRotateScale = false;
//    B2->blendMode = OBJMODE_NORMAL;
//    B2->isMosaic = false;
//    B2->colorMode = OBJCOLOR_16;
//    B2->shape = OBJSHAPE_SQUARE;
//    B2->isHidden = true;
//    B2->size = OBJSIZE_64;
//    B2->gfxIndex = 32;
//    B2->priority = OBJPRIORITY_1;
//    B2->palette = 1;
//    B2->x = 128;
//    B2->y = 96;
//    B2->hFlip = true;
//    B2->vFlip = true;
//
//    dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, BigCirc1Pal, &SPRITE_PALETTE[ 16 ], 32 );
//    dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, BigCirc1Tiles, &SPRITE_GFX[ 32 * 32 / sizeof( SPRITE_GFX[ 0 ] ) ], BigCirc1TilesLen );
//    IO::updateOAM( false );
//}
//
//int stepcnt = 0;
//void stepincrease( ) {
//    stepcnt = ( stepcnt + 1 ) % 256;
//    if( stepcnt == 0 ) {
//        for( size_t s = 0; s < 6; ++s ) {
//            pokemon& ac = FS::SAV->m_pkmnTeam[ s ];
//            if( !ac.m_boxdata.m_speciesId )
//                break;
//
//            if( ac.m_boxdata.m_individualValues.m_isEgg ) {
//                ac.m_boxdata.m_steps--;
//                if( ac.m_boxdata.m_steps == 0 ) {
//                    ac.m_boxdata.m_individualValues.m_isEgg = false;
//                    ac.m_boxdata.m_hatchPlace = curMap->getCurrentLocationId( );
//                    ac.m_boxdata.m_hatchDate[ 0 ] = acday;
//                    ac.m_boxdata.m_hatchDate[ 1 ] = acmonth + 1;
//                    ac.m_boxdata.m_hatchDate[ 2 ] = ( acyear + 1900 ) % 100;
//                    char buffer[ 50 ];
//                    sprintf( buffer, "%ls sch�pfte\naus dem Ei!", ac.m_boxdata.m_name );
//                    IO::messageBox M( buffer );
//                }
//            } else
//                ac.m_boxdata.m_steps = std::min( 255, ac.m_boxdata.m_steps + 1 );
//        }
//    }
//}
//
//
//void shoUseAttack( u16 p_pkmIdx, bool p_female, bool p_shiny ) {
//    IO::OamTop->oamBuffer[ 0 ].isHidden = true;
//    IO::OamTop->oamBuffer[ 1 ].isHidden = false;
//    for( u8 i = 0; i < 5; ++i ) {
//        loadframe( &IO::spriteInfoTop[ 1 ], FS::SAV->m_player.m_picNum + 4, i, true );
//        IO::updateOAM( false );
//        swiWaitForVBlank( );
//        swiWaitForVBlank( );
//        swiWaitForVBlank( );
//    }
//    for( u8 i = 0; i < 4; ++i )
//        IO::OamTop->oamBuffer[ 2 + i ].isHidden = false;
//    if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 6, 2, 96, false, p_shiny, p_female ) ) {
//        IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 6, 2, 96, false, p_shiny, !p_female );
//    }
//    IO::updateOAM( false );
//
//    for( u8 i = 0; i < 40; ++i )
//        swiWaitForVBlank( );
//
//    //animateHero(lastdir,2);
//    IO::OamTop->oamBuffer[ 0 ].isHidden = false;
//    IO::OamTop->oamBuffer[ 1 ].isHidden = true;
//    for( u8 i = 0; i < 8; ++i )
//        IO::OamTop->oamBuffer[ 2 + i ].isHidden = true;
//    IO::updateOAM( false );
//}

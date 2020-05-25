/*
 Pokémon neo
 ------------------------------

 file : sprite.h
 author : Philip Wellnitz
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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Pokémon neo. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <nds.h>

#include "move.h"
#include "saveOptions.h"
#include "type.h"

namespace IO {
    extern const unsigned int*   TypeTiles[ 19 ][ 2 ];
    extern const unsigned short* TypePals[ 19 ][ 2 ];
    extern const unsigned int*   HitTypeTiles[ 3 ];
    extern const unsigned short* HitTypePals[ 3 ];
#define MAX_PLATFORMS 12
    extern const unsigned int*   PlatformTiles[ 2 * MAX_PLATFORMS ];
    extern const unsigned short* PlatformPals[ MAX_PLATFORMS ];

    struct SpriteInfo {
        u8           m_oamId;
        u8           m_width;
        u8           m_height;
        u16          m_angle;
        SpriteEntry* m_entry;
    };

    void updateOAM( bool p_bottom );

    void initOAMTable( bool p_bottom );

    void rotateSprite( SpriteRotation* p_spriteRotation, u16 p_angle );

    void setSpriteVisibility( SpriteEntry* p_spriteEntry, bool p_hidden, bool p_affine = false,
                              bool p_doubleBound = false );

    inline void setSpritePosition( SpriteEntry* p_spriteEntry, u16 p_x = 0, u16 p_y = 0 );
    inline void setSpritePriority( SpriteEntry* p_spriteEntry, ObjPriority p_priority );

    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, bool p_bottom );
    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, const u16 p_palLen,
                        bool p_bottom );
    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, const u8 p_startIdx,
                        const u16 p_palLen, bool p_bottom );
    void copySpriteData( const unsigned int* p_spriteData, const u16 p_tileIdx,
                         const u32 p_spriteDataLen, bool p_bottom );

    u16 loadSprite( const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileIdx, const u16 p_posX,
                    const u16 p_posY, const u8 p_width, const u8 p_height,
                    const unsigned short* p_spritePal, const unsigned int* p_spriteData,
                    const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                    ObjPriority p_priority, bool p_bottom,
                    ObjBlendMode p_blendMode = OBJMODE_NORMAL );
    u16 loadSprite( const u8 p_oamIdx, const u8 p_palIdx, const u8 p_palPos, const u16 p_tileIdx,
                    const u16 p_posX, const u16 p_posY, const u8 p_width, const u8 p_height,
                    const unsigned short* p_spritePal, const unsigned int* p_spriteData,
                    const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                    ObjPriority p_priority, bool p_bottom,
                    ObjBlendMode p_blendMode = OBJMODE_NORMAL );

    u16 loadPKMNSprite( const char* p_path, const u16 p_pkmnNo, const s16 p_posX, const s16 p_posY,
                        u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom,
                        bool p_shiny = false, bool p_female = false, bool p_flipX = false,
                        bool p_topOnly = false, u8 p_forme = 0 );

    u16 loadPKMNSprite( const u16 p_pkmnNo, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                        u8 p_palCnt, u16 p_tileCnt, bool p_bottom, bool p_shiny = false,
                        bool p_female = false, bool p_flipX = false, bool p_topOnly = false,
                        u8 p_forme = 0 );

    u16 loadEggSprite( const u16 p_posX, const u16 p_posY, u8 p_oamIndex, u8 p_palCnt,
                       u16 p_tileCnt, bool p_bottom = false );

    u16 loadTrainerSprite( const char* p_path, const char* p_name, const u16 p_posX,
                           const u16 p_posY, u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt,
                           bool p_bottom, bool p_flipX = false, bool p_topOnly = false );

    u16 loadTrainerSprite( const char* p_name, const u16 p_posX, const u16 p_posY, u8 p_oamIndex,
                           u8 p_palCnt, u16 p_tileCnt, bool p_bottom, bool p_flipX = false,
                           bool p_topOnly = false );

    u16 loadAnimatedSprite( FILE* p_file, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                            u8 p_palCnt, u16 p_tileCnt, ObjPriority p_priority = OBJPRIORITY_2,
                            bool p_bottom = false );
    u16 loadOWSprite( const u16 p_picnum, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                      u8 p_palCnt, u16 p_tileCnt );

    void setOWSpriteFrame( u8 p_frame, u8 p_oamIndex, u16 p_tileCnt );
    void setAnimatedSpriteFrame( u8 p_frame, bool p_hFlip, u8 p_oamIndex, u16 p_tileCnt );

    u16 loadIcon( const char* p_path, const char* p_name, const s16 p_posX, const s16 p_posY,
                  u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom );
    u16 loadIcon( const char* p_path, const char* p_name, const s16 p_posX, const s16 p_posY,
                  u8 p_oamIndex, u8 p_palCnt, u8 p_palPos, u16 p_tileCnt, bool p_bottom );

    u16 loadItemIcon( const u16 p_itemId, const u16 p_posX, const u16 p_posY, u8 p_oamIndex,
                      u8 p_palcnt, u16 p_tileCnt, bool p_bottom = true );

    u16 loadTMIcon( type p_type, bool p_hm, const u16 p_posX, const u16 p_posY, u8 p_oamIndex,
                    u8 p_palCnt, u16 p_tileCnt, bool p_bottom = true );

    u16 loadPKMNIcon( const u16 p_pkmnNo, const u16 p_posX, const u16 p_posY, u8 p_oamIndex,
                      u8 p_palcnt, u16 p_tileCnt, bool p_bottom = true, u8 p_forme = 0,
                      bool p_shiny = false, bool p_female = false );
    u16 loadPKMNIcon( const u16 p_pkmnNo, const u16 p_posX, const u16 p_posY, u8 p_oamIndex,
                      u8 p_palcnt, u8 p_palPos, u16 p_tileCnt, bool p_bottom = true,
                      u8 p_forme = 0 );

    u16 loadEggIcon( const u16 p_posX, const u16 p_posY, u8 p_oamIndex, u8 p_palcnt, u16 p_tileCnt,
                     bool p_bottom = true );
    u16 loadEggIcon( const u16 p_posX, const u16 p_posY, u8 p_oamIndex, u8 p_palCnt, u8 p_palpos,
                     u16 p_tileCnt, bool p_bottom = true );

    u16 loadTypeIcon( type p_type, const u16 p_posX, const u16 p_posY, u8 p_oamIndex, u8 p_palCnt,
                      u16 p_tileCnt, bool p_bottom, SAVE::language p_language );

    u16 loadDamageCategoryIcon( MOVE::moveHitTypes p_type, const u16 p_posX, const u16 p_posY,
                                u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom );

    u16 loadRibbonIcon( u8 p_ribbonIdx, const u16 p_posX, const u16 p_posY, u8 p_oamIndex,
                        u8 p_palCnt, u16 p_tileCnt, bool p_bottom );
} // namespace IO
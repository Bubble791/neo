/*
Pokémon neo
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine

Copyright (C) 2012 - 2021
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

#include <algorithm>

#include "abilityNames.h"
#include "bagViewer.h"
#include "battle.h"
#include "battleDefines.h"
#include "battleTrainer.h"
#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "gameStart.h"
#include "mapDrawer.h"
#include "nav.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sound.h"
#include "sprite.h"
#include "uio.h"

#ifdef DESQUID_MORE
#include <cassert>
#endif

namespace MAP {
#define MAP_BORDER 0x3f

#define SPR_PKMN_OAM 100
#define SPR_CIRC_OAM 104

#define SPR_PKMN_GFX 303
#define SPR_CIRC_GFX 447

    constexpr direction movement2Direction( u8 p_move ) {
        switch( p_move ) {
        case 0: return UP;
        case 1: return DOWN;
        case 2: return RIGHT;
        case 3: return LEFT;

        default: return DOWN;
        }
    }

    direction getRandomLookDirection( moveMode p_movement ) {
        u8 st = rand( ) % 4;

        for( u8 i = 0; i < 4; ++i ) {
            if( ( p_movement == WALK_AROUND_LEFT_RIGHT ) || ( p_movement == WALK_AROUND_UP_DOWN )
                || ( p_movement & ( 1 << ( ( st + i ) % 4 ) ) ) ) {
                return movement2Direction( ( st + i ) % 4 );
            }
        }

        return DOWN;
    }

    mapDrawer* curMap = nullptr;
#define CUR_SLICE _slices[ _curX ][ _curY ]
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }

    void mapDrawer::loadMapObject( std::pair<u8, mapObject>& p_mapObject ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        switch( p_mapObject.second.m_event.m_type ) {
        default:
        case EVENT_TRAINER:
        case EVENT_NPC_MESSAGE:
        case EVENT_NPC: {
            p_mapObject.first = _mapSprites.loadSprite(
                curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                mapSpriteManager::SPTYPE_NPC, p_mapObject.second.sprite( ) );

            _mapSprites.setFrame( p_mapObject.first, getFrame( p_mapObject.second.m_direction ),
                                  false );
#ifdef DESQUID_MORE
            IO::fadeScreen( IO::UNFADE );
            NAV::printMessage(
                ( std::to_string( p_mapObject.first ) + " " + std::to_string( curx ) + " "
                  + std::to_string( cury ) + " " + std::to_string( p_mapObject.second.m_pos.m_posX )
                  + " " + std::to_string( p_mapObject.second.m_pos.m_posY ) + " "
                  + std::to_string( IO::OamTop->oamBuffer[ p_mapObject.first ].isHidden ) ) );
#endif
            break;
        }
        case EVENT_HMOBJECT: {
            if( p_mapObject.second.m_event.m_data.m_hmObject.m_hmType ) {
                p_mapObject.first = _mapSprites.loadSprite(
                    curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                    p_mapObject.second.m_event.m_data.m_hmObject.m_hmType );
            } else {
                // HM object got destroyed already
                p_mapObject.first = 255;
            }
            break;
        }
        case EVENT_BERRYTREE: {
            // Check the growth of the specified berry tree
            if( SAVE::SAV.getActiveFile( ).berryIsAlive(
                    p_mapObject.second.m_event.m_data.m_berryTree.m_treeIdx ) ) {
                u8 stage = SAVE::SAV.getActiveFile( ).getBerryStage(
                    p_mapObject.second.m_event.m_data.m_berryTree.m_treeIdx );
                u8 berryType = SAVE::SAV.getActiveFile( ).getBerry(
                    p_mapObject.second.m_event.m_data.m_berryTree.m_treeIdx );
                p_mapObject.first = _mapSprites.loadBerryTree(
                    curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                    berryType, stage );
            } else {
                p_mapObject.first = 255;
            }
            break;
        }
        case EVENT_ITEM: {
            if( p_mapObject.second.m_event.m_data.m_item.m_itemType ) {
                p_mapObject.first = _mapSprites.loadSprite(
                    curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                    p_mapObject.second.m_event.m_data.m_item.m_itemType == 1
                        ? mapSpriteManager::SPR_ITEM
                        : mapSpriteManager::SPR_HMBALL );
            } else {
                // No item icon for hidden items (otherwise the "hidden" part would be
                // pointless, right)
                p_mapObject.first = 255;
            }
        }
        }
    }

    const mapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
            .m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    const block& mapDrawer::at( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        u16  blockidx = _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
                           .m_blocks[ p_y % SIZE ][ p_x % SIZE ]
                           .m_blockidx;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ].m_blockSet.m_blocks[ blockidx ];
    }

    mapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
            .m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    block& mapDrawer::at( u16 p_x, u16 p_y ) {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        u16  blockidx = _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
                           .m_blocks[ p_y % SIZE ][ p_x % SIZE ]
                           .m_blockidx;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ].m_blockSet.m_blocks[ blockidx ];
    }

    const mapData& mapDrawer::currentData( ) const {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        return currentData( curx, cury );
    }
    const mapData& mapDrawer::currentData( u16 p_x, u16 p_y ) const {
        u16 curx = p_x;
        u16 cury = p_y;

        bool x = ( curx / SIZE != CUR_SLICE.m_x ), y = ( cury / SIZE != CUR_SLICE.m_y );
        return _data[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ];
    }

#define CUR_DATA currentData( )
    u16* mapMemory[ 4 ];
    s8   fastBike = false;

    void mapDrawer::loadBlock( block p_curblock, u32 p_memPos ) {
        u8   toplayer = 1, bottomlayer = 3;
        bool elevateTopLayer = p_curblock.m_topbehave == 0x10;

        mapMemory[ toplayer ][ p_memPos ]      = !elevateTopLayer * p_curblock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 1 ]  = !elevateTopLayer * p_curblock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer ][ p_memPos + 32 ] = !elevateTopLayer * p_curblock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 33 ] = !elevateTopLayer * p_curblock.m_top[ 1 ][ 1 ];

        mapMemory[ toplayer + 1 ][ p_memPos ]      = elevateTopLayer * p_curblock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 1 ]  = elevateTopLayer * p_curblock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 32 ] = elevateTopLayer * p_curblock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 33 ] = elevateTopLayer * p_curblock.m_top[ 1 ][ 1 ];

        mapMemory[ bottomlayer ][ p_memPos ]      = p_curblock.m_bottom[ 0 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 1 ]  = p_curblock.m_bottom[ 0 ][ 1 ];
        mapMemory[ bottomlayer ][ p_memPos + 32 ] = p_curblock.m_bottom[ 1 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 33 ] = p_curblock.m_bottom[ 1 ][ 1 ];
    }
    void mapDrawer::loadBlock( block p_curblock, u8 p_scrnX, u8 p_scrnY ) {
        u32 c = 64 * u32( p_scrnY ) + 2 * ( u32( p_scrnX ) % 16 );
        c += ( u32( p_scrnX ) / 16 ) * 1024;
        loadBlock( p_curblock, c );
    }

    void mapDrawer::registerOnBankChangedHandler( std::function<void( u8 )> p_handler ) {
        _newBankCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnLocationChangedHandler( std::function<void( u16 )> p_handler ) {
        _newLocationCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnMoveModeChangedHandler( std::function<void( moveMode )> p_handler ) {
        _newMoveModeCallbacks.push_back( p_handler );
    }
    // Drawing of Maps and stuff

    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) {
        if( p_init ) {
            videoSetMode( MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE
                          | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                          | ( ( DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
                                | DISPLAY_SPR_1D_BMP_SIZE_128 | ( 5 << 28 ) | 2 )
                              & 0xffffff0 ) );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );

            FADE_TOP_DARK( );
            dmaFillHalfWords( 0, BG_PALETTE, 512 );
            bgUpdate( );

            _curX = _curY = 0;

            u16 mx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                my = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE, my / SIZE,
                            &_slices[ _curX ][ _curY ], _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE, my / SIZE,
                             _data[ _curX ][ _curY ] );
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                            my / SIZE, &_slices[ _curX ^ 1 ][ _curY ], _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                             my / SIZE, _data[ _curX ^ 1 ][ _curY ] );
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE,
                            my / SIZE + currentHalf( my ), &_slices[ _curX ][ _curY ^ 1 ],
                            _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE,
                             my / SIZE + currentHalf( my ), _data[ _curX ][ _curY ^ 1 ] );
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                            my / SIZE + currentHalf( my ), &_slices[ _curX ^ 1 ][ _curY ^ 1 ],
                            _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                             my / SIZE + currentHalf( my ), _data[ _curX ^ 1 ][ _curY ^ 1 ] );

            for( u8 i = 1; i < 4; ++i ) {
                bgInit( i - 1, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetScroll( i - 1, 120, 40 );
            }
            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );
            dmaCopy( CUR_SLICE.m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );

            // for palettes, the unchanged day-time pal comes first
            u8 currDT = ( getCurrentDaytime( ) + 3 ) % 5;
            if( ( CUR_DATA.m_mapType & INSIDE ) || ( CUR_DATA.m_mapType & CAVE ) ) { currDT = 0; }
            dmaCopy( CUR_SLICE.m_pals + currDT * 16, BG_PALETTE, 512 );
            BG_PALETTE[ 0 ] = 0;

            for( u8 i = 1; i < 4; ++i ) {
                mapMemory[ i ] = (u16*) BG_MAP_RAM( 2 * i - 1 );
                bgSetPriority( i - 1, i );
            }

            for( u8 i = 0; i < 4; ++i ) {
                constructAndAddNewMapObjects( _data[ i % 2 ][ i / 2 ],
                                              _slices[ i % 2 ][ i / 2 ].m_x,
                                              _slices[ i % 2 ][ i / 2 ].m_y );
            }
        }

        _lastrow = NUM_ROWS - 1;
        _lastcol = NUM_COLS - 1;

        _cx = p_globX;
        _cy = p_globY;

        u16 mny = p_globY - 8;
        u16 mnx = p_globX - 15;

        for( u16 y = 0; y < NUM_ROWS; y++ )
            for( u16 x = 0; x < NUM_COLS; x++ ) { loadBlock( at( mnx + x, mny + y ), x, y ); }
        bgUpdate( );
    }

    void mapDrawer::draw( ObjPriority ) {
        _mapSprites.reset( );

        // Restore the map objects
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            loadMapObject( SAVE::SAV.getActiveFile( ).m_mapObjects[ i ] );
        }

        draw( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
              SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY, true ); // Draw the map

        drawPlayer( SAVE::SAV.getActiveFile( ).m_playerPriority ); // Draw the player

        for( auto fn : _newBankCallbacks ) { fn( SAVE::SAV.getActiveFile( ).m_currentMap ); }
        auto curLocId = getCurrentLocationId( );
        for( auto fn : _newLocationCallbacks ) { fn( curLocId ); }

        IO::fadeScreen( IO::UNFADE );
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ, false );
    }

    void mapDrawer::drawPlayer( ObjPriority p_playerPrio ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        changeMoveMode( SAVE::SAV.getActiveFile( ).m_player.m_movement );
        _mapSprites.setPriority( _playerSprite,
                                 SAVE::SAV.getActiveFile( ).m_playerPriority = p_playerPrio );
    }

    void mapDrawer::showExclamationAboveMapObject( u8 p_objectId ) {
        auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ];
        _mapSprites.showExclamation( o.first );
        SOUND::playSoundEffect( SFX_EXMARK );
        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }
        _mapSprites.hideExclamation( );
    }

    void mapDrawer::moveMapObject( u8 p_objectId, movement p_movement, bool p_movePlayer,
                                   direction p_playerMovement ) {
        // redirect object
        if( p_movement.m_frame == 0 ) {
            _mapSprites.setFrame( SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].first,
                                  getFrame( p_movement.m_direction ) );
            if( p_movePlayer ) {
                _mapSprites.setFrame( _playerSprite, getFrame( p_playerMovement ) );
            }
        }
        if( p_movement.m_frame == 15 ) {
            _mapSprites.drawFrame( SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].first,
                                   getFrame( p_movement.m_direction ) );

            if( p_movePlayer ) {
                _mapSprites.drawFrame( _playerSprite, getFrame( p_playerMovement ) );
            }
        }

        for( u8 i = 0; i < 16; ++i ) {
            if( i == p_movement.m_frame ) {
                if( p_movePlayer ) { moveCamera( p_playerMovement, true ); }
                _mapSprites.moveSprite( SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].first,
                                        p_movement.m_direction, 1 );
                if( i == 8 ) {
                    _mapSprites.nextFrame(
                        SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].first );
                    if( p_movePlayer ) { _mapSprites.nextFrame( _playerSprite ); }
                }
            }
        }
        if( p_movement.m_frame == 0 ) {
            if( p_movePlayer ) {
                animateField(
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posX,
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posY );

                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                    = SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posX;
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                    = SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posY;
            }
            SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posX
                += dir[ p_movement.m_direction ][ 0 ];
            SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posY
                += dir[ p_movement.m_direction ][ 1 ];
            animateField(
                SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second.m_pos.m_posY );
        }
    }

    void mapDrawer::animateField( u16 p_globX, u16 p_globY ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        switch( behave ) {
        case 0x24: {
            // very hacky, I know
            atom( p_globX, p_globY ).m_blockidx = 0x212;

            loadBlock( at( p_globX, p_globY ), ( _lastcol + NUM_COLS / 2 ) % NUM_COLS,
                       ( _lastrow + NUM_ROWS / 2 + 1 ) % NUM_ROWS );
            bgUpdate( );

            break;
        }
        default: break;
        }

        // TODO
    }

    void mapDrawer::stepOn( u16 p_globX, u16 p_globY, u8 p_z, bool p_allowWildPkmn ) {
        animateField( p_globX, p_globY );
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        auto curLocId = getCurrentLocationId( );
        for( auto fn : _newLocationCallbacks ) { fn( curLocId ); }

        // Check for things that activate upon stepping on a tile

        switch( behave ) {
        case 0x24: { // Add ash to the soot bag
            if( SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ),
                                                        I_SOOT_SACK ) ) {
                SAVE::SAV.getActiveFile( ).m_ashCount++;
                if( SAVE::SAV.getActiveFile( ).m_ashCount > 999'999'999 ) {
                    SAVE::SAV.getActiveFile( ).m_ashCount = 999'999'999;
                }
            }
            break;
        }
        default: break;
        }

        if( p_allowWildPkmn ) {
            bool hadBattle = false;
            // Check for trainer
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

                if( o.second.m_event.m_type == EVENT_TRAINER ) [[unlikely]] {
                    // Check if trainer can see player

                    if( std::abs( p_globX - o.second.m_pos.m_posX ) > o.second.m_range )
                        [[likely]] {
                        continue;
                    }
                    if( std::abs( p_globY - o.second.m_pos.m_posY ) > o.second.m_range )
                        [[likely]] {
                        continue;
                    }
                    if( std::abs( p_globY - o.second.m_pos.m_posY )
                        && std::abs( p_globX - o.second.m_pos.m_posX ) ) [[likely]] {
                        continue;
                    }

                    direction trainerDir = UP;
                    direction playerDir  = DOWN;
                    if( p_globY > o.second.m_pos.m_posY ) {
                        trainerDir = DOWN;
                        playerDir  = UP;
                    }
                    if( p_globX < o.second.m_pos.m_posX ) {
                        trainerDir = LEFT;
                        playerDir  = RIGHT;
                    }
                    if( p_globX > o.second.m_pos.m_posX ) {
                        trainerDir = RIGHT;
                        playerDir  = LEFT;
                    }

                    if( trainerDir != o.second.m_direction ) { continue; }

                    // Check for exclamation mark / music change
                    if( !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_TRAINER_BATTLED(
                            o.second.m_event.m_data.m_trainer.m_trainerId ) ) ) [[likely]] {
                        // player did not defeat the trainer yet
                        auto tr = BATTLE::getBattleTrainer(
                            o.second.m_event.m_data.m_trainer.m_trainerId );

                        // Check if the battle would be a double battle; if so and if the
                        // player has only a single pkmn, the battle is optional
                        if( !BATTLE::isDoubleBattleTrainerClass( tr.m_data.m_trainerClass )
                            || SAVE::SAV.getActiveFile( ).countAlivePkmn( ) >= 2 ) {

                            SAVE::SAV.getActiveFile( ).m_mapObjects[ i ].second.m_movement
                                = NO_MOVEMENT;
                            showExclamationAboveMapObject( i );
                            SOUND::playBGM(
                                SOUND::BGMforTrainerEncounter( tr.m_data.m_trainerClass ) );

                            // walk trainer to player
                            redirectPlayer( playerDir, false );

                            while( dist( p_globX, p_globY, o.second.m_pos.m_posX,
                                         o.second.m_pos.m_posY )
                                   > 1 ) {
                                for( u8 j = 0; j < 16; ++j ) {
                                    moveMapObject( i, { trainerDir, j } );
                                    swiWaitForVBlank( );
                                }
                            }
                            runEvent( o.second.m_event, i );
                        }
                    }
                }
            }

            if( !hadBattle ) { handleWildPkmn( p_globX, p_globY ); }
        }

        handleEvents( p_globX, p_globY, p_z );
    }

    void mapDrawer::loadNewRow( direction p_direction, bool p_updatePlayer ) {
        _cx += dir[ p_direction ][ 0 ];
        _cy += dir[ p_direction ][ 1 ];
#ifdef DESQUID_MORE
        assert( _cx != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                || _cy != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY );
#endif
        if( p_updatePlayer ) {
            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX = _cx;
            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY = _cy;

            SAVE::SAV.getActiveFile( ).stepIncrease( );
        }

        // Check if a new slice should be loaded
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 32 == 16 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 32 == 15 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 32 == 16 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 32 == 15 ) ) {

            // Hacky optimization: Don't load new slices on inside maps.
            if( ( CUR_DATA.m_mapType & CAVE ) || !( CUR_DATA.m_mapType & INSIDE ) ) {
                ANIMATE_MAP = false;
                DRAW_TIME   = false;
                loadSlice( p_direction );
                ANIMATE_MAP = true;
                DRAW_TIME   = true;
            }
#ifdef DESQUID_MORE
            NAV::printMessage( "Load Slice" );
#endif
        }

        // Check if map objects should be updated
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 16 == 8 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 16 == 7 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 16 == 8 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 16 == 7 ) ) {
            ANIMATE_MAP = false;
            for( u8 i = 0; i < 4; ++i ) {
                constructAndAddNewMapObjects( _data[ i % 2 ][ i / 2 ],
                                              _slices[ i % 2 ][ i / 2 ].m_x,
                                              _slices[ i % 2 ][ i / 2 ].m_y );
            }
            ANIMATE_MAP = true;
        }

        // Check if a new slice got stepped onto
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 32 == 0 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 32 == 31 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 32 == 0 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 32 == 31 ) ) {
            _curX = ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1;
            _curY = ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1;
            // Update tileset, block and palette data
            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );

            ANIMATE_MAP = false;
            dmaCopy( CUR_SLICE.m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );

            // for palettes, the unchanged day-time pal comes first
            u8 currDT = ( getCurrentDaytime( ) + 3 ) % 5;
            if( ( CUR_DATA.m_mapType & INSIDE ) || ( CUR_DATA.m_mapType & CAVE ) ) { currDT = 0; }
            dmaCopy( CUR_SLICE.m_pals + currDT * 16, BG_PALETTE, 512 );
            BG_PALETTE[ 0 ] = 0;
            ANIMATE_MAP     = true;

#ifdef DESQUID_MORE
            char buffer[ 100 ];
            snprintf( buffer, 99, "Switch Slice to (%d, %d)", _curX, _curY );
            NAV::printMessage( buffer );
#endif
        }

        switch( p_direction ) {
        case UP: {
            u16 ty  = _cy - 8;
            u16 mnx = _cx - 15;
            for( u16 x = ( _lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, _lastrow );
            _lastrow = ( _lastrow + NUM_ROWS - 1 ) % NUM_ROWS;
            break;
        }
        case LEFT: {
            u16 tx  = _cx - 15;
            u16 mny = _cy - 8;
            for( u16 y = ( _lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), _lastcol, y );
            _lastcol = ( _lastcol + NUM_COLS - 1 ) % NUM_COLS;
            break;
        }
        case DOWN: {
            _lastrow = ( _lastrow + 1 ) % NUM_ROWS;
            u16 ty   = _cy + 7;
            u16 mnx  = _cx - 15;
            for( u16 x = ( _lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, _lastrow );
            break;
        }
        case RIGHT: {
            _lastcol = ( _lastcol + 1 ) % NUM_COLS;
            u16 tx   = _cx + 16;
            u16 mny  = _cy - 8;
            for( u16 y = ( _lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), _lastcol, y );
            break;
        }
        }
        bgUpdate( );
    }

    void mapDrawer::moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows ) {
        for( u8 i = 0; i < 3; ++i ) {
            bgScroll( i, dir[ p_direction ][ 0 ], dir[ p_direction ][ 1 ] );
            bgUpdate( );
        }
        _mapSprites.moveCamera( p_direction, 1, !p_updatePlayer );
        if( p_autoLoadRows
            && ( ( dir[ p_direction ][ 0 ]
                   && ( ( bgState[ 1 ].scrollX >> 8 ) - !!dir[ p_direction ][ 0 ] + 9 ) % 16 == 0 )
                 || ( dir[ p_direction ][ 1 ]
                      && ( ( bgState[ 1 ].scrollY >> 8 ) - !!dir[ p_direction ][ 1 ] + 9 ) % 16
                             == 0 ) ) ) {
            loadNewRow( p_direction, p_updatePlayer );
        }
    }

    void mapDrawer::loadSlice( direction p_direction ) {
        auto mx = CUR_SLICE.m_x + dir[ p_direction ][ 0 ],
             my = CUR_SLICE.m_y + dir[ p_direction ][ 1 ];

        constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                        &_slices[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                                [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ],
                        _slices );
        FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                         _data[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                              [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ] );

        auto& neigh = _slices[ ( _curX + !dir[ p_direction ][ 0 ] ) & 1 ]
                             [ ( _curY + !dir[ p_direction ][ 1 ] ) & 1 ];
        mx = neigh.m_x + dir[ p_direction ][ 0 ];
        my = neigh.m_y + dir[ p_direction ][ 1 ];
        constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                        &_slices[ _curX ^ 1 ][ _curY ^ 1 ], _slices );
        FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                         _data[ _curX ^ 1 ][ _curY ^ 1 ] );
    }

    void mapDrawer::disablePkmn( s16 p_steps ) {
        SAVE::SAV.getActiveFile( ).m_repelSteps = p_steps;
    }
    void mapDrawer::enablePkmn( ) {
        SAVE::SAV.getActiveFile( ).m_repelSteps = 0;
    }

    void mapDrawer::handleWarp( warpType p_type, warpPos p_source ) {
        warpPos tg;
        u16     curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % SIZE;
        u16     cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % SIZE;
        u16     curz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;

        for( u8 i = 0; i < CUR_DATA.m_eventCount; ++i ) {
            if( CUR_DATA.m_events[ i ].m_type == EVENT_WARP && CUR_DATA.m_events[ i ].m_posX == curx
                && CUR_DATA.m_events[ i ].m_posY == cury
                && CUR_DATA.m_events[ i ].m_posZ == curz ) {
                if( CUR_DATA.m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag(
                        CUR_DATA.m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( CUR_DATA.m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag(
                        CUR_DATA.m_events[ i ].m_deactivateFlag ) ) {
                    continue;
                }

                if( CUR_DATA.m_events[ i ].m_data.m_warp.m_warpType != NO_SPECIAL ) {
                    p_type = CUR_DATA.m_events[ i ].m_data.m_warp.m_warpType;
                    if( CUR_DATA.m_events[ i ].m_data.m_warp.m_warpType == LAST_VISITED ) {
                        tg = SAVE::SAV.getActiveFile( ).m_lastWarp;
                    } else {
                        tg = warpPos( CUR_DATA.m_events[ i ].m_data.m_warp.m_bank,
                                      position( CUR_DATA.m_events[ i ].m_data.m_warp.m_mapX * SIZE
                                                    + CUR_DATA.m_events[ i ].m_data.m_warp.m_posX,
                                                CUR_DATA.m_events[ i ].m_data.m_warp.m_mapY * SIZE
                                                    + CUR_DATA.m_events[ i ].m_data.m_warp.m_posY,
                                                +CUR_DATA.m_events[ i ].m_data.m_warp.m_posZ ) );
                    }
                }
                break;
            }
        }

        if( tg.first == 0xFF ) tg = SAVE::SAV.getActiveFile( ).m_lastWarp;
        if( !tg.first && !tg.second.m_posY && !tg.second.m_posZ && !tg.second.m_posX ) return;

        SAVE::SAV.getActiveFile( ).m_lastWarp = p_source;
        warpPlayer( p_type, tg );
    }

    void mapDrawer::handleWarp( warpType p_type ) {
        warpPos current = warpPos{ SAVE::SAV.getActiveFile( ).m_currentMap,
                                   SAVE::SAV.getActiveFile( ).m_player.m_pos };
        if( p_type == LAST_VISITED ) {
            warpPos target = SAVE::SAV.getActiveFile( ).m_lastWarp;
            if( !target.first && !target.second.m_posX && !target.second.m_posY
                && !target.second.m_posZ )
                return;
            SAVE::SAV.getActiveFile( ).m_lastWarp = current;

            warpPlayer( p_type, target );
        } else {
            handleWarp( p_type, current );
        }
    }
    void mapDrawer::handleWildPkmn( u16 p_globX, u16 p_globY ) {
        u8 moveData = atom( p_globX, p_globY ).m_movedata;
        u8 behave   = at( p_globX, p_globY ).m_bottombehave;

        if( SAVE::SAV.getActiveFile( ).m_repelSteps ) return;
        // handle Pkmn stuff
        if( moveData == 0x04 && behave != 0x13 )
            handleWildPkmn( WATER );
        else if( behave == 0x02 || behave == 0x24 || behave == 0x06 || behave == 0x08 )
            handleWildPkmn( GRASS );
        else if( behave == 0x03 )
            handleWildPkmn( HIGH_GRASS );
        //        else if( CUR_DATA.m_mapType & CAVE )
        //            handleWildPkmn( GRASS );
    }
    pokemon wildPkmn;
    bool    mapDrawer::handleWildPkmn( wildPkmnType p_type, bool p_forceEncounter ) {
        u16 rn
            = rand( ) % ( 512 + 3 * SAVE::SAV.getActiveFile( ).m_options.m_encounterRateModifier );
        if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) rn /= 8;
        if( p_forceEncounter ) rn %= 40;

        u8 tier;
        if( rn < 2 )
            tier = 4;
        else if( rn < 6 )
            tier = 3;
        else if( rn < 14 )
            tier = 2;
        else if( rn < 26 )
            tier = 1;
        else
            tier = 0;
        u8 level = SAVE::SAV.getActiveFile( ).getEncounterLevel( tier );

        if( rn > 40 || !level ) {
            if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) {
                _playerIsFast = false;
                NAV::printMessage( GET_STRING( 5 ) );
            }
            return false;
        }
        if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) {
            _playerIsFast = false;
            NAV::printMessage( GET_STRING( 6 ) );
        } else if( SAVE::SAV.getActiveFile( ).m_repelSteps && !p_forceEncounter ) {
            return false;
        }

        s8 availmod = ( SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) - 3 ) / 3;

        u8 total = 0;
        for( u8 i = 0; i < CUR_DATA.m_pokemonDescrCount; ++i ) {
            if( CUR_DATA.m_pokemon[ i ].m_encounterType == p_type ) {
                s8 ownedbadge = SAVE::SAV.getActiveFile( ).getBadgeCount( ) + availmod;
                if( ownedbadge < 0 ) { ownedbadge = 0; }

                if( ownedbadge >= CUR_DATA.m_pokemon[ i ].m_slot ) {

                    if( CUR_DATA.m_pokemon[ i ].m_daytime
                        & ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) {
                        total += CUR_DATA.m_pokemon[ i ].m_encounterRate;
                    } else {
#ifdef DESQUID_MORE
                        NAV::printMessage(
                            ( std::string( "Ignoring pkmn due to wrong time: " )
                              + std::to_string( i ) + " "
                              + std::to_string( CUR_DATA.m_pokemon[ i ].m_daytime ) + " vs "
                              + std::to_string( ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) )
                                .c_str( ),
                            MSG_INFO );
#endif
                    }
                } else {
#ifdef DESQUID_MORE
                    NAV::printMessage( ( std::string( "Ignoring pkmn due to insufficient badges: " )
                                         + std::to_string( i ) )
                                           .c_str( ),
                                       MSG_INFO );
#endif
                }
            }
        }
        if( !total ) {
#ifdef DESQUID_MORE
            NAV::printMessage( "No pkmn", MSG_INFO );
#endif
            return false;
        }

#ifdef DESQUID_MORE
        NAV::printMessage( ( std::to_string( total ) ).c_str( ), MSG_INFO );
#endif
        u16 pkmnId    = 0;
        u8  pkmnForme = 0;

        u8 res = rand( ) % total;
        total  = 0;
        for( u8 i = 0; i < CUR_DATA.m_pokemonDescrCount; ++i ) {
            if( CUR_DATA.m_pokemon[ i ].m_encounterType == p_type ) {
                s8 ownedbadge = SAVE::SAV.getActiveFile( ).getBadgeCount( ) + availmod;
                if( ownedbadge < 0 ) { ownedbadge = 0; }

                if( ownedbadge >= CUR_DATA.m_pokemon[ i ].m_slot
                    && ( CUR_DATA.m_pokemon[ i ].m_daytime
                         & ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) ) {
                    total += CUR_DATA.m_pokemon[ i ].m_encounterRate;

                    if( total > res ) {
                        pkmnId    = CUR_DATA.m_pokemon[ i ].m_speciesId;
                        pkmnForme = CUR_DATA.m_pokemon[ i ].m_forme;
#ifdef DESQUID_MORE
                        NAV::printMessage(
                            ( std::to_string( total ) + " " + std::to_string( res ) ).c_str( ),
                            MSG_INFO );
#endif
                        break;
                    }
                }
            }
        }

        if( !pkmnId ) { return false; }

        bool luckyenc = SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_WISHING_CHARM )
                            ? !( rand( ) & 127 )
                            : !( rand( ) & 2047 );
        bool charm    = SAVE::SAV.getActiveFile( ).m_bag.count(
            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_SHINY_CHARM );

        if( luckyenc ) {
            SOUND::playBGM( MOD_BATTLE_WILD_ALT );
        } else {
            SOUND::playBGM( SOUND::BGMforWildBattle( pkmnId ) );
        }
        _playerIsFast = false;
        fastBike      = false;
        _mapSprites.setFrame( _playerSprite,
                              getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );

        IO::fadeScreen( IO::BATTLE );
        IO::BG_PAL( true )[ 0 ] = 0;
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );

        wildPkmn = pokemon( pkmnId, level, pkmnForme, 0, luckyenc ? 255 : ( charm ? 3 : 0 ),
                            luckyenc, false, 0, 0, luckyenc );

        u8 platform = 0, plat2 = 0;
        u8 battleBack = p_type == WATER ? CUR_DATA.m_surfBattleBG : CUR_DATA.m_battleBG;
        switch( p_type ) {
        case WATER:
            platform = CUR_DATA.m_surfBattlePlat1;
            plat2    = CUR_DATA.m_surfBattlePlat2;
            break;
        case OLD_ROD:
        case GOOD_ROD:
        case SUPER_ROD:
            platform = CUR_DATA.m_battlePlat1;
            plat2    = CUR_DATA.m_surfBattlePlat2;
            break;

        default:
            platform = CUR_DATA.m_battlePlat1;
            plat2    = CUR_DATA.m_battlePlat2;
            break;
        }

        auto playerPrio = _mapSprites.getPriority( _playerSprite );
        ANIMATE_MAP     = false;
        DRAW_TIME       = false;
        swiWaitForVBlank( );
        if( BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                            SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), wildPkmn, platform,
                            plat2, battleBack, getBattlePolicy( true ) )
                .start( )
            == BATTLE::battle::BATTLE_OPPONENT_WON ) {
            faintPlayer( );
            return true;
        }
        SOUND::restartBGM( );
        FADE_TOP_DARK( );
        draw( playerPrio );
        _mapSprites.setPriority( _playerSprite,
                                 SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
        ANIMATE_MAP = true;
        NAV::init( );

        return true;
    }

    BATTLE::battlePolicy mapDrawer::getBattlePolicy( bool p_isWildBattle, BATTLE::battleMode p_mode,
                                                     bool p_distributeEXP ) {
        BATTLE::battlePolicy res = p_isWildBattle
                                       ? BATTLE::battlePolicy( BATTLE::DEFAULT_WILD_POLICY )
                                       : BATTLE::battlePolicy( BATTLE::DEFAULT_TRAINER_POLICY );

        res.m_mode               = p_mode;
        res.m_distributeEXP      = p_distributeEXP;
        res.m_allowMegaEvolution = SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_MEGA_EVOLUTION );

        res.m_weather = BATTLE::weather::NO_WEATHER;
        switch( _weather ) {
        case SUNNY: res.m_weather = BATTLE::weather::SUN; break;
        case RAINY:
        case THUNDERSTORM: res.m_weather = BATTLE::weather::RAIN; break;
        case SNOW:
        case BLIZZARD: res.m_weather = BATTLE::weather::HAIL; break;
        case SANDSTORM: res.m_weather = BATTLE::weather::SANDSTORM; break;
        case FOG: res.m_weather = BATTLE::weather::FOG; break;
        case HEAVY_SUNLIGHT: res.m_weather = BATTLE::weather::HEAVY_SUNSHINE; break;
        case HEAVY_RAIN: res.m_weather = BATTLE::weather::HEAVY_RAIN; break;
        default: break;
        }

        return res;
    }

    void mapDrawer::handleTrainer( ) {
        // TODO
    }

    bool mapDrawer::requestWildPkmn( bool p_forceHighGrass ) {
        u8 moveData = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata;
        u8 behave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                        .m_bottombehave;

        if( moveData == 0x04 && behave != 0x13 )
            return handleWildPkmn( WATER, true );
        else if( behave == 0x02 && !p_forceHighGrass )
            return handleWildPkmn( GRASS, true );
        else if( ( behave == 0x24 || behave == 0x06 ) && !p_forceHighGrass )
            return handleWildPkmn( GRASS, true );
        else if( behave == 0x03 || p_forceHighGrass )
            return handleWildPkmn( HIGH_GRASS, true );
        else if( CUR_DATA.m_mapType & CAVE )
            return handleWildPkmn( GRASS, true );
        return false;
    }

    void mapDrawer::animateMap( u8 p_frame ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        // animate map objects

        bool change = false;
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

            if( o.first == 255 ) { continue; }

            if( o.second.m_event.m_type == EVENT_BERRYTREE ) {
                if( ( p_frame & 31 ) == 15 ) { _mapSprites.nextFrame( o.first ); }
                continue;
            }

            if( o.second.m_movement == NO_MOVEMENT ) { continue; }

            if( o.second.m_movement <= 15 || o.second.m_movement == WALK_AROUND_LEFT_RIGHT
                || o.second.m_movement == WALK_AROUND_UP_DOWN ) {
                if( ( p_frame & 127 ) == 127 ) {
                    o.second.m_direction = getRandomLookDirection( o.second.m_movement );
                    _mapSprites.setFrame( o.first, getFrame( o.second.m_direction ), false );
                    change = true;
                }
            }

            if( change ) { continue; }

            if( o.second.m_currentMovement.m_frame ) {
                moveMapObject( i, o.second.m_currentMovement );
                o.second.m_currentMovement.m_frame
                    = ( o.second.m_currentMovement.m_frame + 1 ) & 15;
                continue;
            }

            if( o.second.m_movement == WALK_AROUND_LEFT_RIGHT
                || o.second.m_movement == WALK_LEFT_RIGHT ) {
                if( ( p_frame & 127 ) == 63 ) {
                    bool nomove = false;

                    if( o.second.m_pos.m_posX % SIZE == ( o.second.m_event.m_posX + 1 ) % SIZE ) {
                        if( o.second.m_pos.m_posX + dir[ LEFT ][ 0 ] != curx
                            || o.second.m_pos.m_posY + dir[ LEFT ][ 1 ] != cury ) {
                            o.second.m_currentMovement = { LEFT, 0 };
                        } else {
                            nomove = true;
                        }
                    } else if( ( o.second.m_pos.m_posX + 1 ) % SIZE
                               == o.second.m_event.m_posX % SIZE ) {
                        if( o.second.m_pos.m_posX + dir[ RIGHT ][ 0 ] != curx
                            || o.second.m_pos.m_posY + dir[ RIGHT ][ 1 ] != cury ) {
                            o.second.m_currentMovement = { RIGHT, 0 };
                        } else {
                            nomove = true;
                        }
                    } else {
                        if( o.second.m_currentMovement.m_direction != LEFT
                            && o.second.m_currentMovement.m_direction != RIGHT ) {
                            if( o.second.m_pos.m_posX + dir[ RIGHT ][ 0 ] != curx
                                || o.second.m_pos.m_posY + dir[ RIGHT ][ 1 ] != cury ) {
                                o.second.m_currentMovement = { RIGHT, 0 };
                            } else {
                                nomove = true;
                            }
                        } else if( o.second.m_pos.m_posX
                                           + dir[ o.second.m_currentMovement.m_direction ][ 0 ]
                                       == curx
                                   && o.second.m_pos.m_posY
                                              + dir[ o.second.m_currentMovement.m_direction ][ 1 ]
                                          == cury ) {
                            nomove = true;
                        }
                    }
                    if( !nomove ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    }
                }
            }
            if( o.second.m_movement == WALK_AROUND_UP_DOWN
                || o.second.m_movement == WALK_UP_DOWN ) {
                if( ( p_frame & 127 ) == 63 ) {
                    bool nomove = false;
                    if( o.second.m_pos.m_posY % SIZE == ( o.second.m_event.m_posY + 1 ) % SIZE ) {
                        if( o.second.m_pos.m_posX + dir[ UP ][ 0 ] != curx
                            || o.second.m_pos.m_posY + dir[ UP ][ 1 ] != cury ) {
                            o.second.m_currentMovement = { UP, 0 };
                        } else {
                            nomove = true;
                        }
                    } else if( ( o.second.m_pos.m_posY + 1 ) % SIZE
                               == o.second.m_event.m_posY % SIZE ) {
                        if( o.second.m_pos.m_posX + dir[ DOWN ][ 0 ] != curx
                            || o.second.m_pos.m_posY + dir[ DOWN ][ 1 ] != cury ) {
                            o.second.m_currentMovement = { DOWN, 0 };
                        } else {
                            nomove = true;
                        }
                    } else {
                        if( o.second.m_currentMovement.m_direction != DOWN
                            && o.second.m_currentMovement.m_direction != UP ) {
                            if( o.second.m_pos.m_posX + dir[ DOWN ][ 0 ] != curx
                                || o.second.m_pos.m_posY + dir[ DOWN ][ 1 ] != cury ) {
                                o.second.m_currentMovement = { DOWN, 0 };
                            } else {
                                nomove = true;
                            }
                        } else if( o.second.m_pos.m_posX
                                           + dir[ o.second.m_currentMovement.m_direction ][ 0 ]
                                       == curx
                                   && o.second.m_pos.m_posY
                                              + dir[ o.second.m_currentMovement.m_direction ][ 1 ]
                                          == cury ) {
                            nomove = true;
                        }
                    }
                    if( !nomove ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    }
                }
            }
        }

        if( change ) { _mapSprites.update( ); }

        return;

        // if( !CUR_SLICE ) return;
        u8* tileMemory = (u8*) BG_TILE_RAM( 1 );
        if( !CUR_SLICE.m_tileSet.m_animationCount1 ) return;
        for( u8 i = 0; i < CUR_SLICE.m_tileSet.m_animationCount1; ++i ) {
            auto& a = CUR_SLICE.m_tileSet.m_animations[ i ];
            if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                dmaCopy( a.m_tiles + a.m_acFrame, tileMemory + a.m_tileIdx * 32, sizeof( tile ) );
            }
        }
        if( !CUR_SLICE.m_tileSet.m_animationCount2 ) return;
        for( u8 i = 0; i < CUR_SLICE.m_tileSet.m_animationCount2; ++i ) {
            auto& a = CUR_SLICE.m_tileSet.m_animations[ i + MAX_ANIM_PER_TILE_SET ];
            if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                dmaCopy( a.m_tiles + a.m_acFrame,
                         tileMemory + ( a.m_tileIdx + MAX_TILES_PER_TILE_SET ) * 32,
                         sizeof( tile ) );
            }
        }
    }

    mapDrawer::mapDrawer( ) : _curX( 0 ), _curY( 0 ), _playerIsFast( false ) {
        _mapSprites.init( );
    }

    // Movement stuff
    bool mapDrawer::canMove( position p_start, direction p_direction, moveMode p_moveMode ) {
        u16 nx = p_start.m_posX + dir[ p_direction ][ 0 ];
        u16 ny = p_start.m_posY + dir[ p_direction ][ 1 ];

#ifdef DESQUID
        // Walk through walls for desquid purposes.
        if( keysHeld( ) & KEY_R ) { return true; }
#endif

        // Check if any event is occupying the target block
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
            if( o.second.m_pos.m_posX == nx && o.second.m_pos.m_posY == ny ) {
                switch( o.second.m_event.m_type ) {
                case EVENT_HMOBJECT:
                    if( o.second.m_event.m_data.m_hmObject.m_hmType
                        == mapSpriteManager::SPR_STRENGTH ) {
                        // Check if the boulder could be moved by using strength
                        if( p_moveMode == STRENGTH
                            || !canMove( { nx, ny, p_start.m_posZ }, p_direction, STRENGTH ) ) {
                            return false;
                        }
                        // Check if the player has actually used strength
                        if( !_strengthUsed ) { return false; }
                        break;
                    }
                    if( o.second.m_event.m_data.m_hmObject.m_hmType ) { return false; }
                    break;
                case EVENT_ITEM:
                    if( o.second.m_event.m_data.m_item.m_itemType ) {
                        return false;
                    } // item is not hidden
                    break;
                case EVENT_NPC:
                case EVENT_NPC_MESSAGE:
                case EVENT_TRAINER:
                case EVENT_OW_PKMN:
                case EVENT_BERRYTREE: return false;
                case EVENT_GENERIC:
                    if( o.second.m_event.m_trigger & TRIGGER_INTERACT ) { return false; }
                default: break;
                }
            }
        }

        // Gather data about the source block
        u8 lstMoveData, lstBehave;
        if( nx / SIZE != p_start.m_posX / SIZE || ny / SIZE != p_start.m_posY / SIZE ) {
            lstMoveData = 0;
            lstBehave   = 0;
        } else {
            lstMoveData = atom( p_start.m_posX, p_start.m_posY ).m_movedata;

            auto lstblock = at( p_start.m_posX, p_start.m_posY );
            lstBehave     = lstblock.m_bottombehave;
        }

        // Gather data about the destination block
        u8 curMoveData, curBehave;
        curMoveData = atom( nx, ny ).m_movedata;

        auto curblock = at( nx, ny );
        curBehave     = curblock.m_bottombehave;

        // Check for special block attributes
        switch( lstBehave ) {
        case 0x30:
            if( p_direction == RIGHT ) return false;
            break;
        case 0x31:
            if( p_direction == LEFT ) return false;
            break;
        case 0x32:
            if( p_direction == UP ) return false;
            break;
        case 0x33:
            if( p_direction == DOWN ) return false;
            break;
        case 0x36:
            if( p_direction == DOWN || p_direction == LEFT ) return false;
            break;
        case 0x37:
            if( p_direction == DOWN || p_direction == RIGHT ) return false;
            break;
        case 0xa0:
            if( !( p_moveMode & WALK ) ) return false;
            break;
        case 0xc0:
            if( ( p_direction & 1 ) == 0 ) return false;
            break;
        case 0xc1:
            if( p_direction & 1 ) return false;
            break;
        case 0x62:
            if( p_direction == RIGHT ) return true;
            break;
        case 0x63:
            if( p_direction == LEFT ) return true;
            break;
        case 0x64:
            if( p_direction == UP ) return true;
            break;
        case 0x65:
        case 0x6d:
            if( p_direction == DOWN ) return true;
            break;

        case 0xd3: // Bike stuff
        case 0xd5:
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == LEFT || p_direction == RIGHT ) {
                if( ( curBehave != 0xd3 && curBehave != 0xd5 ) || p_moveMode != ACRO_BIKE
                    || !( held & KEY_B ) ) {
                    return false;
                }
            }
            break;
        case 0xd6:
        case 0xd4:
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == DOWN || p_direction == UP ) {
                if( ( curBehave != 0xd4 && curBehave != 0xd6 ) || p_moveMode != ACRO_BIKE
                    || !( held & KEY_B ) ) {
                    return false;
                }
            }
            break;

        default: break;
        }

        switch( curBehave ) {
        // Jumpy stuff
        case 0x38:
        case 0x35: return p_direction == RIGHT;
        case 0x39:
        case 0x34: return p_direction == LEFT;
        case 0x3a: return p_direction == UP;
        case 0x3b: return p_direction == DOWN;

        case 0xa0:
            if( !( p_moveMode & WALK ) ) return false;
            break;
        case 0xc0:
            if( ( p_direction & 1 ) == 0 ) return false;
            break;
        case 0xc1:
            if( p_direction & 1 ) return false;
            break;
        case 0x13: return false;
        case 0xd7: return false;

        // door-y stuff
        case 0x69: // Player can move to a door if there is a corresponding warp at the target
                   // position
            if( currentData( nx, ny ).hasEvent( EVENT_WARP, nx % SIZE, ny % SIZE,
                                                p_start.m_posZ ) ) {
                return true;
            } else {
                return false;
            }

        case 0xd3: // Bike stuff
        case 0xd5:
            if( !( p_moveMode & BIKE ) ) { return false; }
            break;
        case 0xd6:
        case 0xd4:
            if( !( p_moveMode & BIKE ) ) { return false; }
            break;

        default: break;
        }

        if( ( p_moveMode & BIKE ) && !canBike( { nx, ny, p_start.m_posZ } ) ) { return false; }

        // Check for movedata stuff
        if( curMoveData % 4 == 1 ) { return false; }
        if( lstMoveData == 0x0a ) { // Stand up (only possible for the player)
            return p_direction == SAVE::SAV.getActiveFile( ).m_player.m_direction;
        }
        if( curMoveData == 0x0a ) { // Sit down
            return ( p_moveMode == WALK );
        }
        if( curMoveData == 4 && !( p_moveMode & SURF ) ) {
            return false;
        } else if( curMoveData == 4 ) {
            return true;
        }

        if( curMoveData == 0x0c && lstMoveData == 4 ) { return true; }
        if( !curMoveData || !lstMoveData ) { return true; }
        if( curMoveData == 0x3c ) { return true; }
        return curMoveData % 4 == 0 && curMoveData / 4 == p_start.m_posZ;
    }
    void mapDrawer::movePlayer( direction p_direction, bool p_fast ) {

        u16 curx        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u8  curz        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        u16 nx          = curx + dir[ p_direction ][ 0 ];
        u16 ny          = cury + dir[ p_direction ][ 1 ];
        u8  newMoveData = atom( nx, ny ).m_movedata;
        u8  lstMoveData = atom( curx, cury ).m_movedata;
        u8  newBehave   = at( nx, ny ).m_bottombehave;
        u8  lstBehave   = at( curx, cury ).m_bottombehave;

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement != moveMode::WALK ) {
            p_fast = false; // Running is only possible when the player is actually walking
        }

        // Check if any event is occupying the target block and push it if necessary
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
            if( o.second.m_pos.m_posX == nx && o.second.m_pos.m_posY == ny ) {
                switch( o.second.m_event.m_type ) {
                case EVENT_HMOBJECT:
                    if( o.second.m_event.m_data.m_hmObject.m_hmType
                        == mapSpriteManager::SPR_STRENGTH ) {
                        // Check if the boulder could be moved by using strength
                        if( !canMove( { nx, ny, curz }, p_direction, STRENGTH ) ) { continue; }
                        // Check if the player has actually used strength
                        if( !_strengthUsed ) { continue; }

                        // push the boulder one block in the current direction
                        SOUND::playSoundEffect( SFX_HM_STRENGTH );
                        for( u8 f = 0; f < 16; ++f ) {
                            _mapSprites.moveSprite( o.first, p_direction, 1 );
                            swiWaitForVBlank( );
                        }
                        o.second.m_pos.m_posX += dir[ p_direction ][ 0 ];
                        o.second.m_pos.m_posY += dir[ p_direction ][ 1 ];
                    }
                    break;
                default: break;
                }
            }
        }

        bool reinit = false, moving = true, hadjump = false;
        while( moving ) {
            if( newMoveData == MAP_BORDER ) {
                fastBike = false;
                stopPlayer( direction( ( u8( p_direction ) + 2 ) % 4 ) );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                stopPlayer( );
                NAV::printMessage( GET_STRING( 7 ), MSG_INFO );
                _playerIsFast = false;
                return;
            }
            // Check for end of surf, stand up and sit down
            if( lstMoveData == 0x0a
                && newMoveData != 0x0a ) { // Stand up (only possible for the player)
                if( p_direction != SAVE::SAV.getActiveFile( ).m_player.m_direction ) return;

                standUpPlayer( p_direction );
                stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
                return;
            } else if( lstMoveData == 0x0a ) {
                fastBike = false;
                return;
            }
            if( newMoveData == 0x0a ) { // Sit down
                if( SAVE::SAV.getActiveFile( ).m_player.m_movement != WALK ) return;
                SAVE::SAV.getActiveFile( ).m_player.m_direction
                    = direction( ( u8( p_direction ) + 2 ) % 4 );
                _mapSprites.setFrame( _playerSprite,
                                      getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
                sitDownPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction, SIT );
                fastBike = false;
                return;
            }
            if( newMoveData == 0x0c && lstMoveData == 4 ) { // End of surf
                standUpPlayer( p_direction );
                stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
                fastBike = false;
                return;
            }

            // Check for jumps/slides/...
            if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, p_direction,
                          SAVE::SAV.getActiveFile( ).m_player.m_movement ) ) {
                stopPlayer( p_direction );
                fastBike = false;
                return;
            }

            switch( newBehave ) {
            // First check for jumps
            case 0x38:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( RIGHT );
                p_direction = RIGHT;
                break;
            case 0x39:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( LEFT );
                p_direction = LEFT;
                break;
            case 0x3a:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( UP );
                p_direction = UP;
                break;
            case 0x3b:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( DOWN );
                p_direction = DOWN;
                break;

            case 0xd3: // Bike stuff
            case 0xd5:
                if( p_direction == LEFT || p_direction == RIGHT ) {
                    bikeJumpPlayer( p_direction );
                    stopPlayer( p_direction );
                    return;
                }
                goto NO_BREAK;
            case 0xd6:
            case 0xd4:
                if( p_direction == UP || p_direction == DOWN ) {
                    bikeJumpPlayer( p_direction );
                    stopPlayer( p_direction );
                    return;
                }
                goto NO_BREAK;

            case 0xd0:
                if( p_direction == DOWN ) {
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    reinit      = true;
                    break;
                } else {
                    if( fastBike > 9 ) goto NEXT_PASS;
                    walkPlayer( p_direction, p_fast );
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    reinit      = true;
                    break;
                }

            // Warpy stuff
            case 0x62:
                if( p_direction == RIGHT ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case 0x63:
                if( p_direction == LEFT ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case 0x64:
                if( p_direction == UP ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case 0x65:
            case 0x6d:
                if( p_direction == DOWN ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            NO_BREAK:
            default:
                // If no jump has to be done, check for other stuff
                hadjump = false;
                switch( lstBehave ) {
                case 0x20:
                case 0x48: slidePlayer( p_direction ); break;
                // These change the direction of movement
                case 0x40:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case 0x41:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case 0x42:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP );
                    p_direction = UP;
                    break;
                case 0x43:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case 0x44:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case 0x45:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case 0x46:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( UP );
                    p_direction = UP;
                    break;
                case 0x47:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case 0x50:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT, true );
                    p_direction = RIGHT;
                    break;
                case 0x51:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT, true );
                    p_direction = LEFT;
                    break;
                case 0x52:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP, true );
                    p_direction = UP;
                    break;
                case 0x53:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( DOWN, true );
                    p_direction = DOWN;
                    break;
                case 0x62:
                    if( p_direction == RIGHT ) {
                        redirectPlayer( RIGHT, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case 0x63:
                    if( p_direction == LEFT ) {
                        redirectPlayer( LEFT, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case 0x64:
                    if( p_direction == UP ) {
                        redirectPlayer( UP, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case 0x65:
                case 0x6d:
                    if( p_direction == DOWN ) {
                        redirectPlayer( DOWN, p_fast );
                        handleWarp( NO_SPECIAL );
                        return;
                    }
                    goto NEXT_PASS;

                case 0xd0:
                    if( fastBike > 9 && p_direction != DOWN ) goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                NEXT_PASS:
                default:
                    if( reinit ) {
                        _mapSprites.setFrame( _playerSprite, getFrame( p_direction ) );
                        fastBike = false;
                        return;
                    }
                    switch( newBehave ) {
                    case 0x20:
                    case 0x48: walkPlayer( p_direction, p_fast ); break;

                    // Check for warpy stuff
                    case 0x60:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( CAVE_ENTRY );
                        return;
                    case 0x61:
                    case 0x68:
                    case 0x6e:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    case 0x66:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( LAST_VISITED );
                        break;
                    case 0x69:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( DOOR );
                        break;
                    case 0x6C:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( EMERGE_WATER );
                        break;
                    case 0x29:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( TELEPORT );
                        break;

                    // These change the direction of movement
                    case 0x40:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case 0x41:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case 0x42:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case 0x43:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;

                    case 0x44:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case 0x45:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case 0x46:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case 0x47:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;

                    case 0x50:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case 0x51:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case 0x52:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case 0x53:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;
                    default: moving = false; continue;
                    }
                }
            }
            reinit = true;
            newMoveData
                = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                      .m_movedata;
            lstMoveData = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata;

            newBehave
                = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                      .m_bottombehave;
            lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                            .m_bottombehave;
        }
        walkPlayer( p_direction, p_fast );
        auto movedt = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata;
        if( movedt > 4 && movedt != 0x3c && movedt != 0xa ) {
            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ = movedt / 4;
        }
    }

    void mapDrawer::warpPlayer( warpType p_type, warpPos p_target ) {
        u8 oldMapType = u8( CUR_DATA.m_mapType );

        if( p_target.first != SAVE::SAV.getActiveFile( ).m_currentMap ) {
            SAVE::SAV.getActiveFile( ).m_mapObjectCount = 0;
        }

        loadNewBank( p_target.first );

        mapData ndata;
        FS::readMapData( p_target.first, p_target.second.m_posX / SIZE,
                         p_target.second.m_posY / SIZE, ndata );

        u8 newMapType = u8( ndata.m_mapType );

        bool entryCave
            = ( !( oldMapType & CAVE ) && ( newMapType & CAVE ) && !( newMapType & INSIDE ) );
        if( entryCave ) {
            SAVE::SAV.getActiveFile( ).m_lastCaveEntry
                = { SAVE::SAV.getActiveFile( ).m_currentMap,
                    SAVE::SAV.getActiveFile( ).m_player.m_pos };
        }
        bool exitCave
            = ( ( oldMapType & CAVE ) && !( oldMapType & INSIDE ) && !( newMapType & CAVE ) );
        if( exitCave ) { SAVE::SAV.getActiveFile( ).m_lastCaveEntry = { 255, { 0, 0, 0 } }; }
        switch( p_type ) {
        case DOOR:
            IO::fadeScreen( IO::CLEAR_DARK );
            SOUND::playSoundEffect( SFX_ENTER_DOOR );
            break;
        case SLIDING_DOOR:
            IO::fadeScreen( IO::CLEAR_DARK );
            SOUND::playSoundEffect( SFX_SLIDING_DOOR );
            break;
        case TELEPORT:
            SOUND::playSoundEffect( SFX_WARP );
            for( u8 j = 0; j < 2; ++j ) {
                redirectPlayer( RIGHT, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( UP, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( LEFT, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( DOWN, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
            }
            IO::fadeScreen( IO::CLEAR_DARK );
            break;
        case EMERGE_WATER: break;
        case CAVE_ENTRY:
            SOUND::playSoundEffect( SFX_CAVE_WARP );
            if( entryCave ) {
                IO::fadeScreen( IO::CAVE_ENTRY );
            } else if( exitCave ) {
                IO::fadeScreen( IO::CAVE_EXIT );
            } else {
                IO::fadeScreen( IO::CLEAR_DARK );
            }
            break;
        case LAST_VISITED:
        default:
            SOUND::playSoundEffect( SFX_CAVE_WARP );
            IO::fadeScreen( IO::CLEAR_DARK );
            break;
        case NO_SPECIAL: break;
        }
        swiWaitForVBlank( );
        swiWaitForVBlank( );

        if( ( ( oldMapType & INSIDE ) && ( newMapType & INSIDE ) && p_type == CAVE_ENTRY ) ) {
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            stopPlayer( DOWN );
            stopPlayer( );
        }

        SAVE::SAV.getActiveFile( ).m_player.m_pos = p_target.second;
        //        if( SAVE::SAV.getActiveFile( ).m_currentMap != p_target.first ) {
        SAVE::SAV.getActiveFile( ).m_currentMap = p_target.first;
        draw( );
        for( auto fn : _newBankCallbacks ) { fn( SAVE::SAV.getActiveFile( ).m_currentMap ); }
        auto curLocId = getCurrentLocationId( );

        if( curLocId == L_POKEMON_CENTER && oldMapType != newMapType && p_type == SLIDING_DOOR ) {
            // Register a new faint position (only if the PC was just entered)
            SAVE::SAV.getActiveFile( ).m_lastPokeCenter = p_target;
            SAVE::SAV.getActiveFile( ).m_lastPokeCenter.second.m_posY -= 4;
        }

        for( auto fn : _newLocationCallbacks ) { fn( curLocId ); }
        //        }

        // if( exitCave ) movePlayer( DOWN );
        u8 behave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                        .m_bottombehave;

        if( ( currentData( ).m_mapType & INSIDE )
            && ( SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::MACH_BIKE
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::ACRO_BIKE
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::BIKE ) ) {
            // Don't bike in buildings
            changeMoveMode( MAP::WALK );
        }

        if( ( ( oldMapType & INSIDE ) && ( newMapType & INSIDE ) && p_type == CAVE_ENTRY ) ) {
            stopPlayer( DOWN );
        }

        switch( behave ) {
        case 0x6e: walkPlayer( UP, false ); break;
        case 0x60:
        case 0x69: walkPlayer( DOWN, false ); break;

        default: break;
        }
    }

    void mapDrawer::redirectPlayer( direction p_direction, bool p_fast ) {
        // Check if redirecting is allowed
        u8 lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;

        if( lstBehave >= 0xd3 && lstBehave <= 0xd6
            && p_direction % 2 != SAVE::SAV.getActiveFile( ).m_player.m_direction % 2 )
            return;

        // Check if the player's direction changed
        if( p_direction != SAVE::SAV.getActiveFile( ).m_player.m_direction ) {
            if( !_mapSprites.getVisibility( _playerPlatSprite ) ) {
                _mapSprites.setFrame( _playerPlatSprite, getFrame( p_direction ), false );
            }
            _mapSprites.setFrame( _playerSprite, ( p_fast * 20 ) + getFrame( p_direction ) );
            SAVE::SAV.getActiveFile( ).m_player.m_direction = p_direction;
        }
    }

    void mapDrawer::standUpPlayer( direction p_direction ) {
        redirectPlayer( p_direction, false );
        bool remPlat = SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                       || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB;

        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        _mapSprites.moveSprite( _playerSprite, DOWN, 1 );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) / 3 + 12 );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        if( remPlat ) { _mapSprites.destroySprite( _playerPlatSprite ); }
        moveCamera( p_direction, true );
        _mapSprites.moveSprite( _playerSprite, DOWN, 2 );

        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        moveCamera( p_direction, true );

        _mapSprites.moveSprite( _playerSprite, DOWN, 1 );
        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }

        changeMoveMode( WALK );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }
    }
    void mapDrawer::sitDownPlayer( direction p_direction, moveMode p_newMoveMode ) {
        direction di   = ( ( p_newMoveMode == SIT ) ? direction( ( u8( p_direction ) + 2 ) % 4 )
                                                    : p_direction );
        u16       curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % SIZE;
        u16       cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % SIZE;

        if( p_newMoveMode == SURF || p_newMoveMode == ROCK_CLIMB ) {
            // Load the Pkmn
            _playerPlatSprite
                = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPR_PLATFORM );
            _mapSprites.setFrame( _playerPlatSprite, getFrame( p_direction ) );
        }

        for( u8 i = 0; i < 7; ++i ) {
            if( i == 3 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            moveCamera( di, true );
            swiWaitForVBlank( );
        }
        changeMoveMode( p_newMoveMode );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) / 3 + 12 );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( di, true );
            if( i % 2 ) swiWaitForVBlank( );
        }
        _mapSprites.moveSprite( _playerSprite, UP, 2 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );
        moveCamera( di, true );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) );
        _mapSprites.moveSprite( _playerSprite, UP, 1 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );
    }

    void mapDrawer::slidePlayer( direction p_direction ) {
        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _mapSprites.setFrame( _playerSprite, getFrame( p_direction ) );
            _mapSprites.nextFrame( _playerSprite );
        }
        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i == 8 ) { _mapSprites.currentFrame( _playerSprite ); }
            swiWaitForVBlank( );
        }
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
    }
    void mapDrawer::walkPlayer( direction p_direction, bool p_fast ) {
        if( SAVE::SAV.getActiveFile( ).m_player.m_movement != WALK ) p_fast = false;
        redirectPlayer( p_direction, p_fast );

        if( /*atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                  SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                    .m_movedata
                == 0x3c
            && */
            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ > 3
            && SAVE::SAV.getActiveFile( ).m_player.m_movement != SURF ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_1 );
        }

        if( p_fast != _playerIsFast ) {
            _playerIsFast = p_fast;
            _mapSprites.setFrame( _playerSprite,
                                  ( p_fast * PLAYER_FAST ) + getFrame( p_direction ) );
        }
        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i == 8 ) { _mapSprites.nextFrame( _playerSprite ); }
            if( ( !p_fast || i % 3 ) && !fastBike ) swiWaitForVBlank( );
            if( i % ( fastBike / 3 + 2 ) == 0 && fastBike ) swiWaitForVBlank( );
        }
        _mapSprites.drawFrame( _playerSprite, ( p_fast * PLAYER_FAST ) + getFrame( p_direction ) );
        if( ( SAVE::SAV.getActiveFile( ).m_player.m_movement & BIKE )
            || SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF ) {
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == ACRO_BIKE ) {
                fastBike = std::min( fastBike + 1, 4 );
            } else if( SAVE::SAV.getActiveFile( ).m_player.m_movement == MACH_BIKE ) {
                fastBike = std::min( fastBike + 1, 12 );
            } else if( SAVE::SAV.getActiveFile( ).m_player.m_movement == BIKE ) {
                fastBike = std::min( fastBike + 1, 8 );
            } else {
                fastBike = std::min( fastBike + 1, 6 );
            }
        } else
            fastBike = false;

        if( atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                  SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                    .m_movedata
                == 0x3c
            && ( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ <= 3
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_3 );
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) {
                _mapSprites.setPriority( _playerPlatSprite, OBJPRIORITY_3 );
            }
        } else if( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ <= 3
                /*
                atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                         SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                             + dir[ p_direction ][ 1 ] )
                           .m_movedata
                       != 0x3c
                   && atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata
                          != 0x3c */ ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_2 );
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) {
                _mapSprites.setPriority( _playerPlatSprite, OBJPRIORITY_2 );
            }
        }
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
    }

    void mapDrawer::bikeJumpPlayer( direction p_direction ) {
        SOUND::playSoundEffect( SFX_JUMP );

        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i < 3 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            if( i > 13 ) { _mapSprites.moveSprite( _playerSprite, DOWN, 3 ); }
            if( i % 2 ) swiWaitForVBlank( );
        }
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
    }

    void mapDrawer::jumpPlayer( direction p_direction ) {
        SOUND::playSoundEffect( SFX_JUMP );

        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _mapSprites.setFrame( _playerSprite, getFrame( p_direction ) );
        }
        for( u8 i = 0; i < 32; ++i ) {
            moveCamera( p_direction, true );
            if( i < 6 && i % 2 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            if( i % 8 == 0 ) { _mapSprites.nextFrame( _playerSprite ); }
            if( i > 28 && i % 2 ) { _mapSprites.moveSprite( _playerSprite, DOWN, 3 ); }
            if( i % 4 ) swiWaitForVBlank( );
        }
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) );
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
    }

    void mapDrawer::stopPlayer( ) {
        while( fastBike ) {
            fastBike = std::max( 0, (s8) fastBike - 3 );
            if( canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos,
                         SAVE::SAV.getActiveFile( ).m_player.m_direction, BIKE ) )
                movePlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction );
            fastBike = std::max( 0, (s8) fastBike - 1 );
        }
        _playerIsFast = false;
        fastBike      = false;
        _mapSprites.setFrame( _playerSprite,
                              getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
    }
    void mapDrawer::stopPlayer( direction p_direction ) {
        if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SIT
            && ( ( p_direction % 2 == SAVE::SAV.getActiveFile( ).m_player.m_direction % 2 )
                 || atom(
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                            .m_movedata
                        != atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                                     + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 0 ],
                                 SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                                     + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 1 ] )
                               .m_movedata ) ) {
            return;
        }
        fastBike = false;
        redirectPlayer( p_direction, false );
        _playerIsFast = false;
        _mapSprites.nextFrame( _playerSprite );
    }

    void mapDrawer::changeMoveMode( moveMode p_newMode ) {
        bool change  = SAVE::SAV.getActiveFile( ).m_player.m_movement != p_newMode;
        u8   basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        fastBike     = false;
        bool surfing = false;
        u8   ydif    = 0;
        SAVE::SAV.getActiveFile( ).m_player.m_movement = p_newMode;
        switch( p_newMode ) {
        case WALK: SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic; break;
        case SURF:
        case ROCK_CLIMB:
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 3;
            surfing                                      = true;
            break;
        case BIKE:
        case MACH_BIKE: SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 1; break;
        case ACRO_BIKE:
            //    SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 2;
            //    TODO
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 1;
            break;
        case SIT:
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 3;
            ydif                                         = 2;
            break;
        default: break;
        }

        u16 curx      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        if( ydif ) { _mapSprites.moveSprite( _playerSprite, UP, ydif, true ); }
        _mapSprites.setFrame( _playerSprite,
                              getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );

        if( surfing ) {
            _playerPlatSprite
                = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPR_PLATFORM );
            _mapSprites.setFrame( _playerPlatSprite,
                                  getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
            if( !change ) { _mapSprites.moveSprite( _playerSprite, UP, 3 ); }
        }

        for( auto fn : _newMoveModeCallbacks ) { fn( p_newMode ); }
    }

    bool mapDrawer::canFish( position p_start, direction p_direction ) {
        return atom( p_start.m_posX + dir[ p_direction ][ 0 ],
                     p_start.m_posY + dir[ p_direction ][ 1 ] )
                       .m_movedata
                   == 0x04
               && atom( p_start.m_posX, p_start.m_posY ).m_movedata != 0x3c;
    }
    void mapDrawer::fishPlayer( direction p_direction, u8 p_rodType ) {
        PLAYER_IS_FISHING = true;
        u16 curx          = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury          = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        u8 basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 6;
        bool surfing  = ( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF );
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        _mapSprites.moveSprite( _playerSprite, 8 * dir[ p_direction ][ 0 ],
                                8 * ( p_direction == DOWN ) );

        u8 frame = 0;
        if( p_direction == UP ) frame = 4;
        if( p_direction == DOWN ) frame = 8;

        for( u8 i = 0; i < 4; ++i ) {
            _mapSprites.drawFrame( _playerSprite, frame + i, p_direction == RIGHT, true );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        u8   rounds = rand( ) % 5;
        bool failed = false;
        NAV::printMessage( 0, MSG_NOCLOSE );
        for( u8 i = 0; i < rounds + 1; ++i ) {
            u8 cr = rand( ) % 7;
            NAV::printMessage( 0, MSG_NOCLOSE );
            for( u8 j = 0; j < cr + 5; ++j ) {
                NAV::printMessage( " .", MSG_NOCLOSE );
                for( u8 k = 0; k < 30; ++k ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    int pressed = keysDown( );
                    if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_B ) ) {
                        failed = true;
                        goto OUT;
                    }
                }
            }
            FRAME_COUNT = 0;
            NAV::printMessage( "\n", MSG_NOCLOSE );
            NAV::printMessage( GET_STRING( 8 ), MSG_NOCLOSE );
            if( FRAME_COUNT > 60 ) {
                failed = true;
                break;
            }
        }

    OUT:
        NAV::printMessage( 0, MSG_NOCLOSE );
        if( failed ) {
            NAV::printMessage( GET_STRING( 9 ) );
        } else {
            NAV::printMessage( 0 );
        }
        for( s8 i = 2; i >= 0; --i ) {
            _mapSprites.drawFrame( _playerSprite, frame + i, p_direction == RIGHT, true );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        changeMoveMode( surfing ? SURF : WALK );
        if( !failed ) {
            // Check if the player's leading Pokémon has sucion cups or sticky hold
            bool forceEncounter
                = ( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].isEgg( )
                    && ( SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                             == A_SUCTION_CUPS
                         || SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                                == A_STICKY_HOLD ) );

            // Start wild PKMN battle here
            switch( p_rodType ) {
            default:
            case 0: handleWildPkmn( OLD_ROD, forceEncounter ); break;
            case 1: handleWildPkmn( GOOD_ROD, forceEncounter ); break;
            case 2: handleWildPkmn( SUPER_ROD, forceEncounter ); break;
            }
        }

        PLAYER_IS_FISHING = false;
    }

    void mapDrawer::usePkmn( u16 p_pkmIdx, bool p_female, bool p_shiny, u8 p_forme ) {
        u8 basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 5;
        bool surfing = ( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF );

        u16 curx      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        for( u8 i = 0; i < 5; ++i ) {
            _mapSprites.drawFrame( _playerSprite, i, false, true );
            for( u8 j = 0; j < 5; ++j ) swiWaitForVBlank( );
        }
        _mapSprites.setVisibility( _playerSprite, true, false );
        IO::loadSpriteB( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_GFX, 64, 32, 64, 64, false, false, false,
                         OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 1, SPR_CIRC_GFX, 128, 32, 64, 64, 0, 0, 0, false, true,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 2, SPR_CIRC_GFX, 64, 96, 64, 64, 0, 0, 0, true, false,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 3, SPR_CIRC_GFX, 128, 96, 64, 64, 0, 0, 0, true, true,
                         false, OBJPRIORITY_1, false );

        SOUND::playCry( p_pkmIdx, p_forme );
        IO::loadPKMNSpriteB( p_pkmIdx, 80, 48, SPR_PKMN_OAM, SPR_PKMN_GFX, false, p_shiny, p_female,
                             false, false, p_forme );
        IO::updateOAM( false );
        for( u8 i = 0; i < 75; ++i ) swiWaitForVBlank( );

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden = true;
            IO::OamTop->oamBuffer[ SPR_PKMN_OAM + i ].isHidden = true;
        }
        IO::updateOAM( false );
        changeMoveMode( surfing ? SURF : WALK );
        swiWaitForVBlank( );
    }

    void mapDrawer::awardBadge( u8 p_type, u8 p_badge ) {

        if( p_type == 0
            && ( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << ( p_badge - 1 ) ) ) ) {
            // player already has this badge/symbol.
            return;
        } else if( p_type == 1 ) {
            auto sym = ( p_badge / 10 ) - 1;
            auto tp  = ( p_badge % 10 ) - 1;

            if( SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges & ( 1 << ( 7 * tp + sym ) ) ) {
                return;
            }
        }

        if( p_type == 0 ) {
            SOUND::playBGMOneshot( MOD_OS_BADGE );
        } else if( p_type == 1 ) {
            SOUND::playBGMOneshot( MOD_OS_SYMBOL );
        }

        IO::loadSpriteB( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_GFX, 64, 32, 64, 64, false, false, false,
                         OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 1, SPR_CIRC_GFX, 128, 32, 64, 64, 0, 0, 0, false, true,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 2, SPR_CIRC_GFX, 64, 96, 64, 64, 0, 0, 0, true, false,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 3, SPR_CIRC_GFX, 128, 96, 64, 64, 0, 0, 0, true, true,
                         false, OBJPRIORITY_1, false );

        if( p_type == 0 ) { // Hoenn badge
            IO::loadSpriteB( ( "ba/b" + std::to_string( p_badge ) ).c_str( ), SPR_PKMN_OAM,
                             SPR_PKMN_GFX, 96, 64, 64, 64, false, false, false, OBJPRIORITY_0,
                             false );
            SAVE::SAV.getActiveFile( ).m_lastAchievementEvent = p_badge;
            SAVE::SAV.getActiveFile( ).m_HOENN_Badges |= ( 1 << ( p_badge - 1 ) );
        } else if( p_type == 1 ) { // Frontier symbol
            IO::loadSpriteB( ( "ba/s" + std::to_string( p_badge ) ).c_str( ), SPR_PKMN_OAM,
                             SPR_PKMN_GFX, 96, 64, 64, 64, false, false, false, OBJPRIORITY_0,
                             false );

            auto sym = ( p_badge / 10 ) - 1;
            auto tp  = ( p_badge % 10 ) - 1;

            SAVE::SAV.getActiveFile( ).m_lastAchievementEvent = 10 + 2 * sym + tp;
            SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges |= ( 1 << ( 7 * tp + sym ) );
        }

        SAVE::SAV.getActiveFile( ).m_lastAchievementDate = SAVE::CURRENT_DATE;

        IO::updateOAM( false );
        for( u16 i = 0; i < 330; ++i ) swiWaitForVBlank( );

        for( u8 i = 0; i < 4; ++i ) { IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden = true; }
        IO::OamTop->oamBuffer[ SPR_PKMN_OAM ].isHidden = true;
        IO::updateOAM( false );

        char buffer[ 140 ];
        snprintf( buffer, 139, GET_STRING( 436 ), SAVE::SAV.getActiveFile( ).m_playername,
                  getBadgeName( p_type, p_badge ) );
        NAV::printMessage( buffer, MSG_INFO );
        SOUND::restartBGM( );
    }

    void mapDrawer::runPokeMart( const std::vector<std::pair<u16, u32>>& p_offeredItems,
                                 const char* p_message, bool p_allowItemSell, u8 p_paymentMethod ) {

        // Select mode (buy/sell/cancel)

        u8 curMode = 0;

        loop( ) {
            if( p_allowItemSell ) {
                curMode = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT )
                              .getResult( p_message ? p_message : GET_STRING( 470 ), MSG_NOCLOSE,
                                          { 468, 469, 387 } );
            } else {
                curMode = 0;
            }

            if( curMode == 0 ) {
                NAV::buyItem( p_offeredItems, p_paymentMethod );
            } else if( curMode == 1 ) {
                BAG::bagViewer bv = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                    BAG::bagViewer::SELL_ITEM );
                ANIMATE_MAP       = false;
                SOUND::dimVolume( );

                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                bv.run( );

                FADE_TOP_DARK( );
                FADE_SUB_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                draw( );
                ANIMATE_MAP = true;
                SOUND::restoreVolume( );
                NAV::init( );
            }
            if( !p_allowItemSell || curMode == 2 ) { break; }
        }

        NAV::init( );
    }

    u16 mapDrawer::getCurrentLocationId( ) const {
        u16            curx  = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % SIZE;
        u16            cury  = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % SIZE;
        const mapData& mdata = currentData( );

        u16 res = mdata.m_baseLocationId;
        for( u8 i = 0; i < mdata.m_extraLocationCount; ++i ) {
            if( mdata.m_extraLocations[ i ].m_left <= curx
                && mdata.m_extraLocations[ i ].m_right >= curx
                && mdata.m_extraLocations[ i ].m_top <= cury
                && mdata.m_extraLocations[ i ].m_bottom >= cury ) {

                res = mdata.m_extraLocations[ i ].m_locationId;
                break;
            }
        }
        return res;
    }
    u16 mapDrawer::getCurrentLocationId( u8 p_file ) const {
        u16 curx = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posX % SIZE;
        u16 cury = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posY % SIZE;
        u16 mapx = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posX / SIZE;
        u16 mapy = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posY / SIZE;

        mapData mdata;
        FS::readMapData( SAVE::SAV.m_saveFile[ p_file ].m_currentMap, mapx, mapy, mdata );

        u16 res = mdata.m_baseLocationId;
        for( u8 i = 0; i < mdata.m_extraLocationCount; ++i ) {
            if( mdata.m_extraLocations[ i ].m_left <= curx
                && mdata.m_extraLocations[ i ].m_right >= curx
                && mdata.m_extraLocations[ i ].m_top <= cury
                && mdata.m_extraLocations[ i ].m_bottom >= cury ) {

                res = mdata.m_extraLocations[ i ].m_locationId;
                break;
            }
        }
        return res;
    }

    void mapDrawer::constructAndAddNewMapObjects( MAP::mapData const& p_data, u8 p_mapX,
                                                  u8 p_mapY ) {
        std::vector<std::pair<u8, mapObject>> res;
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        std::set<position> eventPositions;

#ifdef DESQUID_MORE
        IO::fadeScreen( IO::UNFADE );
        NAV::printMessage( ( std::string( "constructAndAddNewMapObjects " )
                             + std::to_string( p_mapX ) + " " + std::to_string( p_mapY ) )
                               .c_str( ) );
#endif

        // check old objects and purge them if they are not visible anymore
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
            if( o.first == UNUSED_MAPOBJECT ) { continue; }

            if( dist( o.second.m_pos.m_posX, o.second.m_pos.m_posY, curx, cury ) > 24 ) {
#ifdef DESQUID_MORE
                NAV::printMessage(
                    ( std::string( "Destroying " ) + std::to_string( i ) + " "
                      + std::to_string( o.second.m_pos.m_posX ) + " " + std::to_string( curx ) + " "
                      + std::to_string( o.second.m_pos.m_posY ) + " " + std::to_string( cury ) )
                        .c_str( ) );
#endif
                _mapSprites.destroySprite( o.first, false );
            } else if( o.second.m_event.m_activateFlag
                       && !SAVE::SAV.getActiveFile( ).checkFlag(
                           o.second.m_event.m_activateFlag ) ) {
                _mapSprites.destroySprite( o.first, false );
            } else if( o.second.m_event.m_deactivateFlag
                       && SAVE::SAV.getActiveFile( ).checkFlag(
                           o.second.m_event.m_deactivateFlag ) ) {
                _mapSprites.destroySprite( o.first, false );
            } else {
                res.push_back( o );
                eventPositions.insert(
                    { o.second.m_event.m_posX, o.second.m_event.m_posY, o.second.m_event.m_posZ } );
            }
        }

        //        bool loadingNewObjectFailed = false;

        // add new objects
        for( u8 i = 0; i < p_data.m_eventCount; ++i ) {
            if( p_data.m_events[ i ].m_activateFlag
                && !SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_activateFlag ) ) {
                continue;
            }
            if( p_data.m_events[ i ].m_deactivateFlag
                && SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_deactivateFlag ) ) {
                continue;
            }

            // check if there is an event that is already loaded and has the same base
            // coordinates as the current event
            if( eventPositions.count( { p_data.m_events[ i ].m_posX, p_data.m_events[ i ].m_posY,
                                        p_data.m_events[ i ].m_posZ } ) ) {
                continue;
            }

            if( p_data.m_events[ i ].m_type == EVENT_NPC_MESSAGE
                || p_data.m_events[ i ].m_type == EVENT_NPC ) {
                if( p_data.m_events[ i ].m_data.m_npc.m_scriptType == 11
                    && SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
                if( p_data.m_events[ i ].m_data.m_npc.m_scriptType == 10
                    && !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
            }

            std::pair<u8, mapObject> cur;

            switch( p_data.m_events[ i ].m_type ) {
            case EVENT_HMOBJECT: {
                mapObject obj   = mapObject( );
                obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum    = (u16) -1;
                obj.m_movement  = NO_MOVEMENT;
                obj.m_range     = 0;
                obj.m_direction = UP;
                obj.m_event     = p_data.m_events[ i ];
#ifdef DESQUID_MORE
                NAV::printMessage( ( std::to_string( curx ) + "|" + std::to_string( cury ) + " : "
                                     + std::to_string( obj.m_pos.m_posX ) + " , "
                                     + std::to_string( obj.m_pos.m_posY ) )
                                       .c_str( ) );
#endif
                cur = { 0, obj };
                break;
            }
            case EVENT_BERRYTREE: {
                if( !SAVE::SAV.getActiveFile( ).berryIsAlive(
                        p_data.m_events[ i ].m_data.m_berryTree.m_treeIdx ) ) {
                    SAVE::SAV.getActiveFile( ).harvestBerry(
                        p_data.m_events[ i ].m_data.m_berryTree.m_treeIdx );
                    continue;
                } else {
                    // Check the growth of the specified berry tree
                    mapObject obj   = mapObject( );
                    obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                                  u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                                  p_data.m_events[ i ].m_posZ };
                    obj.m_picNum    = (u16) -1;
                    obj.m_movement  = NO_MOVEMENT;
                    obj.m_range     = 0;
                    obj.m_direction = UP;
                    obj.m_event     = p_data.m_events[ i ];

                    cur = { 0, obj };
                }
                break;
            }
            case EVENT_ITEM: {
                mapObject obj   = mapObject( );
                obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum    = (u16) -1;
                obj.m_movement  = NO_MOVEMENT;
                obj.m_range     = 0;
                obj.m_direction = UP;
                obj.m_event     = p_data.m_events[ i ];

                cur = { 0, obj };
                break;
            }
            case EVENT_TRAINER: {
                mapObject obj = mapObject( );
                obj.m_pos     = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum  = p_data.m_events[ i ].m_data.m_trainer.m_spriteId;
                obj.m_movement
                    = (MAP::moveMode) p_data.m_events[ i ].m_data.m_trainer.m_movementType;
                obj.m_range     = (MAP::moveMode) p_data.m_events[ i ].m_data.m_trainer.m_sight;
                obj.m_direction = getRandomLookDirection( obj.m_movement );
                obj.m_event     = p_data.m_events[ i ];
                obj.m_currentMovement = movement{ obj.m_direction, 0 };

                cur = { 0, obj };
                break;
            }

            case EVENT_NPC:
            case EVENT_NPC_MESSAGE: {
                mapObject obj   = mapObject( );
                obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum    = p_data.m_events[ i ].m_data.m_npc.m_spriteId;
                obj.m_movement  = (MAP::moveMode) p_data.m_events[ i ].m_data.m_npc.m_movementType;
                obj.m_range     = 0;
                obj.m_direction = getRandomLookDirection( obj.m_movement );
                obj.m_event     = p_data.m_events[ i ];
                obj.m_currentMovement = movement{ obj.m_direction, 0 };

                cur = { 0, obj };
                break;
            }
            default: continue;
            }

            loadMapObject( cur );
            /*  if( cur.first == 255 ) {
                  // this is not exactly true (e.g. for items that were already picked up)
                  loadingNewObjectFailed = true;
              }*/
            res.push_back( cur );
        }

        /*
        if( loadingNewObjectFailed ) {
            // Sort the map objects based on their distance to the camera
            std::sort(
                res.begin( ), res.end( ),
                [ & ]( const std::pair<u8, mapObject>& p_l, const std::pair<u8, mapObject>& p_r ) {
                    if( p_l.first == UNUSED_MAPOBJECT && p_r.first != UNUSED_MAPOBJECT ) {
                        return false;
                    }
                    if( p_l.first != UNUSED_MAPOBJECT && p_r.first == UNUSED_MAPOBJECT ) {
                        return true;
                    }
                    return dist( p_l.second.m_pos.m_posX, p_l.second.m_pos.m_posY, curx, cury )
                           < dist( p_r.second.m_pos.m_posX, p_r.second.m_pos.m_posY, curx, cury );
                } );
            // Check for objects that weren't loaded and try to make them replace other
            // objects that are farther away from the camera.

            auto good = res.begin( ), bad = --res.end( );
            while( good < bad ) {
                if( good->first == 255 ) {
                    // sprite wasn't loaded, try to find some sprite to unload
                    while( ( bad->first == 255 || bad->first == UNUSED_MAPOBJECT ) && good < bad ) {
                        bad--;
                    }
                    if( good >= bad ) {
                        // nothing to unload, give up
                        break;
                    }
                    _mapSprites.destroySprite( bad->first, false );
                    bad->first = 255;
                    loadMapObject( *good ); // let's just hope it works this time...
                }
                ++good;
            }
        }
        */

        SAVE::SAV.getActiveFile( ).m_mapObjectCount = res.size( );
        for( u8 i = 0; i < res.size( ); ++i ) {
            SAVE::SAV.getActiveFile( ).m_mapObjects[ i ] = res[ i ];
        }

        // force an update
        _mapSprites.update( );
    }

    void mapDrawer::destroyHMObject( u16 p_globX, u16 p_globY ) {
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

            if( o.second.m_pos.m_posX != p_globX || o.second.m_pos.m_posY != p_globY ) { continue; }
            if( o.second.m_event.m_type == MAP::EVENT_HMOBJECT ) {
                if( o.second.m_event.m_data.m_hmObject.m_hmType
                    == mapSpriteManager::SPR_ROCKSMASH ) {
                    SOUND::playSoundEffect( SFX_HM_ROCKSMASH );
                    for( u8 g = 1; g <= 4; ++g ) {
                        for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                        _mapSprites.drawFrame( o.first, g, false, true );
                    }
                    for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                    _mapSprites.destroySprite( o.first );

                    o.first                                     = 255;
                    o.second.m_event.m_data.m_hmObject.m_hmType = 0;
                }
                if( o.second.m_event.m_data.m_hmObject.m_hmType == mapSpriteManager::SPR_CUT ) {
                    SOUND::playSoundEffect( SFX_HM_CUT );
                    for( u8 g = 1; g <= 4; ++g ) {
                        for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                        _mapSprites.drawFrame( o.first, g, false, true );
                    }
                    for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                    _mapSprites.destroySprite( o.first );

                    o.first                                     = 255;
                    o.second.m_event.m_data.m_hmObject.m_hmType = 0;
                }
            }
        }
    }

    void mapDrawer::faintPlayer( ) {
        SAVE::SAV.getActiveFile( ).increaseVar( SAVE::V_NUM_FAINTED );
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        ANIMATE_MAP = false;
        videoSetMode( MODE_5_2D );
        IO::initVideo( true );
        IO::clearScreen( true, true, true );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        SOUND::setVolume( 0 );
        bgUpdate( );

        auto tgpos = SAVE::SAV.getActiveFile( ).m_lastPokeCenter;
        if( !SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first
            || SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first == 255 ) {
            SAVE::printTextAndWait( GET_STRING( 562 ) );
            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                tgpos = { 21, { 0x2b, 0x29, 3 } };
            } else {
                tgpos = { 21, { 0x31, 0x49, 3 } };
            }
        } else {
            SAVE::printTextAndWait( GET_STRING( 561 ) );
        }
        _mapSprites.setPriority( _playerSprite,
                                 SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_2 );

        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( false );
        videoSetMode( MODE_5_2D );
        bgUpdate( );

        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++i ) {
            SAVE::SAV.getActiveFile( ).getTeamPkmn( i )->heal( );
        }

        changeMoveMode( MAP::WALK );
        SOUND::setVolume( 0 );
        IO::initVideoSub( );
        IO::resetScale( true, false );
        ANIMATE_MAP = true;
        NAV::init( );
        redirectPlayer( DOWN, false );
        warpPlayer( NO_SPECIAL, tgpos );
    }
} // namespace MAP

/*
Pokémon neo
------------------------------

file        : sound.cpp
author      : Philip Wellnitz
description :

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

#include <nds.h>

#include "fs.h"
#include "locationNames.h"
#include "pokemonNames.h"
#include "sound.h"

namespace SOUND {
    u16           currentLocation = 0;
    MAP::moveMode currentMoveMode = MAP::WALK;

    u16 LAST_CRY = -1;

    void playCry( u16 p_pokemonId, u8 p_formeId, bool ) {
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
            u16 len;
            u8* cry = FS::readCry( p_pokemonId, p_formeId, len );
            if( cry == nullptr ) {
                if( p_pokemonId != 1 ) { playCry( 1 ); }
                return;
            }

            if( LAST_CRY != u16( -1 ) ) { soundKill( LAST_CRY ); }

            LAST_CRY = soundPlaySample( cry, SoundFormat_8Bit, len, 22050, 127, 64, false, 0 );
        }
#else
        (void) p_pokemonId;
        (void) p_formeId;
#endif
    }

    void playSoundEffect( u16 p_id ) {
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
            u16 len;
            u8* sfx = FS::readSFX( p_id, len );
            if( sfx == nullptr ) { return; }

            if( LAST_CRY != u16( -1 ) ) { soundKill( LAST_CRY ); }

            LAST_CRY = soundPlaySample( sfx, SoundFormat_8Bit, len, 22050, 127, 64, false, 0 );
        }
#else
        (void) p_id;
#endif
    }

    void onLocationChange( u16 p_newLocation ) {
        if( currentLocation == p_newLocation ) { return; }

        currentLocation = p_newLocation;
        if( currentMoveMode == MAP::WALK ) { playBGM( BGMforLocation( currentLocation ) ); }
    }

    void onMovementTypeChange( MAP::moveMode p_newMoveMode ) {
        if( currentMoveMode == p_newMoveMode ) { return; }

        currentMoveMode = p_newMoveMode;
        playBGM( BGMforMoveMode( currentMoveMode ) );
    }

    void restartBGM( ) {
        playBGM( BGMforMoveMode( currentMoveMode ) );
    }

    u16 BGMforMoveMode( MAP::moveMode p_moveMode ) {
        switch( p_moveMode ) {
        case MAP::DIVE:
            return MOD_DIVING;
        case MAP::SURF:
            return MOD_SURFING;
        case MAP::BIKE:
        case MAP::ACRO_BIKE:
        case MAP::MACH_BIKE:
        case MAP::BIKE_JUMP:
            return MOD_CYCLING;
        case MAP::WALK:
        case MAP::ROCK_CLIMB:
        case MAP::SIT:
        default:
            return BGMforLocation( currentLocation );
        }
    }
} // namespace SOUND

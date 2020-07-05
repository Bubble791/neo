/*
Pokémon neo
------------------------------

file        : sound.cpp
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

#include "locationNames.h"
#include "pokemonNames.h"
#include "sound.h"

namespace SOUND {
    u16           currentLocation = 0;
    MAP::moveMode currentMoveMode = MAP::WALK;

    void playCry( u16 p_pokemonId, u8 p_formeId, bool p_female ) {
        if( p_pokemonId == 41 || p_pokemonId == 42 || p_pokemonId == 169 ) {
            playSoundEffect( SFX_CRY_041 );
        } else if( p_pokemonId >= 252 && p_pokemonId <= 254 ) {
            playSoundEffect( SFX_CRY_252 );
        } else if( p_pokemonId >= 255 && p_pokemonId <= 257 ) {
            playSoundEffect( SFX_CRY_255 );
        } else if( p_pokemonId >= 258 && p_pokemonId <= 260 ) {
            playSoundEffect( SFX_CRY_258 );
        } else if( p_pokemonId == 261 || p_pokemonId == 262 ) {
            playSoundEffect( SFX_CRY_261 );
        } else if( p_pokemonId == 263 || p_pokemonId == 264 ) {
            playSoundEffect( SFX_CRY_263 );
        } else if( p_pokemonId >= 265 && p_pokemonId <= 269 ) {
            playSoundEffect( SFX_CRY_265 );
        } else if( p_pokemonId == 278 || p_pokemonId == 279 ) {
            playSoundEffect( SFX_CRY_278 );
        } else if( p_pokemonId >= 280 && p_pokemonId <= 282 ) {
            playSoundEffect( SFX_CRY_280 );
        } else {
           playSoundEffect( SFX_CRY_261 );
        }
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

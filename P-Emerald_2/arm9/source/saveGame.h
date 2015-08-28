/*
    Pok�mon Emerald 2 Version
    ------------------------------

    file        : saveGame.h
    author      : Philip Wellnitz 
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2012 - 2015
    Philip Wellnitz 

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
#include <string>
#include <vector>
#include "bag.h"
#include "box.h"
#include "pokemon.h"
#include "mapObject.h"

namespace FS {
    enum SavMod {
        _NDS,
        _GBA
    };
    extern SavMod savMod;

    struct saveGame {
        //general stuff
        wchar_t     m_playername[ 12 ];
        u8          m_isMale : 1;
        u16         m_id;
        u16         m_sid;
        union {
            u32     m_playtime;
            struct {
                u16 m_hours;
                u8  m_mins;
                u8  m_secs;

            }       m_pt;
        };
        u8          m_HOENN_Badges : 3;
        u8          m_KANTO_Badges : 3;
        u8          m_JOHTO_Badges : 3;
        u8          m_savTyp : 3;
        u8          m_inDex[ 1 + MAX_PKMN / 8 ];
        u32         m_money;

        //Bag stuff
        BAG::bag*   m_bag; //Be VERY CAREFUL when deleting savegames or when just using them!
        u8          m_lstBag;
        u8          m_lstBagItem;

        pokemon     m_pkmnTeam[ 6 ];

        //Stored Pkmn
        BOX::box*   m_storedPokemon; //And I really mean careful

        //Map stuff
        MAP::mapObject m_player;
        u8          m_currentMap;

        u8          m_EXPShareEnabled : 1;
        u8          m_evolveInBattle : 1;
        u8          m_bgIdx;

        u16         m_flags[ 500 ];

        u8          m_hasGDex : 1;
        u8          m_activatedPNav;

        bool        checkflag( u8 p_idx ) {
            return m_flags[ p_idx >> 3 ] & ( 1 << ( p_idx % 8 ) );
        }
        void        setflag( u8 p_idx, bool p_value ) {
            if( p_value != checkflag( p_idx ) )
                m_flags[ p_idx >> 3 ] ^= ( 1 << ( p_idx % 8 ) );
            return;
        }
        void        stepIncrease( );
    };

    saveGame* readSave( );
    bool writeSave( saveGame* p_saveGame );

    extern saveGame* SAV;
}
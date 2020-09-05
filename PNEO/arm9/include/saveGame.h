/*
    Pokémon neo
    ------------------------------

    file        : saveGame.h
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

#include <bit>
#include <memory>
#include <string>
#include <vector>
#include <nds.h>
#include "bag.h"
#include "box.h"
#include "mapObject.h"
#include "pokemon.h"
#include "saveOptions.h"

namespace SAVE {
    constexpr u8 MAX_SAVE_FILES       = 3;
    constexpr u8 MAX_SPECIAL_EPISODES = 0;

    constexpr u32 GOOD_MAGIC1 = 0x01234567;
    constexpr u32 GOOD_MAGIC2 = 0xFEDCBA98;
    constexpr u8  MAX_BOXES   = 35;
    constexpr u8  BERRY_SLOTS = 50;

    constexpr u16 F_MEGA_EVOLUTION   = 1;
    constexpr u16 F_NAV_OBTAINED     = 2;
    constexpr u16 F_DEX_OBTAINED     = 3;
    constexpr u16 F_NAT_DEX_OBTAINED = 4;
    constexpr u16 F_RIVAL_APPEARANCE = 5; // set if rival has appearance 1

    constexpr u16 F_TRAINER_BATTLED( u8 p_trainer ) {
        return p_trainer + 2048;
    }

    constexpr u8 V_INITIAL_PKMN_CHOICE = 0; // 1: TREECKO, 2: TORCHIC, 3: MUDKIP, 0: NONE
    constexpr u8 V_NUM_FAINTED = 1; // how often the player fainted

    extern const char* EPISODE_NAMES[ MAX_SPECIAL_EPISODES + 1 ][ LANGUAGES ];

    enum gameType {
        UNUSED   = 0,
        NORMAL   = 1,
        TRANSFER = 2,

        SPECIAL = 3
    };

    struct time {
        u16 m_hours;
        u8  m_mins;
        u8  m_secs;

        constexpr bool canIncrease( ) const {
            if( m_hours == 999 && m_mins == 59 && m_secs == 59 ) { return false; }
            return true;
        }

        auto operator<=>( const time& ) const = default;
    };

    extern time CURRENT_TIME;

    struct date {
        u8 m_year;
        u8 m_month;
        u8 m_day;

        auto operator<=>( const date& ) const = default;
    };

    extern date CURRENT_DATE;

    struct saveGame {
        struct playerInfo {
            static constexpr u8 ACHIEVEMENT_HALL_OF_FAME    = 1 << 0; // entering the hall of fame
            static constexpr u8 ACHIEVEMENT_DEX             = 1 << 1; // completing the hoenn dex
            static constexpr u8 ACHIEVEMENT_BATTLE_FRONTIER = 1 << 2; // obtain all gold symbols
            static constexpr u8 ACHIEVEMENT_CONTEST         = 1 << 3; // obtain all paintings
            static constexpr u8 ACHIEVEMENT_BERRY           = 1 << 4; // collect all vars of berries
            static constexpr u8 ACHIEVEMENT_GAME_CLEAR      = 1 << 5; // tbd

            u32 m_good1 = 0;

            gameType m_gameType;
            u8       m_achievements; // Hall of Fame / Dex / Battle Frontier /
                                     // Contest / Berries / Game clear
            u8   m_appearance;       // sprite the player currently uses
            char m_playername[ 12 ];
            u16  m_id;
            u16  m_sid;
            time m_playTime;
            date m_startDate;
            date m_lastSaveDate;
            time m_lastSaveTime;
            u16  m_lastSaveLocation;
            date m_lastAchievementDate; // Time player won last badge / entered hof
            u8   m_HOENN_Badges;
            u16  m_FRONTIER_Badges;
            u8   m_KANTO_Badges;
            u8   m_JOHTO_Badges;
            u8   m_RESERVED_Badges;
            u8   m_lastAchievementEvent; // id of a string to be printed on the second line of the
                                         // trainer's card.
            u32            m_money;
            u16            m_coins;
            u16            m_battlePoints;
            MAP::mapPlayer m_player;
            u8             m_currentMap;
            u8             m_stepCount;
            MAP::warpPos   m_lastWarp;
            u8             m_curBox;
            u16            m_lstDex;
            u16            m_lstViewedItem[ 5 ]; // Last viewed item in the bag

            ObjPriority m_playerPriority;

            u32 m_good2 = 0;

            u16         m_registeredItem; // Item registered for fast-use
            u8          m_lstBag;         // Last sub-bag used by the player
            u16         m_lstUsedItem;    // Most recently used bag item
            s16         m_repelSteps;     // Steps remaining of the currently active repel
            saveOptions m_options;        // Various options and settings
            pokemon     m_pkmnTeam[ 6 ];
            u16         m_vars[ 256 ];  // variables to be set by map scripts etc.
            u16         m_flags[ 244 ]; // flags tracking the progress of the player's adventure

            u8   m_berryTrees[ BERRY_SLOTS ];       // berry indices for each berry tree
            u8   m_berryHealth[ BERRY_SLOTS ];      // health of the berry (255 for default)
            date m_berryPlantedDate[ BERRY_SLOTS ]; // Date when the berry was planted
            time m_berryPlantedTime[ BERRY_SLOTS ]; // Time when the berry was planted

            u32 m_ashCount = 0; // ash collected on Route 113

            MAP::warpPos m_lastPokeCenter; // position where the player is teleported to if all of
                                           // their pkmn are fainted
            MAP::warpPos m_lastCaveEntry; // position where a player is teleported to if they use an
                                          // escape rope in a cave

            std::pair<u8, MAP::mapObject> m_mapObjects[ 256 ];
            u8                            m_mapObjectCount;

            u16 m_initGameItemCount = 0; // Items accessible during begin game PC check
            u16 m_initGameItems[ 5 ];

            u32 m_reserved[ 17 ]; // reserved for future things that need to be stored

            BAG::bag m_bag;

            boxPokemon m_dayCarePkmn[ 6 ]; // currently unused
            boxPokemon m_dayCareEgg[ 3 ];  // currently unused

            BOX::box m_storedPokemon[ MAX_BOXES ]; // pkmn in the storage system

            u8 m_caughtPkmn[ 125 ]; // The pkmn the player has caught
            u8 m_seenPkmn[ 125 ];   // The pkmn the player has seen

            constexpr bool hasBadgeCase( u8 p_case ) const {
                if( p_case == 0 ) { return true; }

                if( p_case == 1 ) { return !!m_FRONTIER_Badges; }

                return false;
            }

            /*
             * @brief: returns the number of stars on the trainer's card
             */
            constexpr u8 getAchievementCount( ) const {
                u8 res = 0;
                for( u8 i = 0; i < 6; ++i ) {
                    if( ( 1 << i ) & m_achievements ) { ++res; }
                }
                return res;
            }

            /*
             * @brief: Returns the number of seen pkmn.
             */
            constexpr u16 getSeenCount( ) const {
                u16 res = 0;
                for( u8 i = 0; i < 125; ++i ) {
                    // Count the number of set bits
                    res += std::popcount( m_seenPkmn[ i ] );
                }
                return res;
            }

            /*
             * @brief: Returns the number of caught pkmn.
             */
            constexpr u16 getCaughtCount( ) const {
                u16 res = 0;
                for( u8 i = 0; i < 125; ++i ) {
                    // Count the number of set bits
                    res += std::popcount( m_caughtPkmn[ i ] );
                }
                return res;
            }

            constexpr void registerSeenPkmn( u16 p_pokemon ) {
                m_seenPkmn[ p_pokemon / 8 ] |= 1 << ( p_pokemon % 8 );
            }

            constexpr void registerCaughtPkmn( u16 p_pokemon ) {
                m_caughtPkmn[ p_pokemon / 8 ] |= 1 << ( p_pokemon % 8 );
            }

            /*
             * @brief: Stores the given pkmn in the storage system at the next available
             * spot in the box the player used most recently (or any subsequent box until
             * a free space is found). Returns the box the given pkmn ended up in or -1 if
             * all boxes are full.
             */
            s8 storePkmn( const boxPokemon& p_pokemon );
            s8 storePkmn( const pokemon& p_pokemon );

            u16 countPkmn( u16 p_pkmnIdx );

            /*
             * @brief: Return the number of seen pkmn.
             */
            u16 getDexCount( );

            /*
             * @brief: Returns the box of the storage system the player used most recently.
             */
            BOX::box* getCurrentBox( );

            /*
             * @brief: Clears the save file.
             */
            void clear( );

            /*
             * @brief: Clears and initializes the save file.
             */
            void initialize( );

            /*
             * @brief: Checks whether the given flag is set.
             */
            bool checkFlag( u16 p_idx );

            /*
             * @brief: Sets the specified flag to the specified value.
             */
            void setFlag( u16 p_idx, bool p_value );

            /*
             * @brief: Returns the value of the specified variable
             */
            u16 getVar( u8 p_idx );

            /*
             * @brief: Sets the specified var to the specified value.
             */
            void setVar( u8 p_idx, u16 p_value );

            /*
             * @brief: Increases the variable; does nothing if an overflow would occur.
             */
            void increaseVar( u8 p_idx );

            /*
             * @brief: Increases the number of steps the player has walked by 1.
             * Needs to be called whenever the player makes a step in the ow.
             */
            void stepIncrease( );

            /*
             * @brief: Returns a suitable level for wild pkmn encounters for the current
             * number of badges that the player currently has.
             */
            u8 getEncounterLevel( u8 p_tier );

            /*
             * @brief: Returns the number of badges the player has obtained so far.
             */
            u8 getBadgeCount( u8 p_region = -1 );

            /*
             * @brief: Returns the number of pkmn currently in the player's party.
             */
            u8 getTeamPkmnCount( );

            /*
             * @brief: Returns the team pkmn at the specified position or nullptr if there
             * is no pkmn at that position.
             */
            inline pokemon* getTeamPkmn( u8 p_position ) {
                if( m_pkmnTeam[ p_position ].getSpecies( ) )
                    return &m_pkmnTeam[ p_position ];
                else
                    return nullptr;
            }

            /*
             * @brief: Sets the team pkmn at the specified position to the specified
             * value, overwriting any existing pkmn.
             * @param p_pokemon: The new pkmn. If nullptr, deletes the pokemon at the
             * specified position (as long as there is at least 1 other
             * non-fainted/non-egg pkmn in the party.
             * @returns: true iff the operation was successful (i.e. only returns false
             * when trying to delete the last non-fainted, non-egg pkmn)
             */
            bool setTeamPkmn( u8 p_position, pokemon* p_pokemon );
            bool setTeamPkmn( u8 p_position, boxPokemon* p_pokemon );

            /*
             * @brief: Returns the number of party pkmn that can participate in a battle.
             */
            constexpr u8 countAlivePkmn( ) const {
                u8 res = 0;
                for( u8 i = 0; i < 6; ++i ) { res += m_pkmnTeam[ i ].canBattle( ); }
                return res;
            }

            /*
             * @brief: Removes any empty spots between pkmn in the party.
             * @returns: The first slot where something changed or u8( -1 ) if nothing happened.
             */
            u8 consolidatePkmn( );

            /*
             * @brief: Returns whether the save is valid.
             */
            inline bool isGood( ) const {
                return m_good1 == GOOD_MAGIC1 && m_good2 == GOOD_MAGIC2;
            }

            /*
             * @brief: Draws the trainer's id card to the specified screen.
             * @param p_dummy: Show an empty trainer's card
             */
            void drawTrainersCard( bool p_bottom = false, bool p_dummy = false );

            /*
             * @brief: Increases the time by 1 second.
             */
            constexpr void increaseTime( ) {
                if( m_playTime.canIncrease( ) ) {
                    if( ++m_playTime.m_secs == 60 ) {
                        m_playTime.m_secs = 0;
                        if( ++m_playTime.m_mins == 60 ) {
                            m_playTime.m_mins = 0;
                            ++m_playTime.m_hours;
                        }
                    }
                }
            }

            /*
             * @brief: Checks whether the specified berry plant is still alive.
             */
            bool berryIsAlive( u8 p_berrySlot ) const;

            void waterBerry( u8 p_berrySlot );

            /*
             * @brief: Returns the number of berries the specified plant yields when
             * harvested.
             */
            u8 getBerryYield( u8 p_berrySlot ) const;

            /*
             * @brief: Returns the growth stage of the berry at the specified slot
             * @returns: 0 if no berry is present, ow stage from [1, 4].
             * (1 - just planted, 2 - sprouting, 3 - blooming, 4 - yielding)
             */
            u8 getBerryStage( u8 p_berrySlot ) const;

            inline u8 getBerry( u8 p_berrySlot ) const {
                return m_berryTrees[ p_berrySlot ];
            }

            /*
             * @brief: Removes any berry in the specified berry slot.
             */
            inline void harvestBerry( u8 p_berrySlot ) {
                m_berryHealth[ p_berrySlot ] = 0;
                m_berryTrees[ p_berrySlot ]  = 0;
            }

            /*
             * @brief: Plants the specified berry (given by itemid) in the specified slot.
             */
            void plantBerry( u8 p_berrySlot, u16 p_berry );
        } m_saveFile[ MAX_SAVE_FILES ];

        u8  m_activeFile;      // The save file the player is currently using.
        u32 m_version;         // The game version the save was created with.
        u16 m_specialEpisodes; // Unlocked special episodes

        inline std::vector<u8> getSpecialEpisodes( ) const {
            auto res = std::vector<u8>( );
            for( u8 i = 0; i < MAX_SPECIAL_EPISODES; ++i ) {
                if( m_specialEpisodes & ( 1 << i ) ) { res.push_back( i + 1 ); }
            }
#ifdef DESQUID
            res.push_back( 0 );
#endif
            return res;
        }

        /*
         * @brief: Returns whether the save is valid.
         */
        bool isGood( );

        /*
         * @brief: Clears the save file.
         */
        void clear( );

        inline playerInfo& getActiveFile( ) {
            return m_saveFile[ m_activeFile ];
        }
    };

    bool initNewGame( );

    extern saveGame SAV;
} // namespace SAVE
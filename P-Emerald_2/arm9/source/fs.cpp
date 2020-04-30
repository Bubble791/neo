/*
Pokémon neo
------------------------------

file        : fs.cpp
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

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

#include "ability.h"
#include "berry.h"
#include "defines.h"
#include "fs.h"
#include "item.h"
#include "mapSlice.h"
#include "move.h"
#include "pokemon.h"
#include "uio.h"

#include "messageBox.h"

const char ITEM_PATH[]        = "nitro:/ITEMS/";
const char PKMNDATA_PATH[]    = "nitro:/PKMNDATA/";
const char ABILITYDATA_PATH[] = "nitro:/PKMNDATA/ABILITIES/";
const char SCRIPT_PATH[]      = "nitro:/MAPS/SCRIPTS/";

const char ITEM_NAME_PATH[]    = "nitro:/DATA/ITEM_NAME/";
const char ABILITY_NAME_PATH[] = "nitro:/ABTY/ABTY_NAME/";
const char MOVE_NAME_PATH[]    = "nitro:/DATA/MOVE_NAME/";
const char POKEMON_NAME_PATH[] = "nitro:/DATA/PKMN_NAME/";
const char POKEMON_DATA_PATH[] = "nitro:/DATA/PKMN_DATA/";
const char PKMN_LEARNSET_PATH[] = "nitro:/DATA/PKMN_LEARN/";

namespace FS {
    bool exists( const char* p_path, const char* p_name ) {
        FILE* fd = open( p_path, p_name );
        if( !fd ) return false;
        fclose( fd );
        return true;
    }
    bool exists( const char* p_path, u16 p_name, bool p_unused ) {
        (void) p_unused;

        FILE* fd = open( p_path, p_name );
        if( !fd ) return false;
        fclose( fd );
        return true;
    }
    bool exists( const char* p_path, u16 p_pkmnIdx, const char* p_name ) {
        char* buffer = new char[ 100 ];
        snprintf( buffer, 99, "%s%d/%d%s.raw", p_path, p_pkmnIdx, p_pkmnIdx, p_name );
        FILE* fd = fopen( buffer, "rb" );
        delete[] buffer;
        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fclose( fd );
        return true;
    }

    FILE* open( const char* p_path, const char* p_name, const char* p_ext, const char* p_mode ) {
        char* buffer = new char[ 100 ];
        snprintf( buffer, 99, "%s%s%s", p_path, p_name, p_ext );
        auto res = fopen( buffer, p_mode );
        delete[] buffer;
        return res;
    }
    FILE* open( const char* p_path, u16 p_value, const char* p_ext, const char* p_mode ) {
        char* buffer = new char[ 100 ];
        snprintf( buffer, 99, "%s%d%s", p_path, p_value, p_ext );
        auto res = fopen( buffer, p_mode );
        delete[] buffer;
        return res;
    }
    FILE* openSplit( const char* p_path, u16 p_value, const char* p_ext,
            u16 p_maxValue, const char* p_mode ) {
        char* buffer = new char[ 100 ];
        if( p_maxValue < 10 * ITEMS_PER_DIR ) {
            snprintf( buffer, 99, "%s%d/%d%s", p_path, p_value / ITEMS_PER_DIR,
                      p_value, p_ext );
        } else if( p_maxValue < 100 * ITEMS_PER_DIR ) {
            snprintf( buffer, 99, "%s%02d/%d%s", p_path, p_value / ITEMS_PER_DIR,
                      p_value, p_ext );
        } else {
            snprintf( buffer, 99, "%s%03d/%d%s", p_path, p_value / ITEMS_PER_DIR,
                      p_value, p_ext );
        }

        auto res = fopen( buffer, p_mode );
        delete[] buffer;
        return res;
    }

    void close( FILE* p_file ) {
        fclose( p_file );
    }
    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        return fread( p_buffer, p_size, p_count, p_stream );
    }
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        return fwrite( p_buffer, p_size, p_count, p_stream );
    }

    FILE* openScript( u8 p_bank, u8 p_mapX, u8 p_mapY, u8 p_relX, u8 p_relY, u8 p_id ) {
        char buffer[ 60 ];
        snprintf( buffer, 50, "%hhu/%hhu_%hhu/%hhu_%hhu_%hhu", p_bank, p_mapY, p_mapX, p_relX,
                  p_relY, p_id );
        return open( SCRIPT_PATH, buffer, ".bin", "r" );
    }
    FILE* openScript( MAP::warpPos p_pos, u8 p_id ) {
        return openScript( p_pos.first, p_pos.second.m_posX, p_pos.second.m_posY, p_id );
    }
    FILE* openScript( u8 p_map, u16 p_globX, u16 p_globY, u8 p_id ) {
        return openScript( p_map, u8( p_globX / MAP::SIZE ), u8( p_globY / MAP::SIZE ),
                           u8( p_globX % MAP::SIZE ), u8( p_globY % MAP::SIZE ), p_id );
    }

    bool readData( const char* p_path, const char* p_name, unsigned short p_dataCnt,
                   unsigned short* p_data ) {
        FILE* fd = open( p_path, p_name );
        if( !fd ) return false;
        read( fd, p_data, sizeof( unsigned short ), p_dataCnt );
        close( fd );
        return true;
    }

    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name,
                         const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, p_tileCnt, TEMP, p_palCnt,
                                                     TEMP_PAL ) )
            return false;

        IO::copySpriteData( TEMP, p_spriteInfo->m_entry->gfxIndex, 4 * p_tileCnt, p_bottom );
        IO::copySpritePal( TEMP_PAL, p_spriteInfo->m_entry->palette, 2 * p_palCnt, p_bottom );
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize,
                          u32 p_tileCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, 12288, TEMP, 256, TEMP_PAL ) )
            return false;

        dmaCopy( TEMP, p_layer, p_tileCnt );
        if( p_bottom )
            dmaCopy( TEMP_PAL, BG_PALETTE_SUB, p_palSize );
        else
            dmaCopy( TEMP_PAL, BG_PALETTE, p_palSize );
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize,
                          u16 p_palStart, u32 p_tileCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, 12288, TEMP, 256, TEMP_PAL ) )
            return false;

        dmaCopy( TEMP, p_layer, p_tileCnt );
        if( p_bottom )
            dmaCopy( TEMP_PAL + p_palStart, BG_PALETTE_SUB + p_palStart, p_palSize );
        else
            dmaCopy( TEMP_PAL + p_palStart, BG_PALETTE + p_palStart, p_palSize );
        return true;
    }

    bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no ) {
        if( p_no == SAVE::SAV->getActiveFile( ).m_options.m_bgIdx && IO::NAV_DATA[ 0 ] ) {
            dmaCopy( IO::NAV_DATA, p_layer, 256 * 192 );
            dmaCopy( IO::NAV_DATA_PAL, BG_PAL( !SCREENS_SWAPPED ), 192 * 2 );
            return true;
        }

        if( !readData( "nitro:/PICS/NAV/", p_name, (unsigned int) 12288, IO::NAV_DATA,
                       (unsigned short) 192, IO::NAV_DATA_PAL ) )
            return false;

        dmaCopy( IO::NAV_DATA, p_layer, 256 * 192 );
        dmaCopy( IO::NAV_DATA_PAL, BG_PAL( !SCREENS_SWAPPED ), 192 * 2 );

        return true;
    }

    bool readNop( FILE* p_file, u32 p_cnt ) {
        if( p_file == 0 ) return false;
        read( p_file, 0, 1, p_cnt );
        return true;
    }

    bool readPal( FILE* p_file, MAP::palette* p_palette ) {
        if( p_file == 0 ) return false;
        read( p_file, p_palette, sizeof( u16 ) * 16, 6 );
        return true;
    }

    bool readTiles( FILE* p_file, MAP::tile* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        read( p_file, p_tileSet + p_startIdx, sizeof( MAP::tile ) * p_size, 1 );
        return true;
    }

    bool readBlocks( FILE* p_file, MAP::block* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        readNop( p_file, 4 );
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottom, 4 * sizeof( MAP::blockAtom ),
                  1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_top, 4 * sizeof( MAP::blockAtom ), 1 );
        }
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottombehave, sizeof( u8 ), 1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_topbehave, sizeof( u8 ), 1 );
        }
        return true;
    }

    u8 readAnimations( FILE* p_file, MAP::tileSet::animation* p_animations ) {
        if( !p_file ) return 0;
        u8 N;
        fread( &N, sizeof( u8 ), 1, p_file );

        if( !p_animations ) return 0;

        N = std::min( N, MAP::MAX_ANIM_PER_TILE_SET );

        for( int i = 0; i < N; ++i ) {
            auto& a = p_animations[ i ];
            fread( &a.m_tileIdx, sizeof( u16 ), 1, p_file );
            fread( &a.m_speed, sizeof( u8 ), 1, p_file );
            fread( &a.m_maxFrame, sizeof( u8 ), 1, p_file );
            a.m_acFrame = 0;
            fread( ( (u8*) a.m_tiles ), sizeof( u8 ), a.m_maxFrame * 32, p_file );
        }
        fclose( p_file );
        return N;
    }

    bool readBankData( u8 p_bank, MAP::bankInfo& p_result ) {
        char buffer[ 50 ];
        snprintf( buffer, 45, "%hhu/%hhu", p_bank, p_bank );
        FILE* f = open( MAP::MAP_PATH, buffer, ".bnk" );
        if( !f ) return false;
        read( f, &p_result.m_locationId, sizeof( u16 ), 1 );
        read( f, &p_result.m_battleBg, sizeof( u8 ), 1 );
        read( f, &p_result.m_mapType, sizeof( u8 ), 1 );
        for( u8 i = 0; i < MAX_MAP_LOCATIONS; ++i ) {
            read( f, &p_result.m_data[ i ].m_upperLeftX, sizeof( u16 ), 5 );
            read( f, &p_result.m_data[ i ].m_battleBg, sizeof( u8 ), 1 );
            read( f, &p_result.m_data[ i ].m_mapType, sizeof( u8 ), 1 );
        }
        close( f );
        return true;
    }

    [[deprecated]]
    std::string readString( FILE* p_file, bool p_new ) {
        std::string ret = "";
        int         ac;

        while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' )
            ;

        if( ac == '*' || ac == EOF ) {
            return ret;
        } else
            ret += ac;

        while( ( ac = fgetc( p_file ) ) != '*' && ac != EOF ) {
            if( ac == '|' )
                ret += (char) 136;
            else if( ac == '#' )
                ret += (char) 137;
            else
                ret += ac;
        }
        if( !p_new )
            return convertToOld( ret );
        else
            return ret;
    }

    std::string breakString( const std::string& p_string, u8 p_lineLength ) {
        std::string result = "";

        u8          acLineLength = 0;
        std::string tmp          = "";
        for( auto c : p_string ) {
            if( c == ' ' ) {
                if( acLineLength + tmp.length( ) > p_lineLength ) {
                    if( acLineLength ) {
                        result += "\n" + tmp + " ";
                        acLineLength = tmp.length( );
                        tmp          = "";
                    } else {
                        result += tmp + "\n";
                        acLineLength = 0;
                        tmp          = "";
                    }
                } else {
                    result += tmp + ' ';
                    tmp = "";
                    acLineLength += tmp.length( ) + 1;
                }
            } else
                tmp += c;
        }

        if( acLineLength + tmp.length( ) > p_lineLength && acLineLength )
            result += "\n" + tmp + " ";
        else
            result += tmp;
        return result;
    }

    std::string breakString( const std::string& p_string, IO::font* p_font, u8 p_lineLength ) {
        std::string result = "";

        u8          acLineLength = 0;
        std::string tmp          = "";
        for( auto c : p_string ) {
            if( c == ' ' ) {
                u8 tmpLen = p_font->stringWidth( tmp.c_str( ) );
                if( acLineLength + tmpLen > p_lineLength ) {
                    if( acLineLength ) {
                        result += "\n" + tmp + " ";
                        acLineLength = tmpLen;
                        tmp          = "";
                    } else {
                        result += tmp + "\n";
                        acLineLength = 0;
                        tmp          = "";
                    }
                } else {
                    result += tmp + ' ';
                    tmp = "";
                    acLineLength += tmpLen;
                }
            } else
                tmp += c;
        }

        if( acLineLength + p_font->stringWidth( tmp.c_str( ) ) > p_lineLength && acLineLength )
            result += "\n" + tmp + " ";
        else
            result += tmp;
        return result;
    }

    [[deprecated]]
    std::string convertToOld( const std::string& p_string ) {
        std::string ret = "";
        for( auto ac = p_string.begin( ); ac != p_string.end( ); ++ac ) {
            if( *ac == 'ä' )
                ret += '\x84';
            else if( *ac == 'Ä' )
                ret += '\x8E';
            else if( *ac == 'ü' )
                ret += '\x81';
            else if( *ac == 'Ü' )
                ret += '\x9A';
            else if( *ac == 'ö' )
                ret += '\x94';
            else if( *ac == 'Ö' )
                ret += '\x99';
            else if( *ac == 'ß' )
                ret += '\x9D';
            else if( *ac == 'é' )
                ret += '\x82';
            else if( *ac == '%' )
                ret += ' ';
            else if( *ac == '|' )
                ret += (char) 136;
            else if( *ac == '#' )
                ret += (char) 137;
            else if( *ac == '\r' )
                ret += "";
            else
                ret += *ac;
        }
        return ret;
    }

    std::string getLocation( u16 p_ind ) {
        if( p_ind > 5000 ) return FARAWAY_PLACE;
        FILE* f = FS::openSplit( "nitro:/LOCATIONS/", p_ind, ".data", 5000 );

        if( !f ) {
            if( p_ind > 322 && p_ind < 1000 ) return getLocation( 3002 );

            return FARAWAY_PLACE;
        }
        auto res = readString( f, true );
        fclose( f );
        return res;
    }

    std::unique_ptr<SAVE::saveGame> readSave( const char* p_path ) {
        FILE* f = FS::open( "", p_path, ".sav" );
        if( !f ) return 0;

        std::unique_ptr<SAVE::saveGame> result
            = std::unique_ptr<SAVE::saveGame>( new SAVE::saveGame( ) );
        FS::read( f, result.get( ), sizeof( SAVE::saveGame ), 1 );
        FS::close( f );
        return result;
    }

    bool writeSave( std::unique_ptr<SAVE::saveGame>& p_saveGame, const char* p_path ) {
        FILE* f = FS::open( "", p_path, ".sav", "w" );
        if( !f ) return 0;
        FS::write( f, p_saveGame.get( ), sizeof( SAVE::saveGame ), 1 );
        FS::close( f );
        return true;
    }
} // namespace FS

ability::ability( int p_abilityId ) {
    FILE* f = FS::openSplit( ABILITYDATA_PATH, p_abilityId, ".data" );

    if( !f ) return;

    m_abilityName = FS::readString( f, true );
    m_flavourText = FS::readString( f, true );
    u32 tmp;
    fscanf( f, "%lu", &tmp );
    m_type = static_cast<ability::abilityType>( tmp );
    FS::close( f );
}

bool getAbilityName( int p_abilityId, int p_language, char* p_out ) {
    FILE* f = FS::openSplit( ABILITY_NAME_PATH, p_abilityId, ".str" );
    if( !f ) return false;

    for( int i = 0; i <= p_language; ++i ) {
        fread( p_out, 1, ABILITY_NAMELENGTH, f );
    }
    fclose( f );
    return true;
}

std::string getAbilityName( int p_abilityId, int p_language ) {
    char tmpbuf[ 20 ];
    if( !getAbilityName( p_abilityId, p_language, tmpbuf ) ) {
        return "---";
    }
    return std::string( tmpbuf );
}

std::string toString( u16 p_num ) {
    char buffer[ 32 ];
    sprintf( buffer, "%hu", p_num );
    return std::string( buffer );
}

std::string getDisplayName( u16 p_pkmnId, u8 p_language, u8 p_forme ) {
    char tmpbuf[ 20 ];
    if( !getDisplayName( p_pkmnId, tmpbuf, p_language, p_forme ) ) {
        return "???";
    }
    return std::string( tmpbuf );
}
bool getDisplayName( u16 p_pkmnId, char* p_out, u8 p_language, u8 p_forme ) {
    FILE* f;
    if( p_forme ) {
        char tmpbuf[ 40 ];
        snprintf( tmpbuf, 35, "_%hhu.str", p_forme );
        f = FS::openSplit( POKEMON_NAME_PATH, p_pkmnId, tmpbuf );
    }
    if( !p_forme || !f ) {
        f = FS::openSplit( POKEMON_NAME_PATH, p_pkmnId, ".str" );
    }
    if( !f ) return false;

    for( int i = 0; i <= p_language; ++i ) {
        assert( PKMN_NAMELENGTH + ( 5 * !!p_forme ) ==
                fread( p_out, 1, PKMN_NAMELENGTH + ( 5 * !!p_forme ), f ) );
    }
    fclose( f );
    return true;
}

pkmnData    getPkmnData( const u16 p_pkmnId, const u8 p_forme ) {
    pkmnData res;
    if( getPkmnData( p_pkmnId, p_forme, &res ) ) {
        return res;
    }
    getPkmnData( 0, &res );
    return res;
}
bool        getPkmnData( const u16 p_pkmnId, pkmnData* p_out ) {
    return getPkmnData( p_pkmnId, 0, p_out );
}
bool        getPkmnData( const u16 p_pkmnId, const u8 p_forme, pkmnData* p_out ) {
    FILE* f = FS::openSplit( POKEMON_DATA_PATH, p_pkmnId, ".data" );
    if( !f ) return false;
    fread( p_out, sizeof( pkmnData ), 1, f );
    fclose( f );

    if( p_forme ) {
        char tmpbuf[ 40 ];
        snprintf( tmpbuf, 35, "_%hhu.data", p_forme );
        f = FS::openSplit( POKEMON_DATA_PATH, p_pkmnId, tmpbuf );
        if( !f ) return true;
        fread( &p_out->m_baseForme, sizeof( pkmnFormeData ), 1, f );
        fclose( f );
    }
    return true;
}


bool getAll( u16 p_pkmnId, pokemonData& p_out, u8 p_forme ) {
    FILE* f;
    if( p_forme ) {
        char buffer[ 15 ];
        snprintf( buffer, 13, "%02hu/%hu-%hhu", p_pkmnId / FS::ITEMS_PER_DIR, p_pkmnId, p_forme );
        f = FS::open( PKMNDATA_PATH, buffer, ".data" );
    }

    if( !p_forme || !f ) f = FS::openSplit( PKMNDATA_PATH, p_pkmnId, ".data" );
    if( !f ) return false;

    FS::read( f, &p_out, sizeof( pokemonData ), 1 );
    FS::close( f );
    return true;
}

void getLearnMoves( u16 p_pkmnId, u16 p_fromLevel, u16 p_toLevel, u16 p_mode, u16 p_amount,
                    u16* p_result ) {
    FILE* f = FS::openSplit( PKMN_LEARNSET_PATH, p_pkmnId, ".learnset.data" );
    if( !f )
        f  = FS::openSplit( PKMN_LEARNSET_PATH, 201, ".learnset.data" );
    if( !f )
        return;

    u16* buffer = new u16[ 700 ];
    FS::read( f, buffer, sizeof( u16 ), 699 );
    FS::close( f );
    u16 ptr = 0;

    u16 rescnt = 0;
    for( u8 i = 0; i < p_amount; ++i ) p_result[ i ] = 0;

    if( p_fromLevel > p_toLevel ) {
        std::vector<u16> reses;
        for( u16 i = 0; i <= p_fromLevel; ++i ) {
            u16 z = buffer[ ptr++ ];
            for( int j = 0; j < z; ++j ) {
                u16 g = buffer[ ptr++ ], h = buffer[ ptr++ ];
                if( i >= p_toLevel && h == (u16) p_mode && g < MAX_ATTACK ) reses.push_back( g );
            }
        }
        auto I = reses.rbegin( );
        for( u16 i = 0; i < p_amount && I != reses.rend( ); ++i, ++I ) {
            for( u16 z = 0; z < i; ++z )
                if( *I == p_result[ z ] ) {
                    --i;
                    goto N;
                }
            p_result[ i ] = *I;
        N:;
        }
        delete[] buffer;
        return;
    } else {
        for( u16 i = 0; i <= p_toLevel; ++i ) {
            u16 z = buffer[ ptr++ ];
            for( u16 j = 0; j < z; ++j ) {
                u16 g = buffer[ ptr++ ], h = buffer[ ptr++ ];
                if( i >= p_fromLevel && h == p_mode && g < MAX_ATTACK ) {
                    for( u16 k = 0; k < rescnt; ++k )
                        if( g == p_result[ k ] ) goto NEXT;
                    p_result[ rescnt ] = g;
                    if( ++rescnt == p_amount ) {
                        delete[] buffer;
                        return;
                    }
                NEXT:;
                }
            }
        }
    }
    delete[] buffer;
}
bool canLearn( u16 p_pkmnId, u16 p_moveId, u16 p_mode ) {
    FILE* f = FS::openSplit( PKMN_LEARNSET_PATH, p_pkmnId, ".learnset.data" );
    if( !f ) return false;

    u16* buffer = new u16[ 700 ];
    FS::read( f, buffer, sizeof( u16 ), 699 );
    FS::close( f );
    u16 ptr = 0;

    for( int i = 0; i <= 100; ++i ) {
        int z = buffer[ ptr++ ];
        for( int j = 0; j < z; ++j ) {
            u16 g = buffer[ ptr++ ], h = buffer[ ptr++ ];
            if( g == p_moveId && h == p_mode ) {
                delete[] buffer;
                return true;
            }
        }
    }
    delete[] buffer;
    return false;
}

u16 item::getItemId( ) {
    for( u16 i = 0; i < MAX_ITEMS; ++i )
        if( ItemList[ i ]->m_itemName == m_itemName ) return i;
    return 0;
}

bool item::load( ) {
    if( m_loaded ) return true;
    FILE* f = FS::open( ITEM_PATH, m_itemName.c_str( ), ".data" );
    if( !f ) return false;

    memset( &m_itemData, 0, sizeof( itemData ) );
    u8 tmp;
    fscanf( f, "%hhu %lu %lu\n", &tmp, &m_itemData.m_price, &m_itemData.m_itemEffect );
    m_itemData.m_itemEffectType = static_cast<item::itemEffectType>( tmp );
    strcpy( m_itemData.m_itemDisplayName, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemDescription, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemShortDescr, FS::readString( f, true ).c_str( ) );
    FS::close( f );
    return m_loaded = true;
}

bool berry::load( ) {
    if( m_loaded ) return true;
    FILE* f = FS::open( ITEM_PATH, m_itemName.c_str( ), ".data" );
    if( !f ) return false;

    memset( &m_itemData, 0, sizeof( itemData ) );
    u8 tmp;
    fscanf( f, "%hhu %lu %lu\n", &tmp, &m_itemData.m_price, &m_itemData.m_itemEffect );
    m_itemData.m_itemEffectType = static_cast<item::itemEffectType>( tmp );
    strcpy( m_itemData.m_itemDisplayName, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemDescription, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemShortDescr, FS::readString( f, true ).c_str( ) );

    memset( &m_berryData, 0, sizeof( berryData ) );
    fscanf( f, "%hu %hhu", &m_berryData.m_berrySize, &tmp );
    m_berryData.m_berryGuete = static_cast<berry::berryGueteType>( tmp );
    fscanf( f, "%hhu %hhu", &tmp, &m_berryData.m_naturalGiftStrength );
    m_berryData.m_naturalGiftType = static_cast<type>( tmp );

    for( u8 i = 0; i < 5; ++i ) fscanf( f, "%hhu", &m_berryData.m_berryTaste[ i ] );
    fscanf( f, "%hhu %hhu %hhu", &m_berryData.m_hoursPerGrowthStage, &m_berryData.m_minBerries,
            &m_berryData.m_maxBerries );
    FS::close( f );
    return m_loaded = true;
}

std::string item::getDisplayName( bool p_new ) {
    if( !m_loaded && !load( ) ) return m_itemName;
    if( p_new )
        return std::string( m_itemData.m_itemDisplayName );
    else
        return FS::convertToOld( std::string( m_itemData.m_itemDisplayName ) );
}

std::string item::getDescription( ) {
    if( !m_loaded && !load( ) ) return NO_DATA;
    return std::string( m_itemData.m_itemDescription );
}

std::string item::getShortDescription( ) {
    if( !m_loaded && !load( ) ) return NO_DATA;
    return std::string( m_itemData.m_itemShortDescr );
}

u32 item::getEffect( ) {
    if( !m_loaded && !load( ) ) return 0;
    return m_itemData.m_itemEffect;
}

item::itemEffectType item::getEffectType( ) {
    if( !m_loaded && !load( ) ) return itemEffectType::NONE;
    return m_itemData.m_itemEffectType;
}

u32 item::getPrice( ) {
    if( !m_loaded && !load( ) ) return 0;
    return m_itemData.m_price;
}

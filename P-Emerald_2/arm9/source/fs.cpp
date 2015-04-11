/*
Pok�mon Emerald 2 Version
------------------------------

file        : fs.cpp
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

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



#include <string>
#include <vector>
#include <cstdio>
#include <initializer_list>

#include "fs.h"
#include "buffer.h"
#include "ability.h"
#include "move.h"
#include "pokemon.h"
#include "item.h"
#include "berry.h"
#include "uio.h"
#include "defines.h"


const char ITEM_PATH[ ] = "nitro:/ITEMS/";
const char PKMNDATA_PATH[ ] = "nitro:/PKMNDATA/";
const char ABILITYDATA_PATH[ ] = "nitro:/PKMNDATA/ABILITIES/";

ability::ability( int p_abilityId ) {
    sprintf( buffer, "nitro:/PKMNDATA/ABILITIES/%i.data", p_abilityId );
    FILE* f = fopen( buffer, "r" );

    if( !f )
        return;

    m_abilityName = FS::readString( f, true );
    m_flavourText = FS::readString( f, true );
    fscanf( f, "%u", &( m_type ) );
    fclose( f );
}

std::wstring getWAbilityName( int p_abilityId ) {
    sprintf( buffer, "nitro:/LOCATIONS/%i.data", p_abilityId );
    FILE* f = fopen( buffer, "r" );

    if( !f )
        return L"---";
    auto ret = FS::readWString( f, false );
    fclose( f );
    return ret;
}

namespace FS {
    bool exists( const char* p_path, const char* p_name ) {
        char buffer[ 100 ];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fclose( fd );
        return true;
    }
    bool exists( const char* p_path, u16 p_pkmnIdx, const char* p_name ) {
        char buffer[ 100 ];
        sprintf( buffer, "%s%d/%d%s.raw", p_path, p_pkmnIdx, p_pkmnIdx, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fclose( fd );
        return true;
    }

    bool readData( const char* p_path, const char* p_name, const unsigned short p_dataCnt, unsigned short* p_data ) {
        char buffer[ 100 ];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fread( p_data, sizeof( unsigned short ), p_dataCnt, fd );
        fclose( fd );
        return true;
    }

    bool readData( const char* p_path, const char* p_name, const unsigned short p_dataCnt1,
                   unsigned short* p_data1, unsigned int p_dataCnt2, unsigned int* p_data2 ) {
        char buffer[ 100 ];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fread( p_data1, sizeof( unsigned short ), p_dataCnt1, fd );
        fread( p_data2, sizeof( unsigned int ), p_dataCnt2, fd );
        fclose( fd );
        return true;
    }
    bool readData( const char* p_path, const char* p_name, const unsigned int p_dataCnt1,
                   unsigned int* p_data1, unsigned short p_dataCnt2, unsigned short* p_data2 ) {
        char buffer[ 100 ];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fread( p_data1, sizeof( unsigned int ), p_dataCnt1, fd );
        fread( p_data2, sizeof( unsigned short ), p_dataCnt2, fd );
        fclose( fd );
        return true;
    }

    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom ) {
        if( !readData( p_path, p_name, (unsigned int)p_tileCnt, TEMP, (unsigned short)p_palCnt, TEMP_PAL ) )
            return false;

        if( p_bottom ) {
            dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_spriteInfo->m_entry->gfxIndex * IO::OFFSET_MULTIPLIER ], 4 * p_tileCnt );
            dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ p_spriteInfo->m_entry->palette * IO::COLORS_PER_PALETTE ], 2 * p_palCnt );
        } else {
            dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_spriteInfo->m_entry->gfxIndex * IO::OFFSET_MULTIPLIER ], 4 * p_tileCnt );
            dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ p_spriteInfo->m_entry->palette * IO::COLORS_PER_PALETTE ], 2 * p_palCnt );
        }
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize, u32 p_tileCnt, bool p_bottom ) {

        if( !readData( p_path, p_name, (unsigned int)( 12288 ), TEMP, (unsigned short)( 256 ), TEMP_PAL ) )
            return false;

        dmaCopy( TEMP, p_layer, p_tileCnt );
        if( p_bottom )
            dmaCopy( TEMP_PAL, BG_PALETTE_SUB, p_palSize );
        else
            dmaCopy( TEMP_PAL, BG_PALETTE, p_palSize );
        return true;
    }

    bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no ) {
        if( p_no == SAV->m_bgIdx && IO::NAV_DATA[ 0 ] ) {
            dmaCopy( IO::NAV_DATA, p_layer, 256 * 192 );
            dmaCopy( IO::NAV_DATA_PAL, BG_PALETTE_SUB, 256 * 2 );
            return true;
        }

        char buffer[ 100 ];
        sprintf( buffer, "%s", p_name );

        if( !readData( "nitro:/PICS/NAV/", buffer, (unsigned int)( 12288 ), IO::NAV_DATA, (unsigned short)( 256 ), IO::NAV_DATA_PAL ) )
            return false;

        dmaCopy( IO::NAV_DATA, p_layer, 256 * 192 );
        dmaCopy( IO::NAV_DATA_PAL, BG_PALETTE_SUB, 256 * 2 );

        return true;
    }

    std::string readString( FILE* p_file, bool p_new ) {
        std::string ret = "";
        char ac;

        while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' );

        if( ac == '*' ) {
            ret += '\0';
            return ret;
        } else ret += ac;

        while( ( ac = fgetc( p_file ) ) != '*' ) {
            if( !p_new ) {
                if( ac == '�' )
                    ret += '\x84';
                else if( ac == '�' )
                    ret += '\x8E';
                else if( ac == '�' )
                    ret += '\x81';
                else if( ac == '�' )
                    ret += '\x9A';
                else if( ac == '�' )
                    ret += '\x94';
                else if( ac == '�' )
                    ret += '\x99';
                else if( ac == '�' )
                    ret += '\x9D';
                else if( ac == '�' )
                    ret += '\x82';
                else if( ac == '%' )
                    ret += ' ';
                else if( ac == '|' )
                    ret += (char)136;
                else if( ac == '#' )
                    ret += (char)137;
                else if( ac == '\r' )
                    ret += "";
                else
                    ret += ac;
                continue;
            }
            if( ac == '|' )
                ret += (char)136;
            else if( ac == '#' )
                ret += (char)137;
            else
                ret += ac;
        }
        //ret += '\0';
        return ret;
    }

    std::wstring readWString( FILE* p_file, bool p_new ) {
        std::wstring ret = L"";
        char ac;
        while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' );
        if( ac == '*' ) {
            ret += L'\0';
            return ret;
        } else ret += ac;
        while( ( ac = fgetc( p_file ) ) != '*' ) {
            if( !p_new ) {
                if( ac == '�' )
                    ret += L'\x84';
                else if( ac == '�' )
                    ret += L'\x8E';
                else if( ac == '�' )
                    ret += L'\x81';
                else if( ac == '�' )
                    ret += L'\x9A';
                else if( ac == '�' )
                    ret += L'\x94';
                else if( ac == '�' )
                    ret += L'\x99';
                else if( ac == '�' )
                    ret += L'\x9D';
                else if( ac == '�' )
                    ret += L'\x82';
                else if( ac == '%' )
                    ret += L' ';
                else if( ac == '\r' )
                    ret += L"";
                else
                    ret += ac;
                continue;
            }
            if( ac == '|' )
                ret += (wchar_t)136;
            else if( ac == '#' )
                ret += (wchar_t)137;
            else
                ret += ac;
        }
        //ret += L'\0';
        return ret;
    }

    std::string breakString( const std::string& p_string, u8 p_lineLength ) {
        std::string result = "";

        u8 acLineLength = 0;
        std::string tmp = "";
        for( auto c : p_string ) {
            if( c == ' ' ) {
                if( acLineLength + tmp.length( ) > p_lineLength ) {
                    if( acLineLength ) {
                        result += "\n" + tmp + " ";
                        acLineLength = tmp.length( );
                        tmp = "";
                    } else {
                        result += tmp + "\n";
                        acLineLength = 0;
                        tmp = "";
                    }
                } else {
                    result += tmp + ' ';
                    tmp = "";
                    acLineLength += tmp.length( ) + 1;
                }
            } else
                tmp += c;
        }

        if( acLineLength + tmp.length( ) > p_lineLength  && acLineLength )
            result += "\n" + tmp + " ";
        else
            result += tmp;
        return result;
    }

    std::string breakString( const std::string& p_string, IO::font* p_font, u8 p_lineLength ) {
        std::string result = "";

        u8 acLineLength = 0;
        std::string tmp = "";
        for( auto c : p_string ) {
            if( c == ' ' ) {
                u8 tmpLen = p_font->stringWidth( tmp.c_str( ) );
                if( acLineLength + tmpLen > p_lineLength ) {
                    if( acLineLength ) {
                        result += "\n" + tmp + " ";
                        acLineLength = tmpLen;
                        tmp = "";
                    } else {
                        result += tmp + "\n";
                        acLineLength = 0;
                        tmp = "";
                    }
                } else {
                    result += tmp + ' ';
                    tmp = "";
                    acLineLength += tmpLen;
                }
            } else
                tmp += c;
        }

        if( acLineLength + p_font->stringWidth( tmp.c_str( ) ) > p_lineLength  && acLineLength )
            result += "\n" + tmp + " ";
        else
            result += tmp;
        return result;
    }

    const char* getLoc( u16 p_ind ) {
        if( p_ind > 5000 )
            return "Entfernter Ort";

        sprintf( buffer, "nitro:/LOCATIONS/%i.data", p_ind );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 ) {
            fclose( f );
            if( savMod == SavMod::_NDS && p_ind > 322 && p_ind < 1000 )
                return getLoc( 3002 );

            return "Entfernter Ort";
        }
        std::string ret = readString( f );
        fclose( f );
        return ret.c_str( );
    }
}

Type getType( u16 p_pkmnId, u16 p_type ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return UNBEKANNT;
    }
    char buf[ 12 ];
    fscanf( f, "%s", buf );
    fclose( f );
    return (Type)( buf[ p_type ] - 42 );
}
u16 getBase( u16 p_pkmnId, u16 p_base ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return 0;
    }
    char buf[ 12 ];
    fscanf( f, "%s", buf );
    fclose( f );
    return (short)buf[ 2 + p_base ];
}
u16 getCatchRate( u16 p_pkmnId ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return 0;
    }
    for( int i = 0; i < 8; ++i )
        fgetc( f );
    short buf; fscanf( f, "%hi", &buf );
    fclose( f );
    return buf;
}
const char* getDisplayName( u16 p_pkmnId ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return "???";
    }
    for( int i = 0; i < 8; ++i )
        fgetc( f );
    short buf; fscanf( f, "%hi", &buf );
    fgetc( f );
    std::string ret = FS::readString( f, true );
    fclose( f );
    ret += " ";
    ret.pop_back( );
    return ret.c_str( );
}
const wchar_t* getWDisplayName( u16 p_pkmnId ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return L"???";
    }
    for( int i = 0; i < 8; ++i )
        fgetc( f );
    short buf; fscanf( f, "%hi", &buf );
    fgetc( f );
    std::wstring ret = FS::readWString( f );
    fclose( f );
    ret += L" ";
    ret.pop_back( );
    return ret.c_str( );
}
void getWDisplayName( u16 p_pkmnId, wchar_t* p_name ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        wcscpy( p_name, L"???" );
        return;
    }
    for( int i = 0; i < 8; ++i )
        fgetc( f );
    short buf; fscanf( f, "%hi", &buf );
    fgetc( f );
    std::wstring ret = FS::readWString( f );
    fclose( f );
    wcscpy( p_name, ret.c_str( ) );
}
void getHoldItems( u16 p_pkmnId, u16* p_items ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        p_items[ 0 ] = p_items[ 1 ] = p_items[ 2 ] = p_items[ 3 ] = 0;
        return;
    }
    for( int i = 0; i < 8; ++i )
        fgetc( f );
    short buf; fscanf( f, "%hi", &buf );
    fgetc( f );
    FS::readString( f );
    p_items[ 0 ] = p_items[ 1 ] = p_items[ 2 ] = p_items[ 3 ] = 0;
    for( int i = 0; i < 4; ++i )
        fscanf( f, "%hi", &p_items[ i ] );
    fclose( f );
    return;
}
pkmnGenderType getGenderType( u16 p_pkmnId ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return (pkmnGenderType)0;
    }
    for( int i = 0; i < 8; ++i )
        fgetc( f );
    short buf; fscanf( f, "%hi", &buf );
    fgetc( f );
    FS::readString( f );
    for( int i = 0; i < 5; ++i )
        fscanf( f, "%hi", &buf );
    fclose( f );
    return (pkmnGenderType)buf;
}
const char* getDexEntry( u16 p_pkmnId ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return "Keine Daten vorhanden.";
    }
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    for( int i = 2; i < 8; ++i )
        fgetc( f );
    short s;
    fscanf( f, "%hi", &s );
    fgetc( f );
    FS::readString( f );
    for( int i = 0; i < 4; ++i )
        fscanf( f, "%hi", &s );
    fscanf( f, "%hi", &s );
    fgetc( f );
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    fgetc( f );
    fgetc( f );
    for( int i = 0; i < 6; ++i )
        fgetc( f );
    fscanf( f, "%hi", &s );
    fgetc( f );
    std::string ret = FS::readString( f );
    fclose( f );
    return ret.c_str( );
}
u16 getForme( u16 p_pkmnId, u16 p_formeId, std::string& p_retFormeName ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return p_pkmnId;
    }
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    for( int i = 2; i < 8; ++i )
        fgetc( f );
    u16 s;
    fscanf( f, "%hi", &s );
    fgetc( f );
    FS::readString( f );
    for( int i = 0; i < 4; ++i )
        fscanf( f, "%hi", &s );
    fscanf( f, "%hi", &s );
    fgetc( f );
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    fgetc( f );
    fgetc( f );
    for( int i = 0; i < 6; ++i )
        fgetc( f );
    fscanf( f, "%hi", &s );
    fgetc( f );
    FS::readString( f );
    fscanf( f, "%hi", &s );
    u16 d;
    for( int i = 0; i <= std::min( s, p_formeId ); ++i ) {
        fscanf( f, "%hi", &d );
        p_retFormeName = FS::readString( f );
    }
    //fscanf(f,"%hi",d); 
    //ret = readString(f);
    fclose( f );
    return d;
}
std::vector<u16> getAllFormes( u16 p_pkmnId ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return{ };
    }
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    for( int i = 2; i < 8; ++i )
        fgetc( f );
    u16 s;
    fscanf( f, "%hi", &s );
    fgetc( f );
    FS::readString( f );
    for( int i = 0; i < 4; ++i )
        fscanf( f, "%hi", &s );
    fscanf( f, "%hi", &s );
    fgetc( f );
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    fgetc( f );
    fgetc( f );
    for( int i = 0; i < 6; ++i )
        fgetc( f );
    fscanf( f, "%hi", &s );
    fgetc( f );
    FS::readString( f );
    fscanf( f, "%hi", &s );
    u16 d;
    std::vector<u16> res;
    for( int i = 0; i < s; ++i ) {
        fscanf( f, "%hi", &d );
        res.push_back( d );
        FS::readString( f );
    }
    //fscanf(f,"%hi",d); 
    //ret = readString(f);
    fclose( f );
    return res;
}
const char* getSpecies( u16 p_pkmnId ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return "UNBEKANNT";
    }
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    for( int i = 2; i < 8; ++i )
        fgetc( f );
    short s;
    fscanf( f, "%hi", &s );
    fgetc( f );
    FS::readString( f );
    for( int i = 0; i < 4; ++i )
        fscanf( f, "%hi", &s );
    fscanf( f, "%hi", &s );
    fgetc( f );
    for( int i = 0; i < 2; ++i )
        fgetc( f );
    fgetc( f );
    fgetc( f );
    for( int i = 0; i < 6; ++i )
        fgetc( f );
    fscanf( f, "%hi", &s );
    fgetc( f );
    FS::readString( f );
    fscanf( f, "%hi", &s );
    short d; std::string ret2;
    for( int i = 0; i < s; ++i ) {
        fscanf( f, "%hi", &d ); FS::readString( f );
    }
    fscanf( f, " " );
    ret2 = FS::readString( f, true );
    ret2 += " ";
    ret2.pop_back( );
    fclose( f );
    return ret2.c_str( );
}

void getAll( u16 p_pkmnId, pokemonData& p_out ) {
    char pt[ 100 ];
    sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );

    if( f == 0 ) {
        fclose( f );
        return;
    }
    for( int i = 0; i < 2; ++i )
        p_out.m_types[ i ] = (Type)( ( fgetc( f ) ) - 42 );
    for( int i = 2; i < 8; ++i )
        p_out.m_bases[ i - 2 ] = (short)fgetc( f );
    fscanf( f, "%hi", &p_out.m_catchrate );
    fgetc( f );
    FS::readString( f );
    for( int i = 0; i < 4; ++i )
        fscanf( f, "%hi", &p_out.m_items[ i ] );
    fscanf( f, "%hi", &p_out.m_gender );
    fgetc( f );
    for( int i = 0; i < 2; ++i )
        p_out.m_eggT[ i ] = (pkmnEggType)( ( fgetc( f ) ) - 42 );
    p_out.m_eggcyc = ( ( fgetc( f ) ) - 42 );
    p_out.m_baseFriend = ( ( fgetc( f ) ) - 42 );
    for( int i = 0; i < 6; ++i )
        p_out.m_EVYield[ i ] = ( ( fgetc( f ) ) - 42 );
    fscanf( f, "%hi", &p_out.m_EXPYield );
    fgetc( f );
    FS::readString( f );
    fscanf( f, "%hi", &p_out.m_formecnt );
    short d;
    for( int i = 0; i < p_out.m_formecnt; ++i ) {
        fscanf( f, "%hi", &d ); FS::readString( f );
    }
    fscanf( f, " " );
    FS::readString( f, true );
    fscanf( f, "%hi", &p_out.m_size );
    fscanf( f, "%hi", &p_out.m_weight );
    fscanf( f, "%hi", &p_out.m_expType );
    for( int i = 0; i < 4; ++i )
        fscanf( f, "%hu ", &p_out.m_abilities[ i ] );
    for( int i = 0; i < 7; ++i )
        for( int j = 0; j < 15; ++j )
            fscanf( f, "%hi ", &( p_out.m_evolutions[ i ].m_evolveData[ j ] ) );
    fclose( f );
    return;
}

void getLearnMoves( u16 p_pkmnId, u16 p_fromLevel, u16 p_toLevel, u16 p_mode, u16 p_amount, u16* p_result ) {
    sprintf( buffer, "%s/LEARNSETS/%d.learnset.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( buffer, "r" );
    if( !f )
        return;

    u16 rescnt = 0;

    for( u8 i = 0; i < p_amount; ++i )
        p_result[ i ] = 0;

    if( p_fromLevel > p_toLevel ) {
        std::vector<u16> reses;
        for( u16 i = 0; i <= p_fromLevel; ++i ) {
            u16 z;
            fscanf( f, "%hd", &z );
            for( int j = 0; j < z; ++j ) {
                u16 g, h;
                fscanf( f, "%hd %hd", &g, &h );
                if( i >= p_toLevel && h == (u16)p_mode && g < MAXATTACK )
                    reses.push_back( g );
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
N:
            ;
        }
        fclose( f );
        return;
    } else {
        for( u16 i = 0; i <= p_toLevel; ++i ) {
            u16 z;
            fscanf( f, "%hd", &z );
            for( u16 j = 0; j < z; ++j ) {
                u16 g, h;
                fscanf( f, "%hd %hd", &g, &h );
                if( i >= p_fromLevel && h == p_mode && g < MAXATTACK ) {
                    for( u16 z = 0; z < rescnt; ++z )
                        if( g == p_result[ z ] )
                            goto NEXT;
                    p_result[ rescnt ] = g;
                    if( ++rescnt == p_amount )
                        return;
NEXT:
                    ;
                }
            }
        }
    }
    fclose( f );
}
bool canLearn( u16 p_pkmnId, u16 p_moveId, u16 p_mode ) {
    char pt[ 150 ];
    sprintf( pt, "%s/LEARNSETS/%d.learnset.data", PKMNDATA_PATH, p_pkmnId );
    FILE* f = fopen( pt, "r" );
    if( !f )
        return false;

    for( int i = 0; i <= 100; ++i ) {
        int z; fscanf( f, "%d", &z );
        for( int j = 0; j < z; ++j ) {
            u16 g, h;
            fscanf( f, "%hd %hd", &g, &h );
            if( g == p_moveId && h == p_mode )
                return true;
        }
    }
    return false;
}

u16 item::getItemId( ) {
    for( int i = 0; i < 700; ++i )
        if( ItemList[ i ]->m_itemName == m_itemName )
            return i;
    return 0;
}

bool item::load( ) {
    //    std::stringstream FILENAME;
    //    FILENAME << ITEM_PATH << Name << ".data";
    //    FILE* f = fopen(FILENAME.str().c_str(),"r");
    //    if(f == 0)
    //        return false;
    //    //itemtype = GOODS;
    //    int ac;
    //    fscanf(f,"%i",&ac);
    //    effekt = item::EFFEKT(ac);
    //    fscanf(f,"%i\n",&(price));
    //    displayName = readString(f);
    //    dscrpt = readString(f);
    //    effekt_script = readString(f);
    //    fclose(f);
    return m_loaded = true;
}

bool berry::load( ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );

    if( f == 0 )
        return m_loaded = false;
    //itemtype = BERRIES;
    int ac;
    fscanf( f, "%i", &ac );
    //effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(price));
    fscanf( f, "%i", &ac );
    /*displayName = */FS::readString( f, false );
    /*dscrpt = "  "+ */FS::readString( f, false );
    /*effekt_script = */FS::readString( f, false );

    fscanf( f, "%hi", &( m_berrySize ) );

    fscanf( f, "%i", &ac );
    m_berryGuete = berry::berryGueteType( ac );

    fscanf( f, "%i", &ac );
    m_naturalGiftType = Type( ac );

    fscanf( f, "%hhu", &( m_naturalGiftStrength ) );

    for( int i = 0; i < 5; ++i )
        fscanf( f, "%hhu", &( m_berryTaste[ i ] ) );

    fscanf( f, "%hhu", &( m_hoursPerGrowthStage ) );

    fscanf( f, "%hhu", &( m_minBerries ) );

    fscanf( f, "%hhu\n", &( m_maxBerries ) );

    fclose( f );
    return m_loaded = true;
}

std::string item::getDescription( bool p_new ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return "Keine Daten.";
    int ac;
    fscanf( f, "%i", &ac );
    fscanf( f, "%i\n", &ac );
    std::string s = FS::readString( f, p_new );
    s = FS::readString( f, p_new );
    fclose( f );
    return s;
}

std::string item::getDisplayName( bool p_new ) {
    if( _dislayNameStatus && p_new == ( _dislayNameStatus - 1 ) )
        return _displayName;
    _dislayNameStatus = 1 + p_new;

    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return _displayName = m_itemName;
    int ac;
    fscanf( f, "%i", &ac );
    fscanf( f, "%i\n", &ac );
    _displayName = FS::readString( f, p_new );
    fclose( f );

    return _displayName;
}

item::itemEffectType item::getEffectType( ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return ( item::itemEffectType::NONE );
    int ac;
    fscanf( f, "%i", &ac );
    fclose( f );
    return ( item::itemEffectType )ac;
}

item::itemType item::getItemType( ) {
    return m_itemType;
}

u32 item::getPrice( ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return 0;
    int ac;
    fscanf( f, "%i", &ac );
    fscanf( f, "%i\n", &ac );
    fclose( f );
    return ac;
}

std::string berry::getDescription2( bool p_new ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return "Keine Daten.";
    //itemtype = BERRIES;
    int ac;
    fscanf( f, "%i", &ac );
    //effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(price));
    fscanf( f, "%i", &ac );
    /*displayName = */FS::readString( f, p_new );
    /*dscrpt = "  "+ */FS::readString( f, p_new );
    /*effekt_script = */FS::readString( f, p_new );

    fscanf( f, "%hi", &( ac ) );
    fscanf( f, "%i", &ac );
    //Guete = berry::Guete_Type(ac);
    fscanf( f, "%i", &ac );
    //BeerenKr_Type = Type(ac);
    fscanf( f, "%hhu", &( ac ) );
    for( int i = 0; i < 5; ++i )
        fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu\n", &( ac ) );

    std::string s = FS::readString( f, p_new );
    fclose( f );
    return s;
}

std::string item::getShortDescription( bool p_new ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return "Keine Daten.";
    int ac;
    fscanf( f, "%i", &ac );
    //effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(price));
    fscanf( f, "%i", &ac );
    /*displayName = */FS::readString( f, p_new );
    /*dscrpt = "  "+ */FS::readString( f, p_new );
    /*effekt_script = */FS::readString( f, p_new );

    if( fscanf( f, "%hi", &( ac ) ) == EOF )
        return "Keine Daten.";
    fscanf( f, "%i", &ac );
    //Guete = berry::Guete_Type(ac);
    fscanf( f, "%i", &ac );
    //BeerenKr_Type = Type(ac);
    fscanf( f, "%hhu", &( ac ) );
    for( int i = 0; i < 5; ++i )
        fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu\n", &( ac ) );

    FS::readString( f, p_new );
    std::string s = FS::readString( f, p_new );
    fclose( f );
    return s;
}

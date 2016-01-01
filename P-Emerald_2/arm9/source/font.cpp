/*
Pok�mon Emerald 2 Version
------------------------------

file        : font.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
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

#include <nds.h>
#include <nds/ndstypes.h>

#include "uio.h"
#include "font.h"
#include "defines.h"

namespace IO {
    font::font( u8 *p_data, u8 *p_widths, void( *p_shiftchar )( u16& val ) ) {
        _data = p_data;
        _widths = p_widths;
        _color[ 0 ] = _color[ 1 ] = _color[ 2 ]
            = _color[ 3 ] = _color[ 4 ] = RGB( 31, 31, 31 );
        _shiftchar = p_shiftchar;
    }

    void font::printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom ) {
        _shiftchar( p_ch );

        s16 putX, putY;
        u8 getX, getY;
        u32 offset = p_ch * FONT_WIDTH * FONT_HEIGHT;

        for( putY = p_y, getY = 0; putY < p_y + FONT_HEIGHT; ++putY, ++getY ) {
            for( putX = p_x, getX = 0; putX < p_x + _widths[ p_ch ]; putX += 2, getX += 2 ) {
                if( putX >= 0 && putX < SCREEN_WIDTH && putY >= 0 && putY < SCREEN_HEIGHT ) {
                    if( !p_bottom ) {
                        topScreenPlot( putX, putY, ( (u8) ( _color[ _data[ 1 + offset + ( getX + getY * FONT_WIDTH ) ] ] ) << 8 ) |
                                       (u8) ( _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ] ) );
                    } else {
                        btmScreenPlot( putX, putY, ( (u8) ( _color[ _data[ 1 + offset + ( getX + getY * FONT_WIDTH ) ] ] ) << 8 ) |
                                       (u8) ( _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ] ) );
                    }
                }
            }
        }
    }

    void font::printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += p_yDistance;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }

    void drawContinue( font p_font, u8 p_x, u8 p_y ) {
        p_font.printChar( /*'@'*/ u16( 172 ), p_x, p_y, true );
    }
    void hideContinue( u8 p_x, u8 p_y ) {
        BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
        printRectangle( p_x, p_y, p_x + 5, p_y + 9, true, false, (u8) 250 );
    }

    void font::printMBString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += FONT_HEIGHT - 2;
                putX = p_x;
                current_char++;
                continue;
            }
            if( p_string[ current_char ] == '`' ) {
                u8 c = 0;
                bool on = false;
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    IO::updateOAM( false );
                }

                loop( ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    if( ++c == 45 ) {
                        c = 0;
                        if( on )
                            hideContinue( 243, 51 );
                        else
                            drawContinue( *this, 243, 51 );
                        on = !on;
                    }
                    if( keysUp( ) & KEY_A )
                        break;
                    auto t = touchReadXY( );
                    if( t.px > 224 && t.py > 164
                        && waitForTouchUp( 224, 164 ) ) {
                        break;
                    }
                }
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    IO::updateOAM( false );
                }
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }
    void font::printMBString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == L'\n' ) {
                putY += FONT_HEIGHT - 2;
                putX = p_x;
                current_char++;
                continue;
            }
            if( p_string[ current_char ] == L'`' ) {
                u8 c = 0;
                bool on = false;
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    IO::updateOAM( false );
                }

                loop( ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    if( ++c == 45 ) {
                        c = 0;
                        if( on )
                            hideContinue( 243, 51 );
                        else
                            drawContinue( *this, 243, 51 );
                        on = !on;
                    }
                    if( keysUp( ) & KEY_A )
                        break;
                    auto t = touchReadXY( );
                    if( t.px > 224 && t.py > 164
                        && waitForTouchUp( 224, 164 ) ) {
                        break;
                    }
                }
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    IO::updateOAM( false );
                }
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }
    void font::printMBStringD( const char *p_string, s16& p_x, s16& p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += FONT_HEIGHT - 2;
                putX = p_x;
                current_char++;
                continue;
            }
            if( p_string[ current_char ] == '`' ) {
                u8 c = 0;
                bool on = false;
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    IO::updateOAM( false );
                }

                loop( ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    if( ++c == 45 ) {
                        c = 0;
                        if( on )
                            hideContinue( 243, 51 );
                        else
                            drawContinue( *this, 243, 51 );
                        on = !on;
                    }
                    if( keysUp( ) & KEY_A )
                        break;
                    auto t = touchReadXY( );
                    if( t.px > 224 && t.py > 164
                        && waitForTouchUp( 224, 164 ) ) {
                        break;
                    }
                }
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    IO::updateOAM( false );
                }
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0; i < 80 / TEXTSPEED; ++i )
                swiWaitForVBlank( );
            current_char++;
        }
        p_x = putX;
        p_y = putY;
    }
    void font::printMBStringD( const wchar_t *p_string, s16& p_x, s16& p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == L'\n' ) {
                putY += FONT_HEIGHT - 2;
                putX = p_x;
                current_char++;
                continue;
            }
            if( p_string[ current_char ] == L'`' ) {
                u8 c = 0;
                bool on = false;
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    IO::updateOAM( false );
                }

                loop( ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    if( ++c == 45 ) {
                        c = 0;
                        if( on )
                            hideContinue( 243, 51 );
                        else
                            drawContinue( *this, 243, 51 );
                        on = !on;
                    }
                    if( keysUp( ) & KEY_A )
                        break;
                    auto t = touchReadXY( );
                    if( t.px > 224 && t.py > 164
                        && waitForTouchUp( 224, 164 ) ) {
                        break;
                    }
                }
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    IO::updateOAM( false );
                }
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0; i < 80 / TEXTSPEED; ++i )
                swiWaitForVBlank( );
            current_char++;
        }
        p_x = putX;
        p_y = putY;
    }
    void font::printString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == L'\n' ) {
                putY += p_yDistance;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }
    void font::printStringD( const char *p_string, s16& p_x, s16& p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += 16;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0; i < 80 / TEXTSPEED; ++i )
                swiWaitForVBlank( );
            current_char++;
        }
        p_x = putX;
        p_y = putY;
    }
    void font::printStringD( const wchar_t *p_string, s16& p_x, s16& p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == L'\n' ) {
                putY += FONT_HEIGHT;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0; i < 80 / TEXTSPEED; ++i )
                swiWaitForVBlank( );
            current_char++;
        }
        p_x = putX;
        p_y = putY;
    }

    void font::printStringCenter( const char *p_string, bool p_bottom ) {
        s16 x = SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2;
        s16 y = SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2;

        printString( p_string, x, y, p_bottom );
    }
    void font::printStringCenterD( const char *p_string, bool p_bottom ) {
        s16 x = ( SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2 );
        s16 y = ( SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2 );

        printStringD( p_string, x, y, p_bottom );
    }
    void font::printStringCenter( const wchar_t *p_string, bool p_bottom ) {
        s16 x = SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2;
        s16 y = SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2;

        printString( p_string, x, y, p_bottom );
    }
    void font::printStringCenterD( const wchar_t *p_string, bool p_bottom ) {
        s16 x = ( SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2 );
        s16 y = ( SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2 );

        printStringD( p_string, x, y, p_bottom );
    }

    void font::printNumber( s32 p_num, s16 p_x, s16 p_y, bool p_bottom ) {
        char numstring[ 10 ] = "";
        u32 number = p_num, quotient = 1, remainder = 0;
        char remainder_str[ 3 ] = "";
        u32 current_char = 0, current_char2 = 0;
        static char string[ 100 ];

        if( number == 0 ) {
            strcpy( string, "0" );
        } else {
            while( quotient != 0 ) {
                remainder = number % 10;
                quotient = number / 10;
                remainder_str[ 0 ] = remainder + 48;
                remainder_str[ 1 ] = '\0';
                strcat( numstring, remainder_str );
                number = quotient;
            }

            current_char = strlen( numstring ) - 1;
            while( current_char != 0 ) {
                string[ current_char2 ] = numstring[ current_char ];
                current_char--;
                current_char2++;
            }
            string[ current_char2 ] = numstring[ current_char ];
            string[ current_char2 + 1 ] = '\0';
        }

        printString( string, p_x, p_y, p_bottom );
    }


    u32 font::stringWidth( const char *p_string ) const {
        u32 current_char = 0;
        u32 width = 0;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' )
                break;
            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            width += _widths[ c ];

            current_char++;
        }

        return width;
    }
    u32 font::stringWidth( const wchar_t *p_string ) const {
        u32 current_char = 0;
        u32 width = 0;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' )
                break;
            width += _widths[ (u8) p_string[ current_char ] ] + 1;

            current_char++;
        }

        return width - 1;
    }
}
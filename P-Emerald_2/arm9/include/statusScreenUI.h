/*
Pokémon neo
------------------------------

file        : statusScreenUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include "pokemon.h"

namespace STS {
    class statusScreenUI {
      private:
        u8   _currentPage;
        bool _allowKeyUp;
        bool _allowKeyDown;

        u16 initTopScreen( bool p_bottom = false );
        u16 initBottomScreen( bool p_bottom = true );

        /*
         * @brief Writes the given string to the specified line to the text field.
         */
        void writeLineTop( const char* p_string, u8 p_line, u8 p_color = 252,
                           bool p_bottom = false );

      public:
        /*
         * @brief Initializes the status screen UI. Destroys anything that was previously on the
         * screen.
         * @param p_initialPage: initially selected page
         */
        void init( u8 p_initialPage, bool p_allowKeyUp = true, bool p_allowKeyDown = true );

        /*
         * @brief Returns the number of different pages.
         */
        constexpr u8 getPageCount( ) {
            return 3;
        }

        /*
         * @brief Draws the specified info page for the given pokemon.
         */
        void draw( pokemon* p_pokemon, u8 p_page );

        /*
         * @brief Draws the p_frame-th frame. Needs to be called every frame.
         */
        void animate( u8 p_frame );
    };
} // namespace STS
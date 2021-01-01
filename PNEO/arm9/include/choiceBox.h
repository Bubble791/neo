/*
Pokémon neo
------------------------------

file        : choiceBox.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#pragma once

#include <nds.h>
#include "defines.h"
#include "pokemon.h"
#include "uio.h"

namespace IO {
    /*
     * @brief: Makes the player select one out of a set of different choices.
     */
    class choiceBox {
      public:
        static void DEFAULT_TICK( ) {
        }
        typedef u8                 selection;
        static constexpr selection DISABLED_CHOICE  = 253;
        static constexpr selection EXIT_CHOICE      = 254;
        static constexpr selection BACK_CHOICE      = 255;
        static constexpr selection NEXT_PAGE_CHOICE = 250;
        static constexpr selection PREV_PAGE_CHOICE = 251;

        enum mode {
            MODE_UP_DOWN            = 0, // A single column of choices (per page)
            MODE_UP_DOWN_LEFT_RIGHT = 1, // Two columns of choices (per page) in total
            MODE_LEFT_RIGHT         = 2, // A single row of choices (no page switch)
            MODE_UP_DOWN_LEFT_RIGHT_CANCEL
            = 3, // Two columns of 2 choices and an extra option centered below
        };

      private:
        mode _mode;

      public:
        choiceBox( mode p_mode = MODE_UP_DOWN ) : _mode( p_mode ) {
        }
        /*
         * @brief: Opens a choiceBox and returns the player's selection.
         * @param p_drawFunction: Callback used to draw a page of the choiceBox; should
         * return all choices visible for the specified page only.
         * @param p_selectFunction: Callback used when the player selects a choice.
         * @returns: A choice from the choices generated by p_drawFunction (but never
         * NEXT_PAGE_CHOICE or PREV_PAGE_CHOICE)
         */
        selection getResult(
            std::function<std::vector<std::pair<inputTarget, selection>>( u8 )> p_drawFunction,
            std::function<void( selection )> p_selectFunction, selection p_initialSelection = 0,
            std::function<void( )> p_tick = DEFAULT_TICK, u8 p_initialPage = 0 );

        selection getResult( const char* p_message, style p_style,
                             const std::vector<u16>& p_choices );
    };
} // namespace IO

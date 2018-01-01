/*
Pok�mon Emerald 2 Version
------------------------------

file        : moveChoiceBox.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2018
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
#include "pokemon.h"

namespace IO {
    class moveChoiceBox {
      public:
        moveChoiceBox( pokemon p_pokemon, u16 p_moveToLearn, bool p_drawSub = true );
        moveChoiceBox( pokemon p_pokemon, bool p_drawSub = true );

        void draw( u8 p_pressedIdx );
        int  getResult( bool p_backButton = false, bool p_drawSub = true,
                        u8 p_initialSelection = 0 );
        void kill( );

      private:
        u8      _selectedIdx;
        pokemon _pokemon;
        u16     _moveToLearn;
        bool    _drawSub;

        void updateButtons( bool p_backButton );
    };
} // namespace IO

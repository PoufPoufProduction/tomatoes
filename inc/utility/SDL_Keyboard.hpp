/*
Copyright 2011 JohannC

This file is part of TOMATOES.

TOMATOES is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

TOMATOES is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
TOMATOES.  If not, see http://www.gnu.org/licenses/
*/

#ifndef UTILITY_SDL_KEYBOARD_HPP_
#define UTILITY_SDL_KEYBOARD_HPP_

#include <SDL.h>

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

namespace utility
{

class SDL_Keyboard
{
private:
    class Key
    {
    public:
        Uint32      last;           // Last event tick
        bool        first;          // Is it a first event ?
        int         repeat[2];      // The repeat timers (first, next)
        bool        down;           // Is the key down
        Key():last(0), first(false), down(false) { repeat[0] = 120; repeat[1] = 50; }
    };

private:
    Uint8 *     keystate;           // The key states array
    Key *       keystateEx;         // The key states enhanced

public:
    SDL_Keyboard();
    ~SDL_Keyboard();

    void    SDL_EnableKeyRepeat(int _delay, int _interval, int _key = -1);
    void    onEvent(SDL_Event & _event);
    Uint8 * SDL_GetKeyState(int * _numkeys);
    Uint8 * SDL_GetKeyStateLight(int * _numkeys);

};


}

#endif


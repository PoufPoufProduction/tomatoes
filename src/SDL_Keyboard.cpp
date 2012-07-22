/*
Copyright 2012 JohannC

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

#include <utility/SDL_Keyboard.hpp>

using namespace utility;

SDL_Keyboard::SDL_Keyboard()
{
    keystate = new Uint8[SDLK_LAST];
    keystateEx = new Key[SDLK_LAST];
}

SDL_Keyboard::~SDL_Keyboard()
{
    delete [] keystate;
    delete [] keystateEx;
}

void SDL_Keyboard::onEvent(SDL_Event & _event)
{
    if (_event.type == SDL_KEYDOWN ) {
        keystateEx[_event.key.keysym.sym].last = 0;
        keystateEx[_event.key.keysym.sym].first = true;
        keystateEx[_event.key.keysym.sym].down = true;
    }
    else if (_event.type == SDL_KEYUP ) {
        keystateEx[_event.key.keysym.sym].last = 0;
        keystateEx[_event.key.keysym.sym].first = false;
        keystateEx[_event.key.keysym.sym].down = false;
    }
}

void SDL_Keyboard::SDL_EnableKeyRepeat(int _delay, int _interval, int _key)
{
    if (_key>=SDLK_LAST) { _key = -1; }
    int first = (_key<0)?0:_key;
    int last = (_key<0)?SDLK_LAST:_key+1;

    for (int i=first; i<last; i++)
    {
        keystateEx[i].repeat[0] = _delay;
        keystateEx[i].repeat[1] = _interval;
    }
}


Uint8 * SDL_Keyboard::SDL_GetKeyStateLight(int * _numkeys UNUSED)
{
    for (int i=0; i<SDLK_LAST; i++) { keystate[i] = (keystateEx[i].down)?1:0; }
    return keystate;
}

Uint8 * SDL_Keyboard::SDL_GetKeyState(int * _numkeys UNUSED)
{
    Uint32 vTick = SDL_GetTicks();
    for (int i=0; i<SDLK_LAST; i++)
    {
        keystate[i] = 0;

        if (keystateEx[i].last)
        {
            if (keystateEx[i].repeat[0]>0 && keystateEx[i].last+keystateEx[i].repeat[keystateEx[i].first?0:1]<vTick)
            {
                keystateEx[i].last = vTick;
                keystateEx[i].first = false;
                keystate[i] = 1;
            }
        }
        else
        {
            if (keystateEx[i].first)
            {
                keystateEx[i].last = vTick;
                keystate[i] = 1;
            }
        }


    }

    return keystate;

}


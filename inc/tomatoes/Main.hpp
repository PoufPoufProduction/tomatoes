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

#ifndef TOMATOES_MAIN_HPP_
#define TOMATOES_MAIN_HPP_

#include <string>
#include <SDL.h>

#include <utility/Configuration.hpp>
#include <utility/SDL_Keyboard.hpp>
#include <tomatoes/Menu.hpp>

namespace tomatoes
{

class Game;

class Tomatoes
{

private:
    utility::SDL_Keyboard       keyboard;                       // The SDL keyboard handler

    SDL_Surface*                screen;                         // The main screen
    int                         screenSize[2];                  // The screen size
    int                         screenDepth;                    // The screen depth

    // -------------- TOMATOES COMPONENTS -----------------------------
    utility::Configuration      settings;                       // The tomatoes settings (volume, bestscores, etc.)
    Menu                        menu;                           // The menu handler
    Game *                      game;                           // The game handler

    // -------------- PLAYER SETTINGS ---------------------------------
    bool                        twoPlayers;                     // Is a two players game ?
    int                         playerId[2];                    // The player id
    int                         level;                          // The current level
    const int                   nbLevels;                       // The number of level


public:
    Tomatoes():
        screenDepth(32), settings("tomatoes"), menu(this), game(0), twoPlayers(false), nbLevels(1)
    {
        screenSize[0] = 480;
        screenSize[1] = 640;
        playerId[0] = 0;
        playerId[1] = 1;
        keyboard.SDL_EnableKeyRepeat(500,200);
    }

    /** Accessors */
    Menu &                      getMenu()                   { return menu; }
    Game *                      getGame()                   { return game; }
    utility::Configuration &    getSettings()               { return settings; }
    utility::SDL_Keyboard &     getKeyboard()               { return keyboard; }

    bool                        multi() const               { return twoPlayers; }
    int                         getPlayerId(int _p) const   { return ((_p==1)||(_p==0))?playerId[_p]:0; }
    int                         getLevel() const            { return level; }
    void                        setMulti(bool _value)       { twoPlayers = _value; }
    void                        setPlayerId(int _p, int _v) { if ((_p==1)||(_p==0)) { playerId[_p]=_v; } }
    void                        resetLevel()                { level = 0; }
    bool                        incLevel()                  { return (level++<nbLevels); }

    void                        createGame(int _level = 1);
    bool                        launchGame();
    void                        run();
};
}

#endif

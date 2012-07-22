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

#ifndef TOMATOES_MENU_HPP_
#define TOMATOES_MENU_HPP_

#include <splashouille/Engine.hpp>

namespace splashouille
{
class Animation;
class Object;
class Engine;
}

namespace tomatoes
{
class Tomatoes;


/**
 * Class menu handles all the menu interactions
 */
class Menu : public splashouille::Engine::Listener
{
private:
    typedef enum
    {
        level0              = 0,                // NOT INTERACTIVE LEVEL
        level1              = 10,               // MAIN MENU SELECTOR (PLAY,SCORES,CREDIT,ETC.)
        level2              = 20,               // PLAYER SELECTION
        level3              = 30,               // LAUNCH THE GAME
        level6              = 60,               // QUIT
        level10             = 100,              // BEST SCORES OVERVIEW
        level13             = 130,              // CREDITS
        level14             = 140,              // OPTIONS
        last                = 200
    } state;
    typedef void (Menu::*p_onFrame)(int _frame, int _timeStampInMilliSeconds);

private:
    Tomatoes *                  tomatoes;                   // The tomatoes parent
    splashouille::Engine *      engine;                     // The menu splashouille engine
    splashouille::Object *      o_level1[6];                // The level1 menu: 1 Player, 2 Players, Scores, Credit,etc.
    splashouille::Object *      o_options[2];               // The options
    splashouille::Object *      o_audio[5];                 // The audio values
    splashouille::Animation *   a_player[2];                // The players selection animation
    int                         level1Value;                // The selected level1 menu
    p_onFrame                   onFrameMethods[last];       // onFrame callbacks regarding the state
    int                         option;                     // The selected options
    bool                        playerReady[2];             // Are the players ready ?

    /** Update the different menu parts */
    void                        updateLevel1(int _level1, int _timeStampInMilliSeconds);
    void                        updateOption(int _option = -1, int _value = -1, int _timeStampInMilliSeconds =-1);
    void                        updateBest(libconfig::Setting & _root);

    /** Menu event handling */
    void                        onLevel1 (int _frame, int _timeStampInMilliSeconds);
    void                        onLevel2 (int _frame, int _timeStampInMilliSeconds);
    void                        onLevel3 (int _frame, int _timeStampInMilliSeconds);
    void                        onLevel6 (int _frame, int _timeStampInMilliSeconds);
    void                        onLevel10(int _frame, int _timeStampInMilliSeconds);
    void                        onLevel13(int _frame, int _timeStampInMilliSeconds);
    void                        onLevel14(int _frame, int _timeStampInMilliSeconds);

public:
    Menu(Tomatoes * _tomatoes):tomatoes(_tomatoes), engine(0), level1Value(0), option(0) {}

    /** Init the class menu */
    bool run(libconfig::Config * _configuration, SDL_Surface * _screen);

    /**
     * Callback called each second
     * @param _frame is the frame number from the beginning of the animation
     * @param _frameSec is the number of frames played during the last second
     * @param _second is the current second
     */
    void onSecond(int _frame, int _frameSec, int _second);

    /**
     * Callback called on each frame
     * @param _frame is the frame number from the beginning of the animation
     * @param _timeStampInMilliSeconds is the current timestamp
     */
    void onFrame(int _frame, int _timeStampInMilliSeconds);

    /**
     * The onEvent Callback
     * @param _event is the SDL_Event
     */
    bool onEvent(SDL_Event & _event, int _timeStampInMilliSeconds);

    /**
     * Callback on the quit event
     * @return true if the event is consumed
     */
    bool onStop() { return true; }
};

}


#endif

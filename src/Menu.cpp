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

#include <splashouille/Defines.hpp>
#include <splashouille/Engine.hpp>
#include <splashouille/Library.hpp>
#include <splashouille/Style.hpp>
#include <splashouille/Fashion.hpp>
#include <splashouille/Crowd.hpp>
#include <splashouille/Sound.hpp>
#include <splashouille/Image.hpp>

#include <tomatoes/Main.hpp>
#include <tomatoes/Game.hpp>

#include <iostream>
#include <sstream>

using namespace tomatoes;

/**
 * Initialize the menu  handler
 * @return true if every objects are found (todo)
 */
bool Menu::run(libconfig::Config * _configuration, SDL_Surface * _screen)
{
    bool rc = true;

    // CREATE AND CONFIGURE THE ENGINE
    engine = splashouille::Engine::createEngine();
    rc = engine->import(_configuration);

    // GET ELEMENTS
    o_level1[0]     = engine->getLibrary()->getObjectById("player");
    o_level1[1]     = engine->getLibrary()->getObjectById("players");
    o_level1[2]     = engine->getLibrary()->getObjectById("scores");
    o_level1[3]     = engine->getLibrary()->getObjectById("options");
    o_level1[4]     = engine->getLibrary()->getObjectById("credit");
    o_level1[5]     = engine->getLibrary()->getObjectById("quit");
    o_options[0]    = engine->getLibrary()->getObjectById("audio");
    o_audio[0]      = engine->getLibrary()->getObjectById("audio0");
    o_audio[1]      = engine->getLibrary()->getObjectById("audio1");
    o_audio[2]      = engine->getLibrary()->getObjectById("audio2");
    o_audio[3]      = engine->getLibrary()->getObjectById("audio3");
    o_audio[4]      = engine->getLibrary()->getObjectById("audio4");
    a_player[0]     = engine->getLibrary()->getAnimationById("a_level2_player1");
    a_player[1]     = engine->getLibrary()->getAnimationById("a_level2_player2");

    o_level1[0]->changeFashion("selected");


    // HANDLE CALLBACKS
    for (int i=0; i<last; i++) { onFrameMethods[i] = 0; }
    onFrameMethods[level1]      = &Menu::onLevel1;
    onFrameMethods[level2]      = &Menu::onLevel2;
    onFrameMethods[level3]      = &Menu::onLevel3;
    onFrameMethods[level6]      = &Menu::onLevel6;
    onFrameMethods[level10]     = &Menu::onLevel10;
    onFrameMethods[level13]     = &Menu::onLevel13;
    onFrameMethods[level14]     = &Menu::onLevel14;

    //engine->setDebug();
    engine->addListener(this);

    updateOption();

    // RUN THE ENGINE
    if (rc) { engine->run(_screen); }

    // DELETE THE ENGINE
    splashouille::Engine::deleteEngine(engine);


    return rc;
}

/**
 * Level 6 callback : end of the menu engine
 */
void Menu::onLevel6(int _frame UNUSED, int _t UNUSED)
{
    engine->stop();
}

/**
 * Callback called each second
 * @param _frame is the frame number from the beginning of the animation
 * @param _frameSec is the number of frames played during the last second
 * @param _second is the current second
 */
void Menu::onSecond(int _frame UNUSED, int _frameSec UNUSED, int _second UNUSED)
{
}

/**
 * Callback called on each frame
 * @param _frame is the frame number from the beginning of the animation
 * @param _timeStampInMilliSeconds is the current timestamp
 */
void Menu::onFrame(int _frame, int _timeStampInMilliSeconds)
{
    p_onFrame  method = onFrameMethods[engine->getState()];
    if (method) { (*this.*method)(_frame, _timeStampInMilliSeconds); }
}

/**
 * The onEvent Callback
 * @param _event is the SDL_Event
 */
bool Menu::onEvent(SDL_Event & _event, int _timeStampInMilliSeconds UNUSED)
{
    tomatoes->getKeyboard().onEvent(_event);
    return true;
}


/**
 * Update the best scores overview
 * @param _root is the root of the settings configuration file
 */
void Menu::updateBest(libconfig::Setting & _root)
{
    // UPDATE THE 10 BEST SCORES
    for (int i=0; i<10; i++)
    {
        libconfig::Setting &    setting = _root["score"][i];
        std::string             player = setting["player"];
        int                     score = setting["score"];

        // UPDATE THE PLAYER NAME
        for (int j=0; j<3; j++)
        {
            char vId[32];
            snprintf(vId, 32, "player%d%d", i, j);
            splashouille::Image* l = engine->getLibrary()->getImageById(vId);
            if (l) { l->setTileIndex(player[j]); }
        }

        // UPDATE THE SCORE VALUE
        for (int j=3; j>=0; j--)
        {
            char vId[32];
            snprintf(vId, 32, "score%d%d", i, j);
            splashouille::Image* l = engine->getLibrary()->getImageById(vId);
            if (l) { l->setTileIndex(score?('0'+score%10):' '); }
            score = score/10;
        }
    }
}

/**
 * Change the level1 selection
 * @param _level1 is the new level1 selected menu
 * @param _timeStampInMilliSeconds is the unused timestamp
 */
void Menu::updateLevel1(int _level1, int _timeStampInMilliSeconds UNUSED)
{
    if (_level1>=0 && _level1<6)
    {
        if (level1!=_level1) { o_level1[level1Value]->changeFashion("normal"); }
        level1Value = _level1;
        o_level1[level1Value]->changeFashion("selected");
    }
}


/**
 * Change the option selection
 * @param _level1 is the new level1 selected menu
 * @param _timeStampInMilliSeconds is the unused timestamp
 */
void Menu::updateOption(int _option, int _value, int _timeStampInMilliSeconds UNUSED)
{
    if (_option<1)
    {
        // THE OPTION HIGHLIGHT
        bool update = true;
        if (_option>=0)
        {
            if (option!=_option) { o_options[option]->changeFashion("normal"); } else { update = false; }
            option = _option;
        }
        if (update) { o_options[option]->changeFashion("selected"); }

        // THE VALUE HIGHLIGHT
        int a = tomatoes->getSettings().getRoot()["audio"];
        update = true;

        if (_value>=0)
        {
            switch(option)
            {
            case 0:
                if (a!=_value) { o_audio[a]->changeFashion("normal"); } else { update=false; }
                a = _value;
                break;
            }
        }

        if (update)
        {
            splashouille::Sound::changeVolume((float)a/4);
            o_audio[a]->changeFashion("selected");
            tomatoes->getSettings().getRoot()["audio"] = a;
            engine->getLibrary()->getObjectById("menusignes")->changeFashion(a?"sound":"nosound");
        }

    }
}


/**
 * Handle the level1 event
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Menu::onLevel1(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED)
{
    Uint8 *             keystates = tomatoes->getKeyboard().SDL_GetKeyState(NULL);
    std::stringstream   s1;
    std::stringstream   s2;

    if( keystates[' '] || keystates['l'] || keystates['e'] )
    {
        switch(level1Value)
        {
            case 0:     tomatoes->setMulti(false);
                        playerReady[0] = false; playerReady[1] = true;
                        s1<<"char"<<(tomatoes->getPlayerId(0)+1);
                        a_player[0]->changeTimeline(s1.str());
                        a_player[1]->changeTimeline("none");
                        engine->changeTimeline("t_level1ToLevel2");
                        break;
            case 1:     tomatoes->setMulti(true);
                        playerReady[0] = playerReady[1] = false;
                        s1<<"char"<<(tomatoes->getPlayerId(0)+1);
                        a_player[0]->changeTimeline(s1.str());
                        s2<<"char"<<(tomatoes->getPlayerId(1)+1);
                        a_player[1]->changeTimeline(s2.str());
                        engine->changeTimeline("t_level1ToLevel2");
                        break;
            case 2:     updateBest(tomatoes->getSettings().getRoot()); engine->changeTimeline("t_level1ToBest"); break;
            case 3:     updateOption(); engine->changeTimeline("t_level1ToLevel14"); break;
            case 4:     engine->changeTimeline("t_level1ToCredits"); break;
            case 5:     engine->changeTimeline("t_level1ToEnd");break;
            default:    break;
        };
    }
    else
    {
        int value = level1Value;

        if( keystates[ SDLK_DOWN ] )    { if (value<4) { value += 2; } }
        if( keystates[ SDLK_UP ] )      { if (value>1) { value -= 2; } }
        if( keystates[ SDLK_LEFT ] )    { value &= 254; }
        if( keystates[ SDLK_RIGHT ] )   { value |= 1; }

        if (value!=level1Value)
        {
            updateLevel1(value, _timeStampInMilliSeconds);
        }
    }
}

/**
 * Handle the players selection event
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Menu::onLevel2(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED)
{
    Uint8 *keystates = tomatoes->getKeyboard().SDL_GetKeyState(NULL);
    if( keystates[SDLK_ESCAPE] ) { engine->changeTimeline("t_level2ToLevel1"); }
    else
    {
        // FIRST PLAYER
        if (!playerReady[0])
        {
            if (keystates['l'] || keystates[' ']) { playerReady[0] = true; a_player[0]->changeTimeline("ready"); }

            int     v1 = tomatoes->getPlayerId(0);
            bool    m1 = false;
            if( keystates[ SDLK_LEFT ] )    { v1 = (v1-1+Game::nbModels)%Game::nbModels; m1=true;}
            if( keystates[ SDLK_RIGHT ] )   { v1 = (v1+1)%Game::nbModels; m1=true;}
            if (m1)
            {
                std::stringstream s1;
                s1<<"char"<<(v1+1);
                a_player[0]->changeTimeline(s1.str());
                tomatoes->setPlayerId(0, v1);
            }
        }

        // SECOND PLAYER IF ANY
        if (tomatoes->multi())
        {
            if (!playerReady[1])
            {
                if (keystates['e']) { playerReady[1] = true; a_player[1]->changeTimeline("ready"); }

                int     v2 = tomatoes->getPlayerId(1);
                bool    m2 = false;
                if( keystates['d'] )    { v2 = (v2-1+Game::nbModels)%Game::nbModels; m2=true;}
                if( keystates['g'] )    { v2 = (v2+1)%Game::nbModels; m2=true;}
                if (m2)
                {
                    std::stringstream s2;
                    s2<<"char"<<(v2+1);
                    a_player[1]->changeTimeline(s2.str());
                    tomatoes->setPlayerId(1, v2);
                }
            }
        }

        // CHECK IF BOTH PLAYERS ARE READY
        if (playerReady[0] && playerReady[1])
        {
            tomatoes->resetLevel();
            engine->changeTimeline("t_level2toGame");
        }
    }
}


/**
 * Launch the game
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Menu::onLevel3(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED)
{
    if (tomatoes->getGame())
    {
        if (tomatoes->getGame()->isReady())
        {
            // LAUNCH THE GAME
            tomatoes->launchGame();
        }
    }
    else
    {
        if (tomatoes->incLevel())
        {
            // CREATE THE GAME
            tomatoes->createGame(tomatoes->getLevel());
        }
        else
        {
            // FINISHED
            engine->changeTimeline("t_menu");
        }
    }

}

/**
 * Handle the best score event
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Menu::onLevel10(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED)
{
    Uint8 *keystates = tomatoes->getKeyboard().SDL_GetKeyState(NULL);
    if( keystates[SDLK_ESCAPE] || keystates[' '] || keystates['l'] || keystates['e'] )
        { engine->changeTimeline("t_bestToLevel1"); }
}


/**
 * Handle the credit event
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Menu::onLevel13(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED)
{
    Uint8 *keystates = tomatoes->getKeyboard().SDL_GetKeyState(NULL);

    if( keystates[SDLK_ESCAPE] || keystates[' ']|| keystates['e'] || keystates['l'] )
        { engine->changeTimeline("t_creditsToLevel1"); }
}


/**
 * Handle the options event
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Menu::onLevel14(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED)
{
    Uint8 *keystates = tomatoes->getKeyboard().SDL_GetKeyState(NULL);

    if( keystates[SDLK_ESCAPE] || keystates[' '] || keystates['e'] || keystates['l'] )
    {
        engine->changeTimeline("t_Level14ToLevel1");
    }
    else
    {
        bool update = false;
        int value = option;
        // if( keystates[ SDLK_DOWN ] )    { if (value<1) { value++; update = true;} }
        // if( keystates[ SDLK_UP ] )      { if (value>0) { value--; update = true; } }

        int value2;
        switch(value)
        {
            case 0: value2 = tomatoes->getSettings().getRoot()["audio"]; break;
        };
        if( keystates[ SDLK_LEFT ] )    { if (value2>0) { value2--; update = true; } }
        if( keystates[ SDLK_RIGHT ] )   { if (value2<4) { value2++; update = true; } }

        if (update) { updateOption(value, value2, _timeStampInMilliSeconds); }
    }
}



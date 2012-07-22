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

#include <tomatoes/Main.hpp>
#include <tomatoes/Game.hpp>

#include <libconfig.h++>

#include <iostream>
#include <iomanip>
#include <getopt.h>

using namespace tomatoes;

/**
 * Run the player
 */
void Tomatoes::run()
{
    bool rc = true;

    // READ THE SETTINGS CONFIGURATION FILE
    settings.scoreValues[5] = 500;
    settings.scoreValues[6] = 400;
    settings.scoreValues[7] = 250;
    settings.scoreValues[8] = 200;
    settings.scoreValues[9] = 150;
    settings.load();

    // INIT THE SPLASHOUILLE FRAMEWORK
    splashouille::Engine::init();

    // CREATE THE SDL WINDOW
    screen = SDL_SetVideoMode(screenSize[0], screenSize[1], screenDepth, SDL_SWSURFACE | SDL_DOUBLEBUF );
    SDL_WM_SetCaption("Tomatoes", "Tomatoes");

    // DEFINE THE WINDOW ICON
    SDL_Surface*    icon     = SDL_LoadBMP("res/img/icon.bmp");
    if (icon)
    {
        Uint32          colorKey = SDL_MapRGB(icon->format, 255, 0, 255);
        SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colorKey);
        SDL_WM_SetIcon(icon, NULL);
    }

    // OPEN AND PARSE THE CONFIGURATION FILE
    libconfig::Config * configuration   = new libconfig::Config();
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));SDL_Flip(screen); 

    try { configuration->readFile("res/conf/tomatoes.ini"); }
        catch(libconfig::FileIOException e) { std::cerr<<e.what()<<std::endl; rc = false; }
        catch(libconfig::ParseException  e) { std::cerr<<e.what()<<std::endl; rc = false; }

    // INITIALIZE THE DIFFERENT COMPONENTS OF TOMATOES
    if (rc) { menu.run(configuration, screen); }

    // SAVE THE CONFIGURATION
    settings.save();
}


bool Tomatoes::launchGame()
{
    bool rc = (game);

    if (rc) { game->run(screen); delete game; }

    return rc;
}

void Tomatoes::createGame(int _level)
{
    game = new Game(this);
    game->init(_level);
}

/**
 * The player main function
 * @param argc is the number of arguments
 * @param argv is the argument values
 * @return 1
 */
int main()
{
    std::cout<<"Tomatoes 2012"<<std::endl;

    // INSTANCE THE GAME ENGINE AND RUN!!!
    Tomatoes * game = new Tomatoes();
    game->run();

    delete game;

    return 1;

}
